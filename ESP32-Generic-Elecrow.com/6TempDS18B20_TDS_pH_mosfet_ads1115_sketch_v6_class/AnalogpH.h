#include <numeric>            // Include the <numeric> header for std::accumulate
#include <Adafruit_ADS1X15.h> // Include the library for an ADS1115 ADC breakout

// Using this code with ADS1115
Adafruit_ADS1115 ads; /* Use this for the 16-bit version */

class pHSensor
{
private:
    // Constants
    const float offset;            // Define the pH offset
    int SENSOR_INPUT_PIN;          // Define the input PIN to read
    const int pHSenseIterations;   // Number of pH measurements to take
    float *analogBuffer;           // Dynamic array for buffer
    int analogBufferIndex;         // Index for circular buffer
    unsigned long lastReadTime;    // To store the last read time
    const unsigned long readDelay; // Delay between reads (milliseconds)

public:
    // Constructor
    pHSensor(float pHManualOffset, int adcPin, int iterations = 40, unsigned long readGap = 250)
        : offset(pHManualOffset), SENSOR_INPUT_PIN(adcPin), pHSenseIterations(iterations), readDelay(readGap), lastReadTime(0), analogBufferIndex(0)
    {
        // Allocate memory for the analog buffer
        analogBuffer = new float[pHSenseIterations];
    }

    // Destructor
    ~pHSensor()
    {
        // Deallocate memory for the analog buffer
        delete[] analogBuffer;
    }

    // Function to read analog value from pH sensor and store in buffer
    void analogReadAction()
    {
        unsigned long currentMillis = millis(); // Assign current millis value for timer

        // Check if the delay gap has passed
        if (currentMillis - lastReadTime >= readDelay)
        {
            int16_t adcValue;
            float voltsValue;

            // Read the appropriate ADC channel based on SENSOR_INPUT_PIN
            Serial.println("Reading the ADC");
            /* Crash report received here related to ADS1115
            Applied Mosfet Power.
Reading the ADC
Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.

Core  1 register dump:
PC      : 0x400d36d3  PS      : 0x00060c30  A0      : 0x800d35dc  A1      : 0x3ffb21c0
A2      : 0x00000000  A3      : 0x3ffc1d5e  A4      : 0x00000003  A5      : 0x00000001
A6      : 0x00000000  A7      : 0x00000000  A8      : 0x800d3a4c  A9      : 0x3ffb21a0
A10     : 0x3ffbdbcc  A11     : 0x3f4054cf  A12     : 0x00000002  A13     : 0xffffffff

Unresolved, see readme.md
*/

            adcValue = ads.readADC_SingleEnded(SENSOR_INPUT_PIN);
            Serial.println(adcValue);
            voltsValue = ads.computeVolts(adcValue);
            Serial.println(voltsValue);

            // Store the voltage value in the circular buffer
            analogBuffer[analogBufferIndex] = voltsValue;

            // Update the buffer index
            analogBufferIndex = (analogBufferIndex + 1) % pHSenseIterations;

            // Update the last read time
            lastReadTime = currentMillis;
        }
    }

    // Function to compute pH value
    float computePHValue()
    {
        // Perform pH sensing operations
        analogReadAction();
        float voltage = averageArray(analogBuffer, pHSenseIterations);
        return 3.5 * voltage + offset;
    }

    // Function to average an array
    double averageArray(float *arr, int number)
    {
        if (arr == nullptr || number <= 0)
        {
            // Handle error condition
            Serial.println("Invalid array or number of elements.");
            return 0;
        }
        // Sum up the elements of the array using std::accumulate
        double sum = std::accumulate(arr, arr + number, 0.0);
        // Calculate the average
        return sum / number;
    }
};
