#define _DISABLE_TLS_
#define THINGER_SERIAL_DEBUG // more details at http://docs.thinger.io/arduino/

#include <Wire.h>
#include <PolledTimeout.h>
#include <ESP8266WiFi.h>
#include <ThingerESP8266.h>
#include "arduino_secrets.h"

const int16_t I2C_SLAVE = 8; // Define the I2C slave address
float receivedFloatValue;    // Variable to store received float value
float receivedFloatpH;      // Variable to store received pH value
float receivedFloatTemp;    // Variable to store received temperature value
float receivedFloatTds;     // Variable to store received Tds value
float latitude = 60.816138638727466; // latitude for default location (testing)
float longitude = 23.623367909936814; // longtidue for default location (testing)
String localeName = "Forssa"; // name for default location (testing)
String locationName = "HAMK"; // name for default location (testing)
unsigned long hours;
unsigned long minutes;
unsigned long seconds;
// bool requestData = false;

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup() {
  Serial.begin(9600); // Start serial communication for output
  Wire.begin();  // Join the I2C bus (address optional for master)

  // add WiFi credentials
  thing.add_wifi(ssid, ssid_pass);

  // initialize digital pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);

  // Define Thinger Resources
  thing["1000"] >> [](pson& out){
    out["temperature"] = receivedFloatTemp;
    out["pH"] = receivedFloatpH;
    out["Tds"] = receivedFloatTds;
    out["latitude"] = latitude;
    out["longitude"] = longitude;
    out["localeName"] = localeName;
    out["locationName"] = locationName;
    out["hoursUptime"] = hours;
    out["minutesUptime"] = minutes;
    out["secondsUptime"] = seconds;
  };

  // Define Thinger Properties
  // thing["1000"] << [](pson& in){
  //  requestData = in;
  // };
}

void loop() {
  
  thing.handle();

  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(60000);
  // static periodic thingerPing(90000);
  static periodic uptimePing(120000);

  if (nextPing) {
    actuateData();
  }
  // if (requestData == true) {
  //   actuateData();
  //   requestData = false;
  // }
  if (uptimePing) {
    deliverUptime();
  }
}

void actuateData() {
  Wire.requestFrom(I2C_SLAVE, 8); // Request 8 bytes from slave device #8
    while (Wire.available()) { // Slave may send less than requested
      uint8_t messageType = Wire.read(); // Read the message type byte
      uint8_t messageLength = Wire.read(); // Read the message length byte
      
      Serial.print("Message Type: ");
      Serial.println(messageType, HEX);
      
      Serial.print("Message Length: ");
      Serial.println(messageLength, HEX);

      // This block is for message type 0x01 (pH value)
      if (messageType == 0x01 && messageLength == 8) {
        Serial.println("Received pH value:");
        uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float
        for (int i = 0; i < messageLength; i++) {
          byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
        }
        // Reconstruct the float value from byteArray
        float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
        receivedFloatValue = *ptr; // Assign the reconstructed float value
        receivedFloatpH = receivedFloatValue;
        Serial.println(messageType, HEX);
        Serial.println(messageLength);
        Serial.println(receivedFloatpH); // Print the reconstructed pH value
      }

      // This block is for message type 0x02 (temperature value)
      if (messageType == 0x02 && messageLength == 8) {
        Serial.println("Received temperature value:");
        uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float
        for (int i = 0; i < messageLength; i++) {
          byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
        }
        // Reconstruct the float value from byteArray
        float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
        receivedFloatValue = *ptr; // Assign the reconstructed float value
        receivedFloatTemp = receivedFloatValue;
        Serial.println(messageType, HEX);
        Serial.println(messageLength);
        Serial.println(receivedFloatTemp); // Print the reconstructed temperature value
      }

      // This block is for message type 0x03 (tds value)
      if (messageType == 0x03 && messageLength == 8) {
        Serial.println("Received Tds value:");
        uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float
        for (int i = 0; i < messageLength; i++) {
          byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
        }
        // Reconstruct the float value from byteArray
        float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
        receivedFloatValue = *ptr; // Assign the reconstructed float value
        receivedFloatTds = receivedFloatValue;
        Serial.println(messageType, HEX);
        Serial.println(messageLength);
        Serial.println(receivedFloatTds); // Print the reconstructed temperature value
      } 
    }
}

void deliverUptime() {
  unsigned long uptimeMillis = millis(); // Get the current uptime in milliseconds
  
  // Convert milliseconds to hours, minutes, and seconds
  hours = uptimeMillis / (1000UL * 60UL * 60UL);
  uptimeMillis %= (1000UL * 60UL * 60UL);
  minutes = uptimeMillis / (1000UL * 60UL);
  uptimeMillis %= (1000UL * 60UL);
  seconds = uptimeMillis / 1000UL;
}