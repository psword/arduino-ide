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
float latitude = 60.816138638727466; 
float longitude = 23.623367909936814;

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup() {
  Serial.begin(9600); // Start serial communication for output
  Wire.begin();  // Join the I2C bus (address optional for master)

  // add WiFi credentials
  thing.add_wifi(ssid, ssid_pass);

  // Define Thinger Resources
  thing["1000"] >> [](pson& out){
    out["temperature"] = receivedFloatTemp;
    out["pH"] = receivedFloatpH;
    out["Tds"] = receivedFloatTds;
    out["latitude"] = latitude;
    out["longitude"] = longitude;
  };
}

void loop() {
  
  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(5000);
  static periodic thingerPing(60000);
  bool messageReceivedFlag = false;

  if (nextPing) {
    Wire.requestFrom(I2C_SLAVE, 8); // Request 8 bytes from slave device #8
    while (Wire.available()) { // Slave may send less than requested
      uint8_t messageType = Wire.read(); // Read the message type byte
      uint8_t messageLength = Wire.read(); // Read the message length byte
      
      Serial.print("Message Type: ");
      Serial.println(messageType, HEX);
      
      Serial.print("Message Length: ");
      Serial.println(messageLength, HEX);

      messageReceivedFlag = true;

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
      } else {
        Serial.println("Received Unknown value:");
        uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float
        for (int i = 0; i < messageLength; i++) {
          byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
        }
        // Reconstruct the float value from byteArray
        float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
        receivedFloatValue = *ptr; // Assign the reconstructed float value
        Serial.println(receivedFloatValue); // Print the reconstructed temperature value
      }
    }
  }

  Serial.println(messageReceivedFlag);
}
if (thingerPing) {
  // Thinger handler
  thing.handle();
}
}