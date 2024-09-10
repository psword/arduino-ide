/*
 * Project: ESP32 Uptime Tracker with Deep Sleep
 * Description: 
 *              This code uses the Thinger.io platform for data transmission and incorporates I2C communication 
 *              with a slave device for receiving sensor data. This is an extension for the development of a sensor device.
 *              It tracks the uptime of an ESP32 microcontroller, even through deep sleep cycles. This is an ongoing project
 *              and is meant to be extended.
 * Author: Philip
 * Written for: ESP32 Dev Board
 */

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include <WiFi.h>
#include <ThingerWifi.h>
#include <ThingerESP32.h>
#include "esp_sleep.h"
#include "arduino_secrets.h"

// Conversion factor definitions
#define uS_TO_S_FACTOR 1000000    // Conversion factor for microseconds to seconds
#define mS_TO_S_FACTOR 1000       // Conversion factor for milliseconds to seconds
#define S_TO_MIN_FACTOR 60        // Conversion factor for seconds to minutes

// Sleep and boot settings
#define TIME_TO_SLEEP 10           // Time to sleep in minutes, should be the same as all other modules
#define BOOT_THRESHOLD 2          // Number of boots before data transmission

// Other definitions
#define _DISABLE_TLS_             // Disable TLS connectivity for testing
#define THINGER_SERIAL_DEBUG      // Enable detailed Thinger.io debug output

// Variable declarations
uint8_t messageType1 = 0x01;      // I2C message type identifier 1
uint8_t messageType2 = 0x02;      // I2C message type identifier 2
uint8_t messageType3 = 0x03;      // I2C message type identifier 3
uint8_t messageType4 = 0x04;      // I2C message type identifier 4 (used for sleep trigger)
uint8_t messageLength = 8;        // Length of the message to be transmitted via I2C
uint8_t slaveAddress = 0x08;      // I2C Slave address

// Location and environmental data (used for testing)
const float latitude = 60.816138638727466; // Default latitude
const float longitude = 23.623367909936814; // Default longitude
String localeName = "Forssa"; // Default location name
String locationName = "HAMK"; // Default specific location name

// Runtime flags and counters
bool flag = false;                // Cloud transmission flag
RTC_DATA_ATTR float receivedFloatTemp; // Received temperature value
RTC_DATA_ATTR float receivedFloatTds;  // Received TDS (Total Dissolved Solids) value
RTC_DATA_ATTR float receivedFloatpH;   // Received pH value
RTC_DATA_ATTR bool isFirstBoot = true; // Flag to check if it's the first boot after deep sleep
RTC_DATA_ATTR uint16_t bootCounter = 0; // Counter to track the number of boots

// Uptime tracking variables
RTC_DATA_ATTR unsigned long hours = 0;   // Total hours of uptime
RTC_DATA_ATTR unsigned long minutes = 0; // Total minutes of uptime
RTC_DATA_ATTR unsigned long seconds = 0; // Total seconds of uptime
uint64_t elapsedMillis;                  // Elapsed time in milliseconds

// Function prototypes
void receiveData(int bytes);       // Function to handle I2C data reception
float readFloatFromWire();         // Function to read a float value from the I2C bus
void transmitDataToCloud();        // Function to transmit data to the cloud via Thinger.io
void addSleepMillis(uint64_t sleepMillis); // Function to add sleep time to the uptime
void updateUptime();               // Function to update uptime based on the current millis()

// Thinger.io setup
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup()
{
    Serial.begin(9600); // Initialize the serial port for debugging
    while (!Serial)
    {
        ; // Wait for the serial port to connect
    }

    // Initialize I2C as a slave on the specified address
    Wire.begin(slaveAddress);      // Set the I2C slave address to 0x08
    Wire.onReceive(receiveData);   // Register the data reception handler

    // Add WiFi credentials
    thing.add_wifi(ssid, ssid_pass);

    // Define Thinger.io resources
    thing["1000"] >> [](pson& out){
        // Populate Thinger.io resources with data
        out["temperature"] = receivedFloatTemp;
        out["pH"] = receivedFloatpH;
        out["Tds"] = receivedFloatTds;
        out["latitude"] = latitude;
        out["longitude"] = longitude;
        out["localeName"] = localeName;
        out["locationName"] = locationName;
        out["hoursUptime"] = hours;
        out["minutesUptime"] = minutes;
        out["secondsUptime"] = seconds;
        out["bootThreshold"] = BOOT_THRESHOLD;
        out["bootCounter"] = bootCounter;
        out["deepSleepTimerMinutes"] = TIME_TO_SLEEP;
    };

    if (!isFirstBoot) {
        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
            Serial.println("Waking up from deep sleep");
            // Add sleep time to the uptime
            uint64_t sleepMillis = TIME_TO_SLEEP * S_TO_MIN_FACTOR * mS_TO_S_FACTOR;
            addSleepMillis(sleepMillis);
        }
        
        // Update the uptime with the current running time
        updateUptime();

        // Increment the boot counter
        bootCounter++;
        Serial.print("Boot Counter : ");
        Serial.println(bootCounter);
    } else {
        // Handle the first boot case
        isFirstBoot = false;
        Serial.println("First boot. Initializing uptime...");

        // Initialize uptime on the first boot
        updateUptime();

        Serial.println("First boot. Entering deep sleep...");
        // Enable the timer to wake up from deep sleep
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * S_TO_MIN_FACTOR * uS_TO_S_FACTOR);
        esp_deep_sleep_start(); // Enter deep sleep mode
    }
}

void loop()
{
    thing.handle(); // Handle Thinger.io communications

    // Check if it's time to transmit data to the cloud
    if (!flag && bootCounter == BOOT_THRESHOLD) {
        updateUptime();          // Update uptime before transmitting
        transmitDataToCloud();   // Transmit data to the cloud
    }
}

// I2C data reception handler
void receiveData(int bytes)
{
    Serial.print("Data received. Bytes: ");
    Serial.println(bytes);

    while (Wire.available() >= 2) // Ensure there are at least 2 bytes to read (message type and length)
    {
        uint8_t messageType = Wire.read();   // Read message type byte
        uint8_t messageLength = Wire.read(); // Read message length byte

        Serial.print("Message type: ");
        Serial.print(messageType);
        Serial.print(", Message length: ");
        Serial.println(messageLength);

        if (messageType == messageType4) // Check if the message is a sleep trigger
        {
            if (flag && bootCounter == BOOT_THRESHOLD) {
                bootCounter = 0; // Reset the boot counter after data transmission
            }
            updateUptime(); // Update uptime before sleeping
            Serial.println("Received sleep trigger. Going back to deep sleep...");
            // Enable the timer to wake up from deep sleep
            esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * S_TO_MIN_FACTOR * uS_TO_S_FACTOR);
            esp_deep_sleep_start(); // Enter deep sleep mode
        }

        if (messageLength == 8 && Wire.available() >= 4)
        {
            float receivedValue = readFloatFromWire(); // Read the float value from the I2C bus
            switch (messageType)
            {
            case 0x01:
                receivedFloatTemp = receivedValue; // Store received temperature value
                Serial.print("Temperature received: ");
                Serial.println(receivedFloatTemp);
                break;
            case 0x02:
                receivedFloatTds = receivedValue; // Store received TDS value
                Serial.print("TDS received: ");
                Serial.println(receivedFloatTds);
                break;
            case 0x03:
                receivedFloatpH = receivedValue; // Store received pH value
                Serial.print("pH received: ");
                Serial.println(receivedFloatpH);
                break;
            default:
                Serial.println("Unknown message type");
                break;
            }
        }
    }
}

// Function to read a float value from the I2C bus
float readFloatFromWire()
{
    uint8_t byteArray[sizeof(float)]; // Create a byte array to store the float bytes
    for (int i = 0; i < sizeof(float); i++)
    {
        byteArray[i] = Wire.read(); // Read bytes into the array
    }
    float value;
    memcpy(&value, byteArray, sizeof(float)); // Copy bytes into the float variable
    return value;                             // Return the reconstructed float value
}

// Function to transmit data to the cloud using Thinger.io
void transmitDataToCloud()
{
    Serial.println("Transmitting data to cloud...");
    thing.write_bucket("1000","1000"); // Write data to the Thinger.io data bucket
    flag = true;                       // Set the flag to indicate data transmission
}

// Function to add sleep time to the uptime
void addSleepMillis(uint64_t sleepMillis) {
    // Convert the sleep time in milliseconds to seconds, minutes, and hours
    seconds += sleepMillis / 1000;
    minutes += seconds / 60;
    hours += minutes / 60;

    // Keep seconds and minutes within their bounds
    seconds %= 60;
    minutes %= 60;
}

// Function to update uptime based on the current millis() value
void updateUptime() {
    uint64_t currentMillis = millis(); // Get the current uptime in milliseconds since the last wake-up

    // Update the total uptime in seconds, minutes, and hours
    seconds += currentMillis / 1000;
    minutes += seconds / 60;
    hours += minutes / 60;

    // Keep seconds and minutes within their bounds
    seconds %= 60;
    minutes %= 60;

    // Print updated uptime for debugging
    Serial.print("Current Millis: ");
    Serial.println(currentMillis);
    Serial.print("Updated Hours: ");
    Serial.println(hours);
    Serial.print("Updated Minutes: ");
    Serial.println(minutes);
    Serial.print("Updated Seconds: ");
    Serial.println(seconds);
}
