class TemperatureSensor {
private:
    const int tempSenseIterations; // number of measurements to take
    OneWire oneWire;
    DallasTemperature sensors;
    float* analogBuffer; // Dynamic array for buffer
    int analogBufferIndex; // Index for the circular buffer

public:
   // Constructor
    TemperatureSensor(int oneWirePin, int iterations = 10) : oneWire(oneWirePin), sensors(&oneWire), tempSenseIterations(iterations), analogBufferIndex(0) {
        // Allocate memory for the analog buffer
        analogBuffer = new float[tempSenseIterations];
    }

    // Destructor
    ~TemperatureSensor() {
        // Deallocate memory for the analog buffer
        delete[] analogBuffer;
    }

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
        float sortedBuffer[tempSenseIterations]; // Temporary array for sorting
        std::copy(analogBuffer, analogBuffer + tempSenseIterations, sortedBuffer);
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
