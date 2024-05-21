#include <OneWire.h>             // Library for Signal on Digital Pin for OneWire Device
#include <DallasTemperature.h>   // Library for Temperature Probe
#include "OnewireTemperature.h"  // Temperature Sensor Class

#define ONE_WIRE_BUS 2           // Data wire is plugged into port 2 on the Feather
#define GPIO_PIN 15              // Power Port is GPIO PIN 15

OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass oneWire reference to Dallas Temperature

// Global variables
float adjustedTemp = 0;         // Variable to store temperature
bool codeExecuted = false;      // Flag for code execution

// Create an instance of TemperatureSensor (onewire PIN, number of samples)
TemperatureSensor tempSensor(ONE_WIRE_BUS);

void setup(void) {
  Serial.begin(9600);          // Start serial port for debugging
  pinMode(GPIO_PIN, OUTPUT);   // Set GPIO PIN as OUTPUT for controlling power
  // sensors.begin();             // Start up the temperature sensor library
  tempSensor.beginSensors();      // Start up the temperature sensor library

  // Perform initialization if code has not been executed before
  if (!codeExecuted) {
    delay(5000);                             // Brief setup delay
    Serial.println("Setup Complete.");       // Print setup message to serial monitor
    codeExecuted = true;                     // Set code execution flag
  }
}

void loop(void) {
  digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
  delay(1500);                   // Wait for sensor stabilization
  // sensors.requestTemperatures(); // Send command to get temperatures
  adjustedTemp = tempSensor.readAndAdjustTemp(); // Read and adjust temperature
  Serial.println(adjustedTemp);
  unsigned long stateStartTime = millis();
  if (millis() - stateStartTime >= 45000) { // Wait for 40 seconds
          Serial.println(adjustedTemp); // Print temperature to serial monitor
          delay(1500);                   // Wait for sensor stabilization
          digitalWrite(GPIO_PIN, LOW);  // Power off the sensor
          delay(5000);
        }
}

