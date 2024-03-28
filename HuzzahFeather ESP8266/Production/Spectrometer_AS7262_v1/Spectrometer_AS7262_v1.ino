/***************************************************************************
  This is a library for the Adafruit AS7262 6-Channel Visible Light Sensor

  This sketch reads the sensor

  Designed specifically to work with the Adafruit AS7262 breakout
  ----> http://www.adafruit.com/products/3779
  
  These sensors use I2C to communicate. The device's I2C address is 0x49
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  
  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include "Adafruit_AS726x.h"

// Timing Intervals
#define SAMPLING_INTERVAL 10000  // Interval for sampling
#define SENDING_INTERVAL 5000    // Interval for sending
#define MESSAGE_VIOLET 0x04 // Define the message code for the violet channel value

Adafruit_AS726x ams;  //create the object

// Define variables
uint16_t sensorValues[AS726x_NUM_CHANNELS]; // Buffer to hold raw values
uint8_t messageLength = 8;  // Length of message to transmit
int slaveAddress = 8;            // Slave device address
uint16_t violetValue = sensorValues[AS726x_VIOLET]; // Define violet value

//buffer to hold calibrated values (not used by default in this example)
//float calibratedValues[AS726x_NUM_CHANNELS];

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  // initialize digital pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize Wire library
  Wire.begin();

  //begin and make sure we can talk to the sensor
  if(!ams.begin()){
    Serial.println("could not connect to sensor! Please check your wiring.");
    while(1);
  }
}

void loop() {

  //read the device temperature
  uint8_t temp = ams.readTemperature();
  
  ams.drvOn(); //uncomment this if you want to use the driver LED for readings
  
  static unsigned long samplingTime = millis();

  // Sampling Interval Check
  if (millis() - samplingTime > SAMPLING_INTERVAL) {
    ams.startMeasurement(); //begin a measurement
    
    //wait till data is available
    bool rdy = false;
    while(!rdy){
      delay(500);
      rdy = ams.dataReady();
    }
    //ams.drvOff(); //uncomment this if you want to use the driver LED for readings

    //read the values!
    ams.readRawValues(sensorValues);
    //ams.readCalibratedValues(calibratedValues);
    
    uint16_t violetValue = sensorValues[AS726x_VIOLET];


    Serial.print("Temp: "); Serial.print(temp);
    Serial.print(" Violet: "); Serial.print(sensorValues[AS726x_VIOLET]);
    Serial.print(" Blue: "); Serial.print(sensorValues[AS726x_BLUE]);
    Serial.print(" Green: "); Serial.print(sensorValues[AS726x_GREEN]);
    Serial.print(" Yellow: "); Serial.print(sensorValues[AS726x_YELLOW]);
    Serial.print(" Orange: "); Serial.print(sensorValues[AS726x_ORANGE]);
    Serial.print(" Red: "); Serial.print(sensorValues[AS726x_RED]);
    Serial.println();
    Serial.println();
    transmitSlave();
    samplingTime = millis();
  }
}

// Transmit to slave
void transmitSlave() {
    Wire.beginTransmission(slaveAddress); // Start I2C transmission with slave (Arduino)
    Wire.write(MESSAGE_VIOLET); // Send message type
    Wire.write(violetValue & 0xFF); // Send message length
    // uint8_t byteArray[sizeof(float)]; // Create byte array to store temperature
    // memcpy(byteArray, &adjustedTds, sizeof(float)); // Copy temperature value to byte array

    // for (int i = 0; i < sizeof(float); i++) {
    //     Wire.write(byteArray[i]); // Send temperature byte by byte
    // }

    Wire.endTransmission(); // End transmission
    Serial.println("Message Sent"); // Print status message
    Serial.println(MESSAGE_VIOLET); // Print status message
}

