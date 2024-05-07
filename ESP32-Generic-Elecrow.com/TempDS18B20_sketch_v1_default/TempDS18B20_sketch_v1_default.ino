#include <OneWire.h>             // Library for Signal on Digital Pin for OneWire Device
#include <DallasTemperature.h>   // Library for Temperature Probe

#define ONE_WIRE_BUS 2           // Data wire is plugged into port 2 on the Feather
#define GPIO_PIN 15              // Power Port is GPIO PIN 15

OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass oneWire reference to Dallas Temperature

void setup(void) {
  Serial.begin(9600);          // Start serial port for debugging
  pinMode(GPIO_PIN, OUTPUT);   // Set GPIO PIN as OUTPUT for controlling power
  sensors.begin();             // Start up the temperature sensor library
}

void loop(void) {
  digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
  delay(1500);                   // Wait for sensor stabilization
  sensors.requestTemperatures(); // Send command to get temperatures
  Serial.println(sensors.getTempCByIndex(0)); // Print temperature to serial monitor
  delay(1500);                   // Wait for sensor stabilization
  digitalWrite(GPIO_PIN, LOW);  // Power off the sensor
  delay(1500);                  // Wait before next iteration
}

