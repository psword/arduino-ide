#include <Wire.h>

class I2CCommunicator {
private:
    int maxRetries;
    int delayMin;
    int delayMax;

public:
    I2CCommunicator(int maxRetries, int delayMin, int delayMax)
        : maxRetries(maxRetries), delayMin(delayMin), delayMax(delayMax) {}

    bool transmitData(int slaveAddress, uint8_t* data, size_t dataSize) {
        int retryCount = 0;

        while (retryCount < maxRetries) {
            Wire.beginTransmission(slaveAddress);
            for (size_t i = 0; i < dataSize; i++) {
                Wire.write(data[i]);
            }

            int transmissionResult = Wire.endTransmission();
            if (transmissionResult == 0) {
                return true; // Success
            } else {
                retryCount++;
                delay(random(delayMin, delayMax)); // Back-off delay
            }
        }

        return false; // Maximum retries reached
    }
};
