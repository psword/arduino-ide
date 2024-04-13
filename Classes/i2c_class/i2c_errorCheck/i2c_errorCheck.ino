#include "i2c_communicator.h"
// Instantiate I2CCommunicator
I2CCommunicator i2cCommunicator(3, 10, 100); // Maximum 3 retries, delay between 10 and 100 milliseconds

// Example data
uint8_t data[] = {0x01, 0x02, 0x03};

// Transmit data
bool success = i2cCommunicator.transmitData(slaveAddress, data, sizeof(data));
if (success) {
    Serial.println("Data transmitted successfully.");
} else {
    Serial.println("Error: Maximum retries reached. Unable to transmit data.");
}
