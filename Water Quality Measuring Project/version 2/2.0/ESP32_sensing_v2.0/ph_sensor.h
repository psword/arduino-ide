// ph_sensor.h

#ifndef pHSensor_h
#define pHSensor_h

#include "sensor.h"
#include <DFRobot_ESP_PH.h> // DFRobot pH Library v2.0
#include <EEPROM.h>         // EEPROM library

/**
 * pHSensor class, derived from Sensor, to handle pH sensor operations.
 */
class pHSensor : public Sensor 
{
private:
    int SENSOR_INPUT_PIN;          // Define the input PIN to read
    const int pHSenseIterations;   // Number of pH measurements to retain for buffer
    const float referenceTemp;     // Reference temperature for the pH sensor
    const float maxADCValue;       // ADC bits
    float VC;                      // The voltage on the pin powering the sensor (in 1000 units)
    float *analogBuffer;           // Dynamic array for buffer
    int analogBufferIndex;         // Index for circular buffer

    DFRobot_ESP_PH ph; // pH sensor object
    float averageVoltage;          // Store the average voltage

public:
    /**
     * Constructor to initialize the pH sensor with given parameters.
     * @param voltageConstant The voltage constant for the sensor.
     * @param refTemp The reference temperature for the sensor.
     * @param maxADC The maximum ADC value for the sensor.
     * @param inputPin The input pin for the sensor.
     * @param iterations The number of pH measurements to retain for buffer (default: 10).
     */
    pHSensor(float voltageConstant, float refTemp, float maxADC, int inputPin, int iterations = 10);

    /**
     * Destructor to clean up resources.
     */
    ~pHSensor();

    /**
     * Initializes the pH sensor object.
     */
    void beginSensors();

    /**
     * Returns the average voltage.
     * @return The average voltage.
     */
    float getAverageVoltage() const;

    /**
     * Reads analog value from pH sensor and stores in buffer.
     */
    void analogReadAction();

    /**
     * Computes the median reading from the buffer.
     * @return The median reading.
     */
    float computeMedian();

    /**
     * Adjusts pH based on temperature.
     * @param voltage The voltage to adjust.
     * @param temperature The temperature to adjust with.
     * @return The adjusted pH value.
     */
    float adjustpH(float voltage, float temperature);

    /**
     * Computes the pH value.
     * @param temperature The temperature to compute with.
     * @return The computed pH value.
     */
    float read(float temperature) override;

    /**
     * Initializes the sensor, if needed.
     */
    void init() override;

    /**
     * Shuts down the sensor, no operation.
     */
    void shutdown() override;

    /**
     * Stabilizes the sensor with a timer, possible for future operation.
     */
    void stabilize() override;
};

#endif // pHSensor_h