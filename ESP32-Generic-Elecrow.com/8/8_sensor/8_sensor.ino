/* Written with PlatformIO
Intended for ESP32 Board */

// #include <Arduino.h>

#include <Wire.h>               // Wire Library
#include <OneWire.h>            // DS18B20 Temperature Sensor
#include <DallasTemperature.h>  // DS18B20 Temperature Sensor
#include <DFRobot_ESP_PH.h>     // DFRobot pH Library v2.0 https://github.com/greenponik/DFRobot_ESP_PH_BY_GREENPONIK
#include <EEPROM.h>             // EEPROM library
#include "OnewireTemperature.h" // Temperature Sensor Class
#include "AnalogTds.h"          // TDS sensor class
#include "AnalogpH.h"           // pH sensor class

// Pin definitions
#define ONE_WIRE_BUS 2   // Data wire is plugged into port 2
#define TEMP_GPIO_PIN 17 // Power Port is GPIO PIN 17
#define TDS_SENSOR_BUS 4 // Data Wire is plugged into port 4
#define TDS_GPIO_PIN 16  // Power Port is GPIO PIN 16
#define PH_GPIO_PIN 15   // Power Port is GPIO PIN 15
#define PH_SENSOR_BUS 13 // The ADC Pin for the ADS1115 (must be 0, 1, 2, 3)

// Global variables
float adjustedTemp = 0;        // Variable to store temperature
float adjustedTds = 0;         // Variable to store TDS
float adjustedpH = 0;          // Variable to store pH
float sendFloatTempValue;      // Float value to send
float sendFloatTdsValue;       // Float value to send
float sendFloatpHValue;        // Float value to send
bool codeExecuted = false;     // Flag for code execution
bool sendMessageFlag1 = false; // Flag to indicate relay message
bool sendMessageFlag2 = false; // Flag to indicate relay message
bool sendMessageFlag3 = false; // Flag to indicate relay message
uint8_t messageType1 = 0x01;   // Message type identifier
uint8_t messageType2 = 0x02;   // Message type identifier
uint8_t messageType3 = 0x03;   // Message type identifier
uint8_t messageLength = 8;     // Length of message to transmit
uint8_t slaveAddress = 0x08;   // I2C Slave address

// Define sampling intervals for sensors
#define SAMPLING_INTERVAL 60000 // Interval for sampling (milliseconds)
#define READING_DURATION 12000  // Duration for sampling (milliseconds)

// Define state machine states for each sensor
enum SensorState
{
    SENSOR_OFF,
    SENSOR_INIT,
    SENSOR_STABILIZE,
    SENSOR_READ,
    SENSOR_SHUTDOWN
};

SensorState tempSensorState = SENSOR_OFF;
SensorState tdsSensorState = SENSOR_OFF;
SensorState pHSensorState = SENSOR_OFF;
unsigned long tempStateStartTime = 0;
unsigned long tdsStateStartTime = 0;
unsigned long pHStateStartTime = 0;

// Function prototypes
void startTemperatureSensor();
void stopTemperatureSensor();
void startTDSSensor();
void stopTDSSensor();
void startPHSensor();
void stopPHSensor();
void temperatureSensorStateMachine();
void tdsSensorStateMachine();
void pHSensorStateMachine();
void transmitSlave();

// Create an instance of TemperatureSensor (onewire PIN, number of samples)
// Measurement time ~1.00 seconds due to onewire library
// Using the default value (10) for tempSenseIterations
TemperatureSensor tempSensorInstance(ONE_WIRE_BUS);

// Providing a custom value for tempSenseIterations (e.g., 15)
// TemperatureSensor tempSensorInstance(ONE_WIRE_BUS, 15);

// Create an instance of TdsSensor (voltage, kCoefficient, reference temp, ADC bits, GPIO PIN, number of samples, delay between read)
// Using the default value (10) for tdsSenseIterations and 250ms for read delay
// TdsSensor tdsSensorInstance(3.3, 0.02, 25.0, 4096, TDS_SENSOR_BUS);

// Providing a custom value for tdsSenseIterations (e.g., 15) and measurement delay
TdsSensor tdsSensorInstance(3.3, 0.02, 25.0, 4096, TDS_SENSOR_BUS, 15, 650);

// Create an instance of pHSensor (voltage in 1000 units (3300 for ESP32), reference temp, ADC bits, GPIO PIN, number of samples, delay between read)
// Using 5V passthrough with ESP32 will result in a negative pH value and disables calibration features
// Using the default value (40) for pHSenseIterations and 250ms default for measurement delay
// pHSensor pHSensorInstance(3300, 25.0, 4096, PH_SENSOR_BUS);

// Providing a custom value of pHSense Iterations and measurement delay
pHSensor pHSensorInstance(3300, 25.0, 4096, PH_SENSOR_BUS, 15, 650);

void setup()
{
    Serial.begin(9600); // Start serial port for debugging
    // Wait for serial port to connect
    while (!Serial)
    {
        ;
    }
    pinMode(TEMP_GPIO_PIN, OUTPUT);    // Set GPIO PIN as OUTPUT for controlling power
    pinMode(TDS_GPIO_PIN, OUTPUT);     // Set GPIO PIN as OUTPUT for controlling power
    pinMode(PH_GPIO_PIN, OUTPUT);      // Set GPIO PIN as OUTPUT for controlling power
    pinMode(ONE_WIRE_BUS, INPUT);      // Set GPIO PIN as INPUT for reading data
    pinMode(TDS_SENSOR_BUS, INPUT);    // Set GPIO PIN as INPUT for reading data
    pinMode(PH_SENSOR_BUS, INPUT);     // Set GPIO PIN as INPUT for reading data
    EEPROM.begin(32);                  // Needed for eeprom
    Wire.begin();          // Join I2C bus as master device (message sender)
    tempSensorInstance.beginSensors(); // Start up the temperature sensor library
    pHSensorInstance.beginSensors();   // Start up the pH sensor library

    // Perform initialization if code has not been executed before

    if (!codeExecuted)
    {
        delay(10000);
        Serial.println("Setup Complete."); // Print setup message to serial monitor

        codeExecuted = true; // Set code execution flag
    }
}

void loop()
{
    // Sampling temperature at regular intervals begins the process
    static unsigned long tempSamplingTime = millis();
    if (millis() - tempSamplingTime > SAMPLING_INTERVAL)
    {
        startTemperatureSensor();
        tempSamplingTime = millis();
    }
    // Execute the state machine operations
    temperatureSensorStateMachine();
    tdsSensorStateMachine();
    pHSensorStateMachine();
}

void startTemperatureSensor()
{
    tempSensorState = SENSOR_INIT;
}

void stopTemperatureSensor()
{
    tempSensorState = SENSOR_OFF;
}

void startTDSSensor()
{
    tdsSensorState = SENSOR_INIT;
}

void stopTDSSensor()
{
    tdsSensorState = SENSOR_OFF;
}

void startPHSensor()
{
    pHSensorState = SENSOR_INIT;
}

void stopPHSensor()
{
    pHSensorState = SENSOR_OFF;
}

void temperatureSensorStateMachine()
{
    switch (tempSensorState)
    {
    case SENSOR_OFF:
        // Do nothing until instructed to turn on the sensor
        break;
    case SENSOR_INIT:
        // Initialize sensor
        tempStateStartTime = millis();
        digitalWrite(TEMP_GPIO_PIN, HIGH); // Power on the sensor
        tempSensorState = SENSOR_STABILIZE;
        break;
    case SENSOR_STABILIZE:
        // Wait for sensor stabilization
        if (millis() - tempStateStartTime >= 2000)
        {                                  // Wait for 2 seconds
            tempSensorState = SENSOR_READ; // Move to next state
        }
        break;
    case SENSOR_READ:
        // Read sensor data
        adjustedTemp = tempSensorInstance.readAndAdjustTemp(); // Read and adjust temperature
        // Serial.println(adjustedTemp);      // Uncomment for debugging
        // By default the library reads every ~1s, so the duration must be timed correctly according to buffer size
        if (millis() - tempStateStartTime >= READING_DURATION)
        {                                      // Waiting period
            Serial.println(adjustedTemp);      // Uncomment for debugging
            tempSensorState = SENSOR_SHUTDOWN; // Move to next state
        }
        break;
    case SENSOR_SHUTDOWN:
        // Power off the sensor
        digitalWrite(TEMP_GPIO_PIN, LOW);
        sendFloatTempValue = adjustedTemp;
        sendMessageFlag1 = true;
        transmitSlave();
        stopTemperatureSensor(); // Reset to initial state
        startTDSSensor();
        break;
    }
}

void tdsSensorStateMachine()
{
    static unsigned long lastReadTime = 0; // To track the last read time

    switch (tdsSensorState)
    {
    case SENSOR_OFF:
        // Do nothing until instructed to turn on the sensor
        break;
    case SENSOR_INIT:
        // Initialize sensor
        tdsStateStartTime = millis();
        digitalWrite(TDS_GPIO_PIN, HIGH); // Power on the sensor
        tdsSensorState = SENSOR_STABILIZE;
        break;
    case SENSOR_STABILIZE:
        // Wait for sensor stabilization
        if (millis() - tdsStateStartTime >= 2000)
        {                                 // Wait for 2 seconds
            tdsSensorState = SENSOR_READ; // Move to next state
            lastReadTime = millis();      // Initialize lastReadTime
        }
        break;
    case SENSOR_READ:
        // Read sensor data
        if (millis() - lastReadTime >= tdsSensorInstance.getReadDelay())
        {
            adjustedTds = tdsSensorInstance.readAndAdjustTds(adjustedTemp);
            // Serial.println(adjustedTds); // Uncomment for debugging
            lastReadTime = millis(); // Update the last read time
        }

        if (millis() - tdsStateStartTime >= READING_DURATION)
        {                                     // Waiting period
            Serial.println(adjustedTds);      // Uncomment for debugging
            tdsSensorState = SENSOR_SHUTDOWN; // Move to next state
        }
        break;
    case SENSOR_SHUTDOWN:
        // Power off the sensor
        digitalWrite(TDS_GPIO_PIN, LOW);
        sendFloatTdsValue = adjustedTds;
        sendMessageFlag2 = true;
        transmitSlave();
        stopTDSSensor(); // Reset to initial state
        startPHSensor();
        break;
    }
}

void pHSensorStateMachine()
{
    static unsigned long lastReadTime = 0; // To track the last read time

    switch (pHSensorState)
    {
    case SENSOR_OFF:
        // Do nothing until instructed to turn on the sensor
        break;
    case SENSOR_INIT:
        // Initialize sensor
        pHStateStartTime = millis();
        digitalWrite(PH_GPIO_PIN, HIGH); // Power on the sensor
        pHSensorState = SENSOR_STABILIZE;
        break;
    case SENSOR_STABILIZE:
        // Wait for sensor stabilization
        if (millis() - pHStateStartTime >= 2000)
        {                                // Wait for 2 seconds
            pHSensorState = SENSOR_READ; // Move to next state
            lastReadTime = millis();     // Initialize lastReadTime
        }
        break;
    case SENSOR_READ:
        // Read sensor data
        if (millis() - lastReadTime >= pHSensorInstance.getReadDelay())
        {
            adjustedpH = pHSensorInstance.computePHValue(adjustedTemp);
            // Serial.println(adjustedpH); // Uncomment for debugging
            lastReadTime = millis(); // Update the last read time
        }
        if (millis() - pHStateStartTime >= READING_DURATION) 
        {                                                    // Waiting period
            Serial.println(adjustedpH);                      // Uncomment for debugging
            pHSensorState = SENSOR_SHUTDOWN;                 // Move to next state
        }
        break;
    case SENSOR_SHUTDOWN:
        // Power off the sensor
        digitalWrite(PH_GPIO_PIN, LOW);
        sendFloatpHValue = adjustedpH;
        sendMessageFlag3 = true;
        transmitSlave();
        stopPHSensor(); // Reset to initial state
        break;
    }
}

void transmitSlave()
{
    // Helper function to transmit data
    auto transmitData = [](uint8_t messageType, float data)
    {
        Wire.beginTransmission(slaveAddress); // Start I2C transmission with slave
        Wire.write(messageType);              // Send message type
        Wire.write(messageLength);            // Send message length

        uint8_t byteArray[sizeof(float)];        // Create byte array to store data
        memcpy(byteArray, &data, sizeof(float)); // Copy data value to byte array

        for (int i = 0; i < sizeof(float); i++)
        {
            Wire.write(byteArray[i]); // Send data byte by byte
        }
        uint8_t error = Wire.endTransmission(); // End transmission and capture error

        // Debugging print
        Serial.print("Transmitted message type ");
        Serial.print(messageType);
        Serial.print(" with data: ");
        Serial.print(data);
        Serial.print(" Error: ");
        Serial.println(error);
    };

    if (sendMessageFlag1)
    {
        transmitData(messageType1, adjustedTemp);
        sendMessageFlag1 = false;
    }
    if (sendMessageFlag2)
    {
        transmitData(messageType2, adjustedTds);
        sendMessageFlag2 = false;
    }
    if (sendMessageFlag3)
    {
        transmitData(messageType3, adjustedpH);
        sendMessageFlag3 = false;
    }
}
