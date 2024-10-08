/*
 * File: sensor_config.h
 * Description: Header file containing pin definitions, global variables, and configuration constants
 *              for the sensor monitoring system using ESP32.
 * Author: Philip
 */

#ifndef SENSOR_CONFIG
#define SENSOR_CONFIG

// Pin Definitions

#define ONE_WIRE_BUS 2             // GPIO pin for the OneWire data line (DS18B20 temperature sensor)
#define TEMP_SENSOR_POWER_PIN 17   // GPIO pin for controlling power to the temperature sensor
#define TDS_SENSOR_BUS 4           // GPIO pin for the TDS sensor data line
#define TDS_SENSOR_POWER_PIN 16    // GPIO pin for controlling power to the TDS sensor
#define PH_SENSOR_POWER_PIN 15     // GPIO pin for controlling power to the pH sensor
#define PH_SENSOR_BUS 13           // GPIO pin for the pH sensor data line

// Global Variables

extern float adjustedTemp;               // Global variable to store the adjusted temperature value
extern bool codeExecuted;                // Flag to indicate whether the code has executed before
extern const unsigned long readDelay;    // Global constant for the delay between sensor reads (milliseconds)

// Time and Sleep Configuration

#define READING_DURATION 15000           // Duration in milliseconds for sensor sampling
#define uS_TO_S_FACTOR 1000000           // Conversion factor from microseconds to seconds
#define S_TO_MIN_FACTOR 60               // Conversion factor from seconds to minutes
#define TIME_TO_SLEEP 10                  // Duration in minutes for deep sleep between readings

// Message Type Definitions

extern const uint8_t messageType1;       // Message type identifier for Temperature data
extern const uint8_t messageType2;       // Message type identifier for TDS data
extern const uint8_t messageType3;       // Message type identifier for pH data
extern const uint8_t messageLength;      // Length of the I2C message to transmit
extern const uint8_t slaveAddress;       // I2C Slave address for the ESP32

#endif // SENSOR_CONFIG
