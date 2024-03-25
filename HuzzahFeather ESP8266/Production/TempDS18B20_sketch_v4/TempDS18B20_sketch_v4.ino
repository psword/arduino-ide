#include <Wire.h>
#include <OneWire.h>             // Library for Signal on Digital Pin for OneWire Device
#include <DallasTemperature.h>   // Library for Temperature Probe

#define ONE_WIRE_BUS 2           // Data wire is plugged into port 2 on the Feather
#define GPIO_PIN 15              // Power Port is GPIO PIN 15
#define samplingInterval 20000   // Interval for sampling
#define sendingInterval 10000   // Interval for sending

OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass oneWire reference to Dallas Temperature

float temp = 0;                  // Variable to store temperature
int slaveAddress = 8;           // Define slave address for I2C communication
uint8_t messageType = 0x02;     // Message type correlates with boot sequence
uint8_t messageLength = 8;      // Length of the message
bool codeExecuted = false;       // Flag for code execution

void setup(void) {
  Serial.begin(9600);          // Start serial port for debugging
  // Wait for serial port to connect
  while (!Serial) {
    ;
  }
  pinMode(GPIO_PIN, OUTPUT);   // Set GPIO PIN as OUTPUT for controlling power
  Wire.begin();                // Join I2C bus as master device (message sender)
  sensors.begin();             // Start up the temperature sensor library
  if (!codeExecuted) {
    int tempSenseIterations = 10;
    
    delay(500);                   // Wait for sensor stabilization
    for (int i = 0; i < tempSenseIterations; i++) {
      delay(500);               // Wait for sensor stabilization
      sensors.requestTemperatures(); // Send command to get temperatures
      temp = sensors.getTempCByIndex(0); // Read temperature value           
    }
    digitalWrite(GPIO_PIN, LOW); // Power on the sensor
    Serial.println(temp); // Print temperature to serial monitor
    codeExecuted = true;
  }
}

void loop() {
  
  static unsigned long samplingTime = millis();
  if (millis() - samplingTime > samplingInterval) {
    digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
    delay(100);                   // Wait for sensor stabilization
    sensors.requestTemperatures(); // Send command to get temperatures
    temp = sensors.getTempCByIndex(0); // Read temperature value
    Serial.println(temp); // Print temperature to serial monitor
    delay(100);                   // Wait for sensor stabilization
    digitalWrite(GPIO_PIN, LOW);  // Power off the sensor
    // printValue();                 // Print temperature on LCD
    samplingTime = millis();      // Set the sampling Time
  }
  
  static unsigned long sendingTime = millis();
  if (millis() - sendingTime > sendingInterval) {
    transmitSlave();              // Transmit temperature to slave device
    sendingTime = millis();
  }
}

// Transmit temperature value to slave device
void transmitSlave() {
  Wire.beginTransmission(slaveAddress); // Start I2C transmission with slave (Arduino)
  Wire.write(messageType);              // Send message type
  Wire.write(messageLength);            // Send message length
  uint8_t byteArray[sizeof(float)];    // Create byte array to store temperature
  memcpy(byteArray, &temp, sizeof(float)); // Copy temperature value to byte array
  for (int i = 0; i < sizeof(float); i++) {
    Wire.write(byteArray[i]);          // Send temperature byte by byte
  }
  Wire.endTransmission();               // End transmission
}
