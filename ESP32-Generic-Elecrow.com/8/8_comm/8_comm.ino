#include <Arduino.h>
#include <Wire.h>

// Definitions
#define DEBUGPRINT_INTERVAL 30000 // Interval to request data from I2C slave

// Define variables
uint8_t messageType1 = 0x01; // Message type identifier
uint8_t messageType2 = 0x02; // Message type identifier
uint8_t messageType3 = 0x03; // Message type identifier
uint8_t messageLength = 8;   // Length of message to transmit
uint8_t slaveAddress = 0x08; // I2C Slave address
float receivedFloatTemp;     // Float value to receive
float receivedFloatTds;      // Float value to receive
float receivedFloatpH;       // Float value to receive

// Function prototypes
void receiveData(int bytes);
float readFloatFromWire();

void setup()
{
  Serial.begin(9600); // Start the serial port for debugging
  // Wait for the serial port to connect
  while (!Serial)
  {
    ;
  }
  // Register functions to handle I2C data reception and requests
  Wire.onReceive(receiveData);
  Wire.begin(slaveAddress);

}

void loop()
{
  // Sampling temperature at regular intervals begins the process
  static unsigned long dataRequestTime = millis();
  if (millis() - dataRequestTime > DEBUGPRINT_INTERVAL)
  {
    Serial.println(receivedFloatTemp); // Debugging print
    delay(5000);
    Serial.println(receivedFloatTds);  // Debugging print
    delay(5000);
    Serial.println(receivedFloatpH);   // Debugging print
    delay(5000);
    dataRequestTime = millis();
  }
}

void receiveData(int bytes)
{
  // Serial.print("Data received. Bytes: ");
  // Serial.println(bytes);

  while (Wire.available())
  {
    uint8_t messageType = Wire.read();   // Read message type byte
    uint8_t messageLength = Wire.read(); // Read message length byte

    // Debugging print
    // Serial.print("Message type: ");
    // Serial.print(messageType);
    // Serial.print(", Message length: ");
    // Serial.println(messageLength);

    // Let's process the message based on type and length
    if (messageLength == 8)
    {
      float receivedValue = readFloatFromWire(); // Read float value
      switch (messageType)
      {
      case 0x01:
        receivedFloatTemp = receivedValue; // Read temperature value
        // Serial.print("Temperature received: ");
        // Serial.println(receivedFloatTemp);
        break;
      case 0x02:
        receivedFloatTds = receivedValue; // Read Tds value
        // Serial.print("TDS received: ");
        // Serial.println(receivedFloatTds);
        break;
      case 0x03:
        receivedFloatpH = receivedValue; // Read pH value
        // Serial.print("pH received: ");
        // Serial.println(receivedFloatpH);
        break;
      default:
        // Handle unknown message type if necessary
        // Add handling statements
        // Serial.println("Unknown message type");
        break;
      }
    }
  }
}


// Function to read a float from I2C bus
float readFloatFromWire()
{
  uint8_t byteArray[sizeof(float)]; // Create byte array to store float bytes
  for (int i = 0; i < sizeof(float); i++)
  {
    byteArray[i] = Wire.read(); // Read bytes into array
  }
  float value;
  memcpy(&value, byteArray, sizeof(float)); // Copy bytes to float
  return value;                             // Return reconstructed float value
}
