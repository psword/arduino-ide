#include <Wire.h>

// Pin Definitions
#define SENSOR_INPUT_PIN A0   // Data Wire is plugged into ADC Port
#define GPIO_PIN 15           // Power Port is GPIO PIN 15

// Timing Intervals
#define POLLING_INTERVAL 120000   // Interval for polling
#define SAMPLING_INTERVAL 40000  // Interval for sampling
#define SENDING_INTERVAL 37000    // Interval for sending

// Global Variables
int sensorValue = 0;             // Variable to store sensor value
float tdsValue = 0;               // Variable to store TDS value
float adjustedTds = 0;           // Adjusted TDS calculation
float Voltage = 0;               // Variable to store voltage
float VC = 3.3;                   // Voltage Constant
int slaveAddress = 8;            // Slave device address
String FeatherID = "F1: ";       // Device ID
bool codeExecuted = false;       // Flag for code execution
const int16_t I2C_SLAVE = 8;     // I2C slave address
float receivedFloatValue;        // Received float value
float receivedFloatTemp;         // Received temperature value
uint8_t messageType = 0x03;      // Message type
uint8_t messageLength = 8;       // Message length

// Constants for temperature coefficient calculation
const float tempCoefficient = 0.02;  // 2% Coefficient calculation
const float referenceTemp = 25.0;    // Reference temperature for calibration

// Function Prototypes
void setup();
void loop();
float adjustTds(float rawTDS, float temperature);
void transmitSlave();

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
        int tdsSenseIterations = 40;
        digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
        delay(1000); // Wait for sensor stabilization

        // Perform multiple readings for calibration
        for (int i = 0; i < tdsSenseIterations; i++) {
            delay(500); // Wait for sensor stabilization
            sensorValue = analogRead(SENSOR_INPUT_PIN); // Read analog value from sensor
            Voltage = sensorValue * VC / 1024.0; // Convert analog reading to voltage
            tdsValue = (133.42 / Voltage * Voltage * Voltage - 255.86 * Voltage * Voltage + 857.39 * Voltage) * 0.5; // Convert voltage value to TDS value
        }

        digitalWrite(GPIO_PIN, LOW); // Power off the sensor
        Serial.print("Setup Complete.");
        codeExecuted = true;
    }
}

void loop() {
    static unsigned long pollingTime = millis();

    // Polling Interval Check
    if (millis() - pollingTime > POLLING_INTERVAL) {
        Wire.requestFrom(I2C_SLAVE, 8); // Request 8 bytes from slave device #8

        while (Wire.available()) { // Slave may send less than requested
            uint8_t receivedMessageType = Wire.read(); // Read the message type byte
            uint8_t receivedMessageLength = Wire.read(); // Read the message length byte

            // Handle message type 0x02 (temperature value)
            if (receivedMessageType == 0x02 && receivedMessageLength == 8) {
                uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float

                for (int i = 0; i < receivedMessageLength; i++) {
                    byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
                }

                // Reconstruct the float value from byteArray
                float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
                receivedFloatValue = *ptr; // Assign the reconstructed float value
                receivedFloatTemp = receivedFloatValue;
            }
        }

        pollingTime = millis();
    }

    static unsigned long samplingTime = millis();

    // Sampling Interval Check
    if (millis() - samplingTime > SAMPLING_INTERVAL) {
        digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
        delay(1500); // Wait for sensor stabilization
        sensorValue = analogRead(SENSOR_INPUT_PIN); // Read analog value from sensor
        Voltage = sensorValue * VC / 1024.0; // Convert analog reading to voltage
        tdsValue = (133.42 / Voltage * Voltage * Voltage - 255.86 * Voltage * Voltage + 857.39 * Voltage) * 0.5; // Convert voltage value to TDS value
        adjustedTds = adjustTds(tdsValue, receivedFloatTemp); // Calculate the adjusted TDS
        delay(5000); // Wait for sensor stabilization
        digitalWrite(GPIO_PIN, LOW); // Power off the sensor
        samplingTime = millis();
    }

    static unsigned long sendingTime = millis();

    // Sending Interval Check
    if (millis() - sendingTime > SENDING_INTERVAL) {
        transmitSlave(); // Transmit temperature to slave device
        sendingTime = millis();
    }
}

// Function to calculate adjusted TDS reading based on temperature
float adjustTds(float rawTDS, float temperature) {
    // Calculate temperature correction factor
    float tempCorrection = 1.0 + tempCoefficient * (temperature - referenceTemp);
    // Adjust TDS reading based on temperature
    float adjustedTds = rawTDS + tempCorrection;
    return adjustedTds;
}

// Transmit to slave
void transmitSlave() {
    Wire.beginTransmission(slaveAddress); // Start I2C transmission with slave (Arduino)
    Wire.write(messageType); // Send message type
    Wire.write(messageLength); // Send message length
    uint8_t byteArray[sizeof(float)]; // Create byte array to store temperature
    memcpy(byteArray, &adjustedTds, sizeof(float)); // Copy temperature value to byte array

    for (int i = 0; i < sizeof(float); i++) {
        Wire.write(byteArray[i]); // Send temperature byte by byte
    }

    Wire.endTransmission(); // End transmission
}
