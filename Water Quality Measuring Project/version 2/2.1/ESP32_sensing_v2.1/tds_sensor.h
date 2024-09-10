// tds_sensor.h

#ifndef TDSSENSOR_H
#define TDSSENSOR_H

#include "sensor.h"

/**
 * TdsSensor class, derived from Sensor, to read TDS (Total Dissolved Solids) data.
 */
class TdsSensor : public Sensor
{
private:
    const float kCoefficient;     // 2% Coefficient calculation
    const float referenceTemp;    // Reference temperature for the TDS sensor
    const int tdsSenseIterations; // Number of measurements to retain for buffer
    const float maxADCValue;      // ADC bits
    const int SENSOR_INPUT_PIN;   // Define the input PIN

    float VC;                      // The voltage on the pin powering the sensor
    float *analogBuffer;           // Dynamic array for buffer
    int analogBufferIndex;         // Index for the circular buffer

public:
    /**
     * Constructor for TdsSensor.
     * @param voltageConstant The voltage constant for the sensor.
     * @param kCoeff The temperature coefficient.
     * @param refTemp The reference temperature for TDS measurement.
     * @param maxADC The maximum ADC value.
     * @param inputPin The pin for sensor input.
     * @param iterations The number of measurements to retain for buffer (default is 10).
     */
    TdsSensor(float voltageConstant, float kCoeff, float refTemp, float maxADC, int inputPin, int iterations = 10);

    /**
     * Destructor for TdsSensor.
     */
    ~TdsSensor();

    /**
     * Reads analog value from sensor and stores in buffer.
     */
    void analogReadAction();

    /**
     * Computes the median reading from the buffer.
     * @return The median value.
     */
    float computeMedian();

    /**
     * Adjusts the TDS reading based on temperature.
     * @param voltage The voltage reading from the sensor.
     * @param temperature The current temperature.
     * @return The adjusted TDS value.
     */
    float adjustTds(float voltage, float temperature);

    /**
     * Reads and adjusts the TDS value.
     * @param temperature The current temperature.
     * @return The adjusted TDS value.
     */
    float read(float temperature) override;

    /**
     * Initializes the sensor.
     */
    void init() override;

    /**
     * Shuts down the sensor.
     */
    void shutdown() override;

    /**
     * Stabilizes the sensor.
     */
    void stabilize() override;
};

#endif // TDSSENSOR_H
