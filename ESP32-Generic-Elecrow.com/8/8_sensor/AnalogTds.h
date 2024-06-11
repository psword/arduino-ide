#include <algorithm> // Include for std::copy and std::sort

class TdsSensor
{
private:
    // Constants for temperature coefficient calculation
    const float kCoefficient;     // 2% Coefficient calculation
    const float referenceTemp;    // Reference temperature for the TDS sensor
    const int tdsSenseIterations; // Number of measurements to take
    const float maxADCValue;      // ADC bits
    const int SENSOR_INPUT_PIN;   // Define the input PIN

    float VC;                      // The voltage on the pin powering the sensor
    float *analogBuffer;           // Dynamic array for buffer
    int analogBufferIndex;         // Index for the circular buffer
    unsigned long lastReadTime;    // To store the last read time
    const unsigned long readDelay; // Delay between reads (milliseconds)

public:
    // Constructor
    TdsSensor(float voltageConstant, float kCoeff, float refTemp, float maxADC, int inputPin, int iterations = 10, unsigned long readGap = 250)
        : VC(voltageConstant), kCoefficient(kCoeff), referenceTemp(refTemp), maxADCValue(maxADC), SENSOR_INPUT_PIN(inputPin), tdsSenseIterations(iterations), readDelay(readGap), lastReadTime(0), analogBufferIndex(0)
    {
        // Allocate memory for the analog buffer
        analogBuffer = new float[tdsSenseIterations];
        if (analogBuffer == nullptr)
        {
            Serial.println("Failed to allocate memory for analog buffer");
        }
    }

    // Destructor
    ~TdsSensor()
    {
        // Deallocate memory for the analog buffer
        delete[] analogBuffer;
    }

    // Method to get the readDelay value
    unsigned long getReadDelay() const
    {
        return readDelay;
    }

    // Function to read analog value from sensor and store in buffer
    void analogReadAction()
    {
        unsigned long currentMillis = millis(); // Assign current millis value for timer

        // Check if the delay gap has passed
        if (currentMillis - lastReadTime >= readDelay)
        {
            float sensorValue = analogRead(SENSOR_INPUT_PIN);
            // Serial.println(sensorValue); // Uncomment for debugging
            analogBuffer[analogBufferIndex] = sensorValue;
            analogBufferIndex = (analogBufferIndex + 1) % tdsSenseIterations; // Circular buffer

            // Update the last read time
            lastReadTime = currentMillis;
        }
    }

    // Function to compute median reading from buffer
    float computeMedian()
    {
        float sortedBuffer[tdsSenseIterations]; // Temporary array for sorting
        std::copy(analogBuffer, analogBuffer + tdsSenseIterations, sortedBuffer);
        std::sort(sortedBuffer, sortedBuffer + tdsSenseIterations);

        if (tdsSenseIterations % 2 == 0)
        {
            float median = (sortedBuffer[tdsSenseIterations / 2 - 1] + sortedBuffer[tdsSenseIterations / 2]) / 2.0f;
            return median;
        }
        else
        {
            return sortedBuffer[tdsSenseIterations / 2];
        }
    }

    // Function to adjust TDS reading based on temperature
    float adjustTds(float voltage, float temperature)
    {
        // Calculate temperature correction factor
        float tempCorrection = 1.0 + kCoefficient * (temperature - referenceTemp);

        // Calculate compensation voltage
        float compensationVoltage = voltage / tempCorrection;

        // Adjust TDS reading based on temperature
        return (133.42 / compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
    }

    // Combined function for reading analog value and adjusting TDS
    float readAndAdjustTds(float temperature)
    {
        analogReadAction();
        float medianSensorValue = computeMedian();
        float averageVoltage = medianSensorValue * VC / maxADCValue;
        return adjustTds(averageVoltage, temperature);
    }
};
