#include <Wire.h>
#include <PolledTimeout.h>


const int16_t I2C_SLAVE = 8;
float receivedFloatValue;
float receivedFloatpH;

void setup() {
  Serial.begin(9600);                      // start serial for output
  Wire.begin();  // join i2c bus (address optional for master)
}

void loop() {
  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(1000);

  if (nextPing) {
    Wire.requestFrom(I2C_SLAVE, 7);  // request 7 bytes from slave device #8
    while (Wire.available()) {  // slave may send less than requested
      //
      uint8_t messageType = Wire.read(); // Read the message type byte
      uint8_t messageLength = Wire.read(); // Read the message length byte
      if (messageType == 0x01 && messageLength == 8) { // Check if messageType is correct and messageLength is 8
      uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float
      for (int i = 0; i < messageLength; i++) {
        byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
        // Print each byte as a hexadecimal value for debugging
        // Serial.print(byteArray[i], HEX);
        // Serial.print(" "); // Print a space between bytes for better readability 
      }
      // Reconstruct the float value from byteArray
      float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
      receivedFloatValue = *ptr; // Assign the reconstructed float value to receivedFloatValue 
      receivedFloatpH = receivedFloatValue;
      // memcpy(&receivedFloatValue, byteArray, sizeof(float)); // Another debugging method to reconstruct the float value from the byte array
      Serial.println(receivedFloatpH); // Print the reconstructed float value for debugging, this is what Thinglink should see
      }
    Serial.println();
    delay(5000);
  }
  }
}
