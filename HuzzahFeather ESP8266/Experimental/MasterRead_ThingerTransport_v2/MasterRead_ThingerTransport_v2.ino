#include <Wire.h>
#include <PolledTimeout.h>

const int16_t I2C_SLAVE = 8; // Define the I2C slave address
float receivedFloatValue;    // Variable to store received float value
float receivedFloatpH;      // Variable to store received pH value
float receivedFloatTemp;    // Variable to store received temperature value

void setup() {
  Serial.begin(9600); // Start serial communication for output
  Wire.begin();  // Join the I2C bus (address optional for master)
}

void loop() {
  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(10000);

  if (nextPing) {
    Wire.requestFrom(I2C_SLAVE, 8); // Request 8 bytes from slave device #8
    while (Wire.available()) { // Slave may send less than requested
      uint8_t messageType = Wire.read(); // Read the message type byte
      uint8_t messageLength = Wire.read(); // Read the message length byte

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
        Serial.println(receivedFloatTemp); // Print the reconstructed temperature value
      }
      
      // Delay for stability
      delay(5000);
    }
  }
}