#include <Wire.h>
#include <algorithm>
#include <vector> // Include the vector header

class TdsSensor {
private:
    // Constants for temperature coefficient calculation
    const float kCoefficient; // 2% Coefficient calculation
    const float referenceTemp; // reference temperature for the TDS sensor

    const int tdsSenseIterations; // number of measurements to take
    const float maxADCValue; // ADC bits
    const int SENSOR_INPUT_PIN; // Define the input PIN
    
    float VC; // the voltage on the pin powering the sensor
    std::vector<float> analogBuffer; // Use std::vector instead of raw array
    int analogBufferIndex; // Index for the circular buffer
    
public:
    // Constructor
    TdsSensor(float voltageConstant, float kCoeff, float refTemp, int iterations, float maxADC, int inputPin) 
    : VC(voltageConstant), kCoefficient(kCoeff), referenceTemp(refTemp), tdsSenseIterations(iterations), maxADCValue(maxADC), SENSOR_INPUT_PIN(inputPin), analogBuffer(iterations), analogBufferIndex(0) {
    // Initialize any additional necessary variables or perform setup here
    }

    // Function to read analog value from sensor and store in buffer
    void analogReadAction() {
        float sensorValue = analogRead(SENSOR_INPUT_PIN);
        Serial.println(sensorValue); // Uncomment for debugging
        analogBuffer[analogBufferIndex] = sensorValue;
        analogBufferIndex = (analogBufferIndex + 1) % tdsSenseIterations; // Circular buffer       
    }

    // Function to compute median reading from buffer
    float computeMedian() {
        std::vector<float> sortedBuffer = analogBuffer;
        std::sort(sortedBuffer.begin(), sortedBuffer.end());

        if (tdsSenseIterations % 2 == 0) {
            float median = (sortedBuffer[tdsSenseIterations / 2 - 1] + sortedBuffer[tdsSenseIterations / 2]) / 2.0f;
            return median;
        } else {
            return sortedBuffer[tdsSenseIterations / 2];
        }
    }

    // Function to adjust TDS reading based on temperature
    float adjustTds(float voltage, float temperature) {
        // Calculate temperature correction factor
        float tempCorrection = 1.0 + kCoefficient * (temperature - referenceTemp);
        
        // Calculate compensation voltage
        float compensationVoltage = voltage / tempCorrection;

        // Adjust TDS reading based on temperature
        return (133.42 / compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
    }

    // Combined function for reading analog value and adjusting TDS
    float readAndAdjustTds(float temperature) {
        analogReadAction();
        float medianSensorValue = computeMedian();
        float averageVoltage = medianSensorValue * VC / maxADCValue;
        return adjustTds(averageVoltage, temperature);
    }
};
