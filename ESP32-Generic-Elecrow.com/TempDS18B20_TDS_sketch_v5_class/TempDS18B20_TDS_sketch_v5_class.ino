// #include <Arduino.h>

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "OnewireTemperature.h" // Temperature Sensor Class
#include "AnalogTds.h"          // TDS sensor class
#include "AnalogpH.h"           // pH sensor class

// Pin definitions
#define ONE_WIRE_BUS 2   // Data wire is plugged into port 2
#define TEMP_GPIO_PIN 17 // Power Port is GPIO PIN 17
#define TDS_SENSOR_BUS 4 // Data Wire is plugged into port 4
#define TDS_GPIO_PIN 16  // Power Port is GPIO PIN 16
#define PH_SENSOR_BUS 15 // Data Wire is plugged into port 15
#define PH_GPIO_PIN 18   // Power Port is GPIO PIN 18

// pH Offset
#define Offset 0.37 // Deviation compensation for pH sensor

// Sampling and sending intervals
#define samplingInterval 30000 // Interval for sampling (milliseconds)

// Global variables
float adjustedTemp = 0;    // Variable to store temperature
float adjustedTds = 0;     // Variable to store TDS
float adjustedpH = 0;      // VAriable to store pH
bool codeExecuted = false; // Flag for code execution

// Define sampling intervals for temperature and TDS sensors
#define SAMPLING_INTERVAL 30000 // Interval for temperature sampling (milliseconds)
#define READING_DURATION 12000  // Interval for TDS sampling (milliseconds)

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

// Create an instance of TemperatureSensor (onewire PIN, number of samples)
// Using the default value (10) for tempSenseIterations
TemperatureSensor tempSensorInstance(ONE_WIRE_BUS);

// Providing a custom value for tempSenseIterations (e.g., 15)
// TemperatureSensor tempSensor(ONE_WIRE_BUS, 15);

// Create an instance of TemperatureSensor (voltage, kCoefficient, reference temp, GPIO PIN, number of samples)

// Using the default value (10) for tdsSenseIterations
TdsSensor tdsSensorInstance(3.3, 0.02, 25.0, 1024.0, TDS_SENSOR_BUS);

// Providing a custom value for tdsSenseIterations (e.g., 15)
// TdsSensor tdsSensorInstance(3.3, 0.02, 25.0, 1024.0, TDS_SENSOR_BUS, 15);

// Create an instance of pHSensor (offset, GPIO pin, number of samples)
// Using the default value (40) for pHSenseIterations
pHSensor pHSensorInstance(Offset, PH_SENSOR_BUS);

// Providing a custom value for pHSenseIterations (e.g., 60)
// pHSensor phSensor(Offset, PH_SENSOR_BUS, 60);

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
    pinMode(ONE_WIRE_BUS, INPUT);      // Set GPIO PIN as INPUT for reading data
    pinMode(TDS_SENSOR_BUS, INPUT);    // Set GPIO PIN as INPUT for reading data
    Wire.begin();                      // Join I2C bus as master device (message sender)
    tempSensorInstance.beginSensors(); // Start up the temperature sensor library

    // Perform initialization if code has not been executed before
    if (!codeExecuted)
    {
        delay(5000);                       // Brief setup delay
        Serial.println("Setup Complete."); // Print setup message to serial monitor
        codeExecuted = true;               // Set code execution flag
    }
}

void loop()
{
    // Sampling temperature at regular intervals
    static unsigned long tempSamplingTime = millis();
    if (millis() - tempSamplingTime > SAMPLING_INTERVAL)
    {
        startTemperatureSensor();
        tempSamplingTime = millis();
    }

    // Sampling TDS after temperature sensor has finished
    temperatureSensorStateMachine();
    tdsSensorStateMachine();
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
        // By default the library reads every ~1s, so the duration must be times correctly according to buffer size
        if (millis() - tempStateStartTime >= READING_DURATION)
        {                                      // Wait for 10 seconds
            Serial.println(adjustedTemp);      // Uncomment for debugging
            tempSensorState = SENSOR_SHUTDOWN; // Move to next state
        }
        break;
    case SENSOR_SHUTDOWN:
        // Power off the sensor
        digitalWrite(TEMP_GPIO_PIN, LOW);
        stopTemperatureSensor(); // Reset to initial state
        startTDSSensor();
        break;
    }
}

void tdsSensorStateMachine()
{
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
        }
        break;
    case SENSOR_READ:
        // Read sensor data
        adjustedTds = tdsSensorInstance.readAndAdjustTds(adjustedTemp);
        if (millis() - tdsStateStartTime >= READING_DURATION)
        {                                     // Wait for 10 seconds
            Serial.println(adjustedTds);      // Uncomment for debugging
            tdsSensorState = SENSOR_SHUTDOWN; // Move to next state
        }
        // Serial.println(adjustedTds); // Uncomment for debugging
        break;
    case SENSOR_SHUTDOWN:
        // Power off the sensor
        digitalWrite(TDS_GPIO_PIN, LOW);
        stopTDSSensor(); // Reset to initial state
        startPHSensor();
        break;
    }
}

void pHSensorStateMachine()
{
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
        }
        break;
    case SENSOR_READ:
        // Read sensor data
        adjustedpH = pHSensorInstance.computePHValue();
        if (millis() - pHStateStartTime >= READING_DURATION)
        {                                    // Wait for 10 seconds
            Serial.println(adjustedpH);      // Uncomment for debugging
            pHSensorState = SENSOR_SHUTDOWN; // Move to next state
        }
        // Serial.println(adjustedTds); // Uncomment for debugging
        break;
    case SENSOR_SHUTDOWN:
        // Power off the sensor
        digitalWrite(PH_GPIO_PIN, LOW);
        stopPHSensor(); // Reset to initial state
        break;
    }
}