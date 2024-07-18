// Include necessary libaries for operation
#include <Wire.h>               // Wire Library
#include <OneWire.h>            // DS18B20 Temperature Sensor
#include <DallasTemperature.h>  // DS18B20 Temperature Sensor
#include <DFRobot_ESP_PH.h>     // DFRobot pH Library v2.0
#include <EEPROM.h>             // EEPROM library
#include "temperature_sensor.h" // Temperature Sensor Class
#include "tds_sensor.h"         // TDS sensor class
#include "ph_sensor.h"          // pH sensor class
#include "esp_sleep.h"          // ESP32 deep sleep library
#include "sensor_config.h"      // Parameters, variables, and constants
#include "sensor_state_machine.h" // State machine to control each sensor
#include "transmit_functions.h"  // Include I2C transmit function

bool codeExecuted = false;     // Flag for code execution
const uint8_t messageType4 = 0x04;   // Message type identifier

// Define function prototypes
void goToDeepSleep();

// Create an instance of TemperatureSensor class (onewire PIN, number of samples that buffer retains)
TemperatureSensor tempSensorInstance(ONE_WIRE_BUS);
// Providing a custom value for tempSenseIterations
// TemperatureSensor tempSensorInstance(ONE_WIRE_BUS, 15);

// Create an instance of TdsSensor class (voltage, kCoefficient, reference temp, ADC bits, GPIO PIN, number of samples that buffer retains)
TdsSensor tdsSensorInstance(3.3, 0.02, 25.0, 4096, TDS_SENSOR_BUS, 15);

// Create an instance of pHSensor class (voltage in 1000 units, reference temp, ADC bits, GPIO PIN, number of samples that buffer retains)
pHSensor pHSensorInstance(3300, 25.0, 4096, PH_SENSOR_BUS, 15);

SensorStateMachine<TemperatureSensor> tempStateMachine(tempSensorInstance, TEMP_SENSOR_POWER_PIN);
SensorStateMachine<TdsSensor> tdsStateMachine(tdsSensorInstance, TDS_SENSOR_POWER_PIN);
SensorStateMachine<pHSensor> pHStateMachine(pHSensorInstance, PH_SENSOR_POWER_PIN);

void setup() {
    Serial.begin(9600); // Start serial port for debugging
    // Wait for serial port to connect
    while (!Serial) {
        ;
    }
    pinMode(TEMP_SENSOR_POWER_PIN, OUTPUT);    // Set GPIO PIN as OUTPUT for controlling power
    pinMode(TDS_SENSOR_POWER_PIN, OUTPUT);     // Set GPIO PIN as OUTPUT for controlling power
    pinMode(PH_SENSOR_POWER_PIN, OUTPUT);      // Set GPIO PIN as OUTPUT for controlling power
    pinMode(ONE_WIRE_BUS, INPUT);      // Set GPIO PIN as INPUT for reading data
    pinMode(TDS_SENSOR_BUS, INPUT);    // Set GPIO PIN as INPUT for reading data
    pinMode(PH_SENSOR_BUS, INPUT);     // Set GPIO PIN as INPUT for reading data
    EEPROM.begin(32);                  // Needed for EEPROM
    Wire.begin();                      // Join I2C bus as master device (message sender)
    tempSensorInstance.beginSensors(); // Start up the temperature sensor library
    pHSensorInstance.beginSensors();   // Start up the pH sensor library

    // Check if the system is waking up from deep sleep
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("Waking up from deep sleep");
        codeExecuted = true; // Set code execution flag
    }

    if (!codeExecuted) {
        Serial.println("Going to deep sleep for the first time.");
        goToDeepSleep(); // Enter deep sleep initially
    }

    // The TDS sensor does not require initialization

    /* Perform any other custom initializations if code has not been executed before
    // if (!codeExecuted) {
    //     delay(10000);
    //     Serial.println("Setup Complete."); // Print setup message to serial monitor
    //     codeExecuted = true; // Set code execution flag
    } 
    */
}

void loop() {
    static bool tempStarted = false;
    static bool tdsStarted = false;
    static bool pHStarted = false;

    // Execute the state machine operations
    tempStateMachine.operate();
    tdsStateMachine.operate();
    pHStateMachine.operate();
    
    // Start the temperature sensor first if not already started
    if (!tempStarted) {
        tempStateMachine.start();
        tempStarted = true;
    }

    // Logic to transition between state machines in sequence
    if (tempStateMachine.isOff() && !tdsStarted) {
        tdsStateMachine.start();
        tdsStarted = true;
    }
    if (tempStateMachine.isOff() && tdsStateMachine.isOff() && !pHStarted) {
        pHStateMachine.start();
        pHStarted = true;
    }

    // Check if all messages have been sent, then go to deep sleep
    if (tempStateMachine.isOff() && tdsStateMachine.isOff() && pHStateMachine.isOff()) {
        transmitSleep(messageType4); // Transmit using messageType4
        goToDeepSleep();
    }
}

void goToDeepSleep() {
    // Set the deep sleep timer (for example, 10 minutes)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * S_TO_MIN_FACTOR * uS_TO_S_FACTOR); // 10 minutes in microseconds
    Serial.println("Going to deep sleep now");
    esp_deep_sleep_start();
}
