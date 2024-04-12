#include <Wire.h>
#include <algorithm>

// Pin Definitions
#define SENSOR_INPUT_PIN A0   // Data Wire is plugged into ADC Port
#define GPIO_PIN 15           // Power Port is GPIO PIN 15

// Timing Intervals
#define POLLING_INTERVAL 60000   // Interval for polling
#define SAMPLING_INTERVAL 100000  // Interval for sampling
#define SENDING_INTERVAL 37000    // Interval for sending

// Definition for tds sensor reading iterations (sampling points)
#define tdsSenseIterations  40            // sum of sample point

// Global Variables
float adjustedTds = 0;           // Adjusted TDS calculation
float VC = 3.3;                  // voltage Constant
int slaveAddress = 8;            // Slave device address
bool codeExecuted = false;       // Flag for code execution
const int16_t I2C_SLAVE = 8;     // I2C slave address
float receivedFloatValue;        // Received float value
float receivedFloatTemp;         // Received temperature value
uint8_t messageType = 0x03;      // Message type
uint8_t messageLength = 8;       // Message length
float analogBuffer[tdsSenseIterations];     // store the analog value in the array, read from ADC
int analogBufferIndex = 0;       // Index for circular buffer

// Constants for temperature coefficient calculation
const float tempCoefficient = 0.02;  // 2% Coefficient calculation
const float referenceTemp = 25.0;    // Reference temperature for calibration

// Function Prototypes
void setup();
void loop();
float adjustTds(float sensor, float temperature);
float computeMedian();
void transmitSlave();
void startSensor();
void stopSensor();
void actuateData();
float readFloatFromWire(uint8_t length);

// Configure State Machine
enum SensorState {
    SENSOR_OFF,
    SENSOR_INIT,
    SENSOR_STABILIZE,
    SENSOR_READ,
    SENSOR_SHUTDOWN
};

SensorState currentState = SENSOR_OFF;
unsigned long stateStartTime = 0;

void setup() {
    // Initialize Serial Communication
    Serial.begin(9600);
    while (!Serial); // Wait for serial port to connect

    // Set GPIO PIN as OUTPUT for controlling power
    pinMode(GPIO_PIN, OUTPUT);

    // Initialize Wire library
    Wire.begin();

    // Execute code only once
    if (!codeExecuted) {
        delay(5000);
        Serial.println("Setup Complete.");
        codeExecuted = true;
    }
}

void loop() {

    switch (currentState) {
        case SENSOR_OFF:
            // Do nothing until instructed to turn on the sensor
            break;
        case SENSOR_INIT:
            // Initialize sensor
            digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
            stateStartTime = millis();
            currentState = SENSOR_STABILIZE;
            break;
        case SENSOR_STABILIZE:
            // Wait for sensor stabilization
            if (millis() - stateStartTime >= 5000) { // Wait for 5 seconds
                currentState = SENSOR_READ; // Move to next state
            }
            break;
        case SENSOR_READ:
            // Read sensor data
            adjustedTds = readAndAdjustTds(receivedFloatTemp); // Replace 'temperature' with the actual temperature value to adjust the TDS
            delay(250);
            if (millis() - stateStartTime >= 15000) { // Wait for 10 seconds
              currentState = SENSOR_SHUTDOWN; // Move to next state
            }
            // Serial.println(adjustedTds); // Uncomment for debugging
            break; 
        case SENSOR_SHUTDOWN:
            // Power off the sensor
            digitalWrite(GPIO_PIN, LOW);
            currentState = SENSOR_OFF; // Reset to initial state
            break;
    }
    
    // Polling Interval Check
    static unsigned long pollingTime = millis();
    if (millis() - pollingTime > POLLING_INTERVAL) {
        actuateData();
        pollingTime = millis();
        }

    // Sampling Interval Check
    static unsigned long samplingTime = millis();
    if (millis() - samplingTime > SAMPLING_INTERVAL) {
        startSensor();
        samplingTime = millis();
    }

    // Sending Interval Check
    static unsigned long sendingTime = millis();
    if (millis() - sendingTime > SENDING_INTERVAL) {
        transmitSlave(); // Transmit TDS to slave device
        sendingTime = millis();
    }
}

void startSensor() {
    currentState = SENSOR_INIT;
}

void stopSensor() {
    currentState = SENSOR_OFF;
}

void actuateData() {
    if (Wire.requestFrom(I2C_SLAVE, 8) != 8) {
        // Error: Did not receive expected number of bytes
        // Serial.println("Error retrieving temperature.");
        return;
    }

    // Request 8 bytes from slave device #8
    uint8_t receivedMessageType = Wire.read(); // Read the message type byte
    uint8_t receivedMessageLength = Wire.read(); // Read the message length byte

    // Handle message type 0x02 (temperature value)
    if (receivedMessageType == 0x02 && receivedMessageLength == 8) {
        receivedFloatTemp = readFloatFromWire(receivedMessageLength);
    }
}

float readFloatFromWire(uint8_t length) {
    uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float
    for (int i = 0; i < length; i++) {
        byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
    }
    // Reconstruct the float value from byteArray
    float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
    return *ptr; // Return reconstructed float value
}


// Function to read analog value from sensor and store in buffer
void analogReadAction() {
    float sensorValue = analogRead(SENSOR_INPUT_PIN);
    // Serial.println(sensorValue); // Uncomment for debugging
    analogBuffer[analogBufferIndex] = sensorValue;
    analogBufferIndex = (analogBufferIndex + 1) % tdsSenseIterations; // Circular buffer       
}

// Function to compute median value from buffer
float computeMedian() {
    float sortedBuffer[tdsSenseIterations];
    memcpy(sortedBuffer, analogBuffer, tdsSenseIterations * sizeof(float));
    std::sort(sortedBuffer, sortedBuffer + tdsSenseIterations);
    
    if (tdsSenseIterations % 2 == 0) {
        float median = (sortedBuffer[tdsSenseIterations / 2 - 1] + sortedBuffer[tdsSenseIterations / 2]) / 2.0f;
        return median;
    } else {
        return sortedBuffer[tdsSenseIterations / 2];
    }
}

// Function to adjust TDS reading based on temperature
float adjustTds(float voltage, float temperature) {
    // Calculate temperature correction factor
    float tempCorrection = 1.0 + tempCoefficient * (temperature - referenceTemp);
    
    // Calculate compensation voltage
    float compensationVoltage = voltage / tempCorrection;

    // Adjust TDS reading based on temperature
    return (133.42 / compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
}

// Combined function for reading analog value and adjusting TDS
float readAndAdjustTds(float temperature) {
    analogReadAction();
    float medianSensorValue = computeMedian();
    float averageVoltage = medianSensorValue * VC / 1024.0;
    return adjustTds(averageVoltage, temperature);
}

// Transmit to slave
void transmitSlave() {
    Wire.beginTransmission(slaveAddress); // Start I2C transmission with slave (Arduino)
    Wire.write(messageType); // Send message type
    Wire.write(messageLength); // Send message length
    uint8_t byteArray[sizeof(float)]; // Create byte array to store TDS
    memcpy(byteArray, &adjustedTds, sizeof(float)); // Copy TDS value to byte array

    for (int i = 0; i < sizeof(float); i++) {
        Wire.write(byteArray[i]); // Send TDS byte by byte
    }

    Wire.endTransmission(); // End transmission
}
