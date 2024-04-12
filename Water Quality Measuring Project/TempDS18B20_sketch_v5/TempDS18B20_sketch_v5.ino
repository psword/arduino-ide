#include <Wire.h>
#include <OneWire.h>             // Library for Signal on Digital Pin for OneWire Device
#include <DallasTemperature.h>   // Library for Temperature Probe

// Pin definitions
#define ONE_WIRE_BUS 2           // Data wire is plugged into port 2 on the Feather
#define GPIO_PIN 15              // Power Port is GPIO PIN 15

// Sampling and sending intervals
#define samplingInterval 100000   // Interval for sampling (milliseconds)
#define sendingInterval 30000    // Interval for sending (milliseconds)

// Definition for temperature sensor reading iterations (sampling points)
#define tempSenseIterations  40            // Number of sample points

// Initialize OneWire and DallasTemperature instances
OneWire oneWire(ONE_WIRE_BUS);             // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);        // Pass oneWire reference to Dallas Temperature

// Global variables
float adjustedTemp = 0;                     // Variable to store temperature
int slaveAddress = 8;                       // Define slave address for I2C communication
uint8_t messageType = 0x02;                 // Message type correlates with boot sequence
uint8_t messageLength = 8;                  // Length of the message
bool codeExecuted = false;                   // Flag for code execution
float analogBuffer[tempSenseIterations];     // Array to store temperature readings
int analogBufferIndex = 0;                   // Index for circular buffer

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

// Function prototypes
void setup(void);
void loop();
void startSensor();
void stopSensor();
void analogReadAction();
float computeMedian();
float readAndAdjustTemp();
void transmitSlave();

void setup(void) {
  Serial.begin(9600);                        // Start serial port for debugging
  // Wait for serial port to connect
  while (!Serial) {
    ;
  }
  pinMode(GPIO_PIN, OUTPUT);                 // Set GPIO PIN as OUTPUT for controlling power
  Wire.begin();                              // Join I2C bus as master device (message sender)
  sensors.begin();                           // Start up the temperature sensor library

  // initialize digital pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);

  // Perform initialization if code has not been executed before
  if (!codeExecuted) {
    delay(5000);                             // Brief setup delay
    Serial.println("Setup Complete.");       // Print setup message to serial monitor
    codeExecuted = true;                     // Set code execution flag
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
            adjustedTemp = readAndAdjustTemp(); // Read and adjust temperature
            if (millis() - stateStartTime >= 45000) { // Wait for 40 seconds
              currentState = SENSOR_SHUTDOWN; // Move to next state
            }
            // Serial.println(adjustedTemp); // Uncomment for debugging
            break; 
        case SENSOR_SHUTDOWN:
            // Power off the sensor
            digitalWrite(GPIO_PIN, LOW);
            currentState = SENSOR_OFF; // Reset to initial state
            break;
    }
  
  // Sampling temperature at regular intervals
  static unsigned long samplingTime = millis(); // Set the sampling time
  if (millis() - samplingTime > samplingInterval) {
    startSensor();
    samplingTime = millis();                  // Reset the sampling time
  }

  // Sending temperature to slave device at regular intervals
  static unsigned long sendingTime = millis(); // Set the sending time
  if (millis() - sendingTime > sendingInterval) {
    transmitSlave();                          // Transmit temperature to slave device
    sendingTime = millis();                   // Reset the sending time
  }
}

void startSensor() {
  currentState = SENSOR_INIT;
}

void stopSensor() {
    currentState = SENSOR_OFF;
}

void analogReadAction() {
    sensors.requestTemperatures();           // Send command to get temperatures
    float sensorValue = sensors.getTempCByIndex(0);       // Read temperature value
    // Serial.print("Raw Read: "); // Uncomment for debugging
    // Serial.println(sensorValue); // Uncomment for debugging
    analogBuffer[analogBufferIndex] = sensorValue;
    analogBufferIndex = (analogBufferIndex + 1) % tempSenseIterations; // Circular buffer  
}

// Function to compute median value from buffer
float computeMedian() {
    float sortedBuffer[tempSenseIterations];
    memcpy(sortedBuffer, analogBuffer, tempSenseIterations * sizeof(float));
    std::sort(sortedBuffer, sortedBuffer + tempSenseIterations);
    
    if (tempSenseIterations % 2 == 0) {
        float median = (sortedBuffer[tempSenseIterations / 2 - 1] + sortedBuffer[tempSenseIterations / 2]) / 2.0f;
        return median;
    } else {
        return sortedBuffer[tempSenseIterations / 2];
    }
}

// Combined function for reading analog value and adjusting temperature
float readAndAdjustTemp() {
    analogReadAction();
    float medianSensorValue = computeMedian();
    return medianSensorValue;
}

// Transmit temperature value to slave device
void transmitSlave() {
  Wire.beginTransmission(slaveAddress);       // Start I2C transmission with slave (Arduino)
  Wire.write(messageType);                    // Send message type
  Wire.write(messageLength);                  // Send message length
  
  uint8_t byteArray[sizeof(float)];          // Create byte array to store temperature
  memcpy(byteArray, &adjustedTemp, sizeof(float));    // Copy temperature value to byte array
  
  for (int i = 0; i < sizeof(float); i++) {
    Wire.write(byteArray[i]);                 // Send temperature byte by byte
  }
  
  Wire.endTransmission();                     // End transmission
}
