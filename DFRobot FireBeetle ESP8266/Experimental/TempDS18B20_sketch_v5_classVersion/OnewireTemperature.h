class TemperatureSensor {
private:
    const int tempSenseIterations; // number of measurements to take
    OneWire oneWire;
    DallasTemperature sensors;
    std::vector<float> analogBuffer; // Use std::vector instead of raw array
    int analogBufferIndex;

public:
    // Constructor
    TemperatureSensor(int oneWirePin, int iterations) : oneWire(oneWirePin), tempSenseIterations(iterations),sensors(&oneWire), analogBufferIndex(0) {}

    // Function to initialize the sensors object
    void beginSensors() {
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
        std::vector<float> sortedBuffer = analogBuffer;
        std::sort(sortedBuffer.begin(), sortedBuffer.end()); // Use begin() and end() iterators

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