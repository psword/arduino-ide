// #include <Arduino.h>

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "OnewireTemperature.h"  // Temperature Sensor Class

// Pin definitions
#define ONE_WIRE_BUS 2           // Data wire is plugged into port 2 on the Feather
#define GPIO_PIN 15              // Power Port is GPIO PIN 15

// Sampling and sending intervals
#define samplingInterval 20000   // Interval for sampling (milliseconds)

// Global variables
float adjustedTemp = 0;         // Variable to store temperature
bool codeExecuted = false;      // Flag for code execution

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
void startSensor();
void stopSensor();
void transmitSlave();

// Create an instance of TemperatureSensor (onewire PIN, number of samples)
TemperatureSensor tempSensor(ONE_WIRE_BUS);

void setup() {
  Serial.begin(9600);                        // Start serial port for debugging
  // Wait for serial port to connect
  while (!Serial) {
    ;
  }
  pinMode(GPIO_PIN, OUTPUT);                 // Set GPIO PIN as OUTPUT for controlling power
  Wire.begin();                              // Join I2C bus as master device (message sender)
  tempSensor.beginSensors();                // Start up the temperature sensor library

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
        adjustedTemp = tempSensor.readAndAdjustTemp(); // Read and adjust temperature
        if (millis() - stateStartTime >= 15000) { // Wait for 10 seconds
          Serial.println(adjustedTemp); // Uncomment for debugging
          currentState = SENSOR_SHUTDOWN; // Move to next state
        }
        // Serial.println(adjustedTemp); // Uncomment for debugging
        break; 
    case SENSOR_SHUTDOWN:
        // Power off the sensor
        digitalWrite(GPIO_PIN, LOW);
        stopSensor(); // Reset to initial state
        break;
  }
  
  // Sampling temperature at regular intervals
  static unsigned long samplingTime = millis(); // Set the sampling time
  if (millis() - samplingTime > samplingInterval) {
    startSensor();
    samplingTime = millis();                  // Reset the sampling time
  }

}

void startSensor() {
  // Serial.println("init"); // Uncomment for debugging
  currentState = SENSOR_INIT;
}

void stopSensor() {
  currentState = SENSOR_OFF;
}