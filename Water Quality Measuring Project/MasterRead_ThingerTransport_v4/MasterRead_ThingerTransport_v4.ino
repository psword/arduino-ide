// Include necessary libraries
#define _DISABLE_TLS_
#define THINGER_SERIAL_DEBUG // Enable detailed Thinger.io debugging
#include <Wire.h>
#include <PolledTimeout.h>
#include <ESP8266WiFi.h>
#include <ThingerESP8266.h>
#include "arduino_secrets.h" // Include your secret WiFi credentials
#include "UptimeCalculator.h" // Include UptimeCalculator Library

// Define constants and variables
const int16_t I2C_SLAVE = 8; // I2C slave address
float receivedFloatValue;    // Received float value
float receivedFloatpH;       // Received pH value
float receivedFloatTemp;     // Received temperature value
float receivedFloatTds;      // Received Tds value
float latitude = 60.816138638727466; // Default latitude (testing)
float longitude = 23.623367909936814; // Default longitude (testing)
String localeName = "Forssa"; // Default location name (testing)
String locationName = "HAMK"; // Default location name (testing)
unsigned long hours;
unsigned long minutes;
unsigned long seconds;

// Thinger.io instance
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

// Function prototypes
void actuateData();
void deliverUptime();
float readFloatFromWire();

// Setup function
void setup() {
  Serial.begin(9600); // Initialize serial communication
  Wire.begin();       // Initialize I2C bus

  // Add WiFi credentials
  thing.add_wifi(ssid, ssid_pass);

  // Set LED pin as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Define Thinger Resources
  thing["1000"] >> [](pson& out){
    // Populate Thinger Resources
    out["temperature"] = receivedFloatTemp;
    out["pH"] = receivedFloatpH;
    out["Tds"] = receivedFloatTds;
    out["latitude"] = latitude;
    out["longitude"] = longitude;
    out["localeName"] = localeName;
    out["locationName"] = locationName;
    out["hoursUptime"] = uptime.getHours();
    out["minutesUptime"] = uptime.getMinutes();
    out["secondsUptime"] = uptime.getSeconds();
  };
}

// Main loop function
void loop() {
  thing.handle(); // Handle Thinger.io

  // Set up periodic timers
  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(25000); // Every 25 seconds
  static periodic uptimePing(120000); // Every 2 minutes

  // Perform actions based on timers
  if (nextPing) {
    actuateData(); // Actuate data
  }
  if (uptimePing) {
    uptime.updateUptime(); // Update uptime
  }
}

// Function to actuate data
void actuateData() {
  Wire.requestFrom(I2C_SLAVE, 8); // Request 8 bytes from slave device #8
  while (Wire.available()) { // Process available bytes
    uint8_t messageType = Wire.read(); // Read message type byte
    uint8_t messageLength = Wire.read(); // Read message length byte

    // Process message based on type and length
    if (messageType == 0x01 && messageLength == 8) {
      receivedFloatpH = readFloatFromWire(); // Read pH value
    }
    if (messageType == 0x02 && messageLength == 8) {
      receivedFloatTemp = readFloatFromWire(); // Read temperature value
    }
    if (messageType == 0x03 && messageLength == 8) {
      receivedFloatTds = readFloatFromWire(); // Read Tds value
    }
  }
}

// Function to read a float from I2C bus
float readFloatFromWire() {
  uint8_t byteArray[sizeof(float)]; // Create byte array to store float bytes
  for (int i = 0; i < sizeof(float); i++) {
    byteArray[i] = Wire.read(); // Read bytes into array
  }
  float* ptr = reinterpret_cast<float*>(byteArray); // Type punning to float pointer
  return *ptr; // Return reconstructed float value
}
