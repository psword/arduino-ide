#include <OneWire.h>
#include <DallasTemperature.h>
#include <algorithm> // For std::sort

class TemperatureSensor {
private:
    OneWire oneWire;
    DallasTemperature sensors;
    float analogBuffer[tempSenseIterations];
    int analogBufferIndex;

public:
    // Constructor
    TemperatureSensor(int oneWirePin) : oneWire(oneWirePin), sensors(&oneWire), analogBufferIndex(0) {
        sensors.begin();
    }

    // Function to read temperature and update buffer
    void analogReadAction() {
        sensors.requestTemperatures();
        float sensorValue = sensors.getTempCByIndex(0);
        analogBuffer[analogBufferIndex] = sensorValue;
        analogBufferIndex = (analogBufferIndex + 1) % tempSenseIterations;
    }

    // Function to compute median temperature from buffer
    float computeMedian() {
        float sortedBuffer[tempSenseIterations];
        // memcpy(sortedBuffer, analogBuffer, tempSenseIterations * sizeof(float)); // you can use this method (no type safety) or std::copy (type safety)
        std::copy(analogBuffer, analogBuffer + tempSenseIterations, sortedBuffer); // comment this one out if you use the other
        std::sort(sortedBuffer, sortedBuffer + tempSenseIterations);

        if (tempSenseIterations % 2 == 0) {
            float median = (sortedBuffer[tempSenseIterations / 2 - 1] + sortedBuffer[tempSenseIterations / 2]) / 2.0f;
            return median;
        } else {
            return sortedBuffer[tempSenseIterations / 2];
        }
    }

    // Function to read and adjust temperature
    float readAndAdjustTemp() {
        analogReadAction();
        return computeMedian();
    }
};
