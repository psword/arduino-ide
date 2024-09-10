// tds_sensor.cpp

#include "tds_sensor.h"
#include <Arduino.h>
#include <algorithm> // Include for std::copy and std::sort

/**
 * Constructor for TdsSensor.
 */
TdsSensor::TdsSensor(float voltageConstant, float kCoeff, float refTemp, float maxADC, int inputPin, int iterations)
    : VC(voltageConstant), kCoefficient(kCoeff), referenceTemp(refTemp), maxADCValue(maxADC), SENSOR_INPUT_PIN(inputPin), tdsSenseIterations(iterations), analogBufferIndex(0)
{
    // Allocate memory for the analog buffer
    analogBuffer = new float[tdsSenseIterations];
    if (analogBuffer == nullptr)
    {
        Serial.println("Failed to allocate memory for analog buffer");
    }
}

/**
 * Destructor for TdsSensor.
 */
TdsSensor::~TdsSensor()
{
    // Deallocate memory for the analog buffer
    delete[] analogBuffer;
}

/**
 * Reads analog value from sensor and stores in buffer.
 */
void TdsSensor::analogReadAction()
{
    float sensorValue = analogRead(SENSOR_INPUT_PIN);
    analogBuffer[analogBufferIndex] = sensorValue;
    analogBufferIndex = (analogBufferIndex + 1) % tdsSenseIterations; // Circular buffer
    }

/**
 * Computes the median reading from the buffer.
 */
float TdsSensor::computeMedian()
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

/**
 * Adjusts the TDS reading based on temperature.
 */
float TdsSensor::adjustTds(float voltage, float temperature)
{
    // Calculate temperature correction factor
    float tempCorrection = 1.0 + kCoefficient * (temperature - referenceTemp);

    // Calculate compensation voltage
    float compensationVoltage = voltage / tempCorrection;

    // Adjust TDS reading based on temperature
    return (133.42 / compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
}

/**
 * Reads and adjusts the TDS value.
 */
float TdsSensor::read(float temperature)
{
    analogReadAction();
    float medianSensorValue = computeMedian();
    float voltage = (medianSensorValue / maxADCValue) * VC;
    return adjustTds(voltage, temperature);
}

/**
 * Initializes the sensor.
 */
void TdsSensor::init()
{
    // No specific initialization needed
}

/**
 * Shuts down the sensor.
 */
void TdsSensor::shutdown()
{
    // No specific shutdown needed
}

/**
 * Stabilizes the sensor.
 */
void TdsSensor::stabilize()
{
    // Possible future implementation for stabilization
}
