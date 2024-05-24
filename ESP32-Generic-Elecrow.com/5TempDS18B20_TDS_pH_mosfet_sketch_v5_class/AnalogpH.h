#include <algorithm>        // Include for std::copy and std::sort
#include <DFRobot_ESP_PH.h> // DFRobot pH Library v2.0
#include <EEPROM.h>         // EEPROM library

class pHSensor
{
private:
    // Constants
    int SENSOR_INPUT_PIN;          // Define the input PIN to read
    const int pHSenseIterations;   // Number of pH measurements to take
    const float referenceTemp;     // Reference temperature for the pH sensor
    const float maxADCValue;       // ADC bits
    float VC;                      // The voltage on the pin powering the sensor (in 1000 units)
    float *analogBuffer;           // Dynamic array for buffer
    int analogBufferIndex;         // Index for circular buffer
    unsigned long lastReadTime;    // To store the last read time
    const unsigned long readDelay; // Delay between reads (milliseconds)

    DFRobot_ESP_PH ph; // pH sensor object

public:
    // Constructor
    pHSensor(float voltageConstant, float refTemp, float maxADC, int inputPin, int iterations = 40, unsigned long readGap = 250)
        : VC(voltageConstant), referenceTemp(refTemp), maxADCValue(maxADC), SENSOR_INPUT_PIN(inputPin), pHSenseIterations(iterations), readDelay(readGap), lastReadTime(0), analogBufferIndex(0)
    {
        // Allocate memory for the analog buffer
        analogBuffer = new float[pHSenseIterations];
        if (analogBuffer == nullptr)
        {
            Serial.println("Failed to allocate memory for analog buffer");
        }
    }

    // Destructor
    ~pHSensor()
    {
        // Deallocate memory for the analog buffer
        delete[] analogBuffer;
    }

    // Function to initialize the sensors object
    void beginSensors()
    {
        ph.begin();
    }

    // Function to get the readDelay value
    unsigned long getReadDelay() const
    {
        return readDelay;
    }

    // Function to read analog value from pH sensor and store in buffer
    void analogReadAction()
    {
        // Read the appropriate ADC channel based on SENSOR_INPUT_PIN
        float sensorValue = analogRead(SENSOR_INPUT_PIN);
        Serial.println(sensorValue);
        // Store the voltage value in the circular buffer
        analogBuffer[analogBufferIndex] = sensorValue;

        // Update the buffer index
        analogBufferIndex = (analogBufferIndex + 1) % pHSenseIterations;
    }

    // Function to compute median reading from buffer
    float computeMedian()
    {
        float sortedBuffer[pHSenseIterations]; // Temporary array for sorting
        std::copy(analogBuffer, analogBuffer + pHSenseIterations, sortedBuffer);
        std::sort(sortedBuffer, sortedBuffer + pHSenseIterations);

        if (pHSenseIterations % 2 == 0)
        {
            float median = (sortedBuffer[pHSenseIterations / 2 - 1] + sortedBuffer[pHSenseIterations / 2]) / 2.0f;
            return median;
        }
        else
        {
            return sortedBuffer[pHSenseIterations / 2];
        }
    }

    // Function to adjust pH based on temperature
    float adjustpH(float voltage, float temperature)
    {
        float pHValue = ph.readPH(voltage, temperature); // Convert voltage to pH with temp adjustment
        return pHValue;
    }

    // Function to compute pH value
    float computePHValue(float temperature)
    {
        analogReadAction();
        float medianSensorValue = computeMedian();
        float averageVoltage = medianSensorValue * VC / maxADCValue;
        return adjustpH(averageVoltage, temperature);
    }
};
