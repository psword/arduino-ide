#include <numeric> // Include the <numeric> header for std::accumulate

class pHSensor {
private:
    // Constants
    const float offset; // Define the pH offset
    const int SENSOR_INPUT_PIN; // Define the input PIN
    const int pHSenseIterations; // Number of pH measurements to take

    float* analogBuffer; // Dynamic array for buffer
    int analogBufferIndex; // Index for circular buffer
    // pH sampling function
    
public:
    // Constructor
    pHSensor(float pHManualOffset, int inputPin, int iterations = 40) 
    : offset(pHManualOffset), SENSOR_INPUT_PIN(inputPin), pHSenseIterations(iterations), analogBufferIndex(0) {
        // Allocate memory for the analog buffer
        analogBuffer = new float[pHSenseIterations];
    }
    
    // Destructor
    ~pHSensor() {
        // Deallocate memory for the analog buffer
        delete[] analogBuffer;
    }

    // Function to read analog value from pH sensor and store in buffer
    void analogReadAction() {
        float sensorValue = analogRead(SENSOR_INPUT_PIN);
        analogBuffer[analogBufferIndex] = sensorValue;
        analogBufferIndex = (analogBufferIndex + 1) % pHSenseIterations; // Circular buffer       
    }

    // Function to compute pH value
    float computePHValue() {
        // Perform pH sensing operations
        analogReadAction();
        float voltage = averageArray(analogBuffer, pHSenseIterations) * 5.0 / 1024;
        return 3.5 * voltage + offset; // Assuming offset is defined somewhere
    }

    // Function to average an array
    double averageArray(float* arr, int number) {
        if (number <= 0) {
            // Handle error condition
            return 0;
        }
        // Sum up the elements of the array using std::accumulate
        double sum = std::accumulate(arr, arr + number, 0.0);
        // Calculate the average
        return sum / number;
    }
};