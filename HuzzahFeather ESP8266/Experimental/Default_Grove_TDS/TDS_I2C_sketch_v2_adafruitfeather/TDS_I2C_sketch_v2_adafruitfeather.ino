#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Wire.h>

#define sensor_Input_Pin A0   // Data Wire is plugged into ADC Port
#define GPIO_PIN 15           // Power Port is GPIO PIN 15
#define pollingInterval 10000   // Interval for sampling
#define samplingInterval 25000   // Interval for sampling

int sensorValue = 0;         // Variable to store sensor value
float tdsValue = 0;          // Variable to store tdsValue
float adjustedTDS;
float Voltage = 0;           // Variable to store Voltage
char tdsChar[8];             // Character array to store tds value as string
float VC = 3.3;              // Voltage Constant
int slaveAddress = 8;        // Define slave address
String FeatherID = "F1: ";     // Device ID
bool codeExecuted = false;         // Flag for code execution
const int16_t I2C_SLAVE = 8; // Define the I2C slave address
float receivedFloatValue;    // Variable to store received float value
float receivedFloatTemp;    // Variable to store received temperature value
uint8_t messageType = 0x03;     // Message type correlates with boot sequence
uint8_t messageLength = 8;      // Length of the message

// Constants for temperature coefficient calculation
const float tempCoefficient = 0;  // Initialize at zero
const float referenceTemp = 25.0;    // Reference temperature for calibration

// LCD Address
LiquidCrystal_I2C lcd(0x20, 16, 2); // I2C address 0x20, 16 columns and 2 rows

void setup() {
    Serial.begin(9600);    // Initialize serial communication
    // Wait for serial port to connect
    while (!Serial) {
    ;
    }
    pinMode(GPIO_PIN, OUTPUT);      // Set GPIO PIN as OUTPUT for controlling power
    Wire.begin();                   // Join I2C bus as master device (message sender)
    int bootSequence = 3;           // Initialize boot priority
    delay(2500*bootSequence);       // Wait for "x" seconds after boot to reduce init competition for the LCD
    lcd.init();                     // Initialize the LCD
    lcd.backlight();                // Turn on the LCD backlight
    lcd.setCursor(0, 0);            // Set the LCD Cursor
    lcd.print("Master - F1");       // Display device info on LCD
    lcd.setCursor(0, 1);            // Set the LCD Cursor
    lcd.print("Huzzah Feather 1");  // Set the LCD Cursor
    lcd.setCursor(0, 0);            // Set the LCD Cursor
    delay(5000);                    // Wait for 5 seconds
    lcd.print("F1-SetupComplete");  // Indicate setup completion on LCD
    delay(1000);                    // Wait for 1 second
}

void loop() {
    if (!codeExecuted) {
    delay(5000);
    codeExecuted = true;
    }
    
  static unsigned long pollingTime = millis();
  if (millis() - pollingTime > pollingInterval) {
    Wire.requestFrom(I2C_SLAVE, 8); // Request 8 bytes from slave device #8
    while (Wire.available()) { // Slave may send less than requested
      uint8_t messageType = Wire.read(); // Read the message type byte
      uint8_t messageLength = Wire.read(); // Read the message length byte

      // This block is for message type 0x02 (temperature value)
      if (messageType == 0x02 && messageLength == 8) {
        Serial.println("Received temperature value:");
        uint8_t byteArray[sizeof(float)]; // Create an array to store the bytes of the float
        for (int i = 0; i < messageLength; i++) {
          byteArray[i] = Wire.read(); // Read bytes of the message body into byteArray
        }
        // Reconstruct the float value from byteArray
        float *ptr = reinterpret_cast<float*>(byteArray); // Type punning
        receivedFloatValue = *ptr; // Assign the reconstructed float value
        receivedFloatTemp = receivedFloatValue;
        Serial.println(receivedFloatTemp); // Print the reconstructed temperature value
      }
    }
    pollingTime = millis();
  }
    
  static unsigned long samplingTime = millis();
  if (millis() - samplingTime > samplingInterval) {
    digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
    delay(500);                   // Wait for sensor stabilization
    sensorValue = analogRead(sensor_Input_Pin); // Read analog value from sensor
    delay(500);                   // Wait for sensor stabilization
    digitalWrite(GPIO_PIN, LOW);  // Power off the sensor
    samplingTime = millis();
  }

  Voltage = sensorValue * VC / 1024.0; // Convert analog reading to voltage (10-bit resolution for ESP8266, 3.3V)
  tdsValue = (133.42 / Voltage * Voltage * Voltage - 255.86 * Voltage * Voltage + 857.39 * Voltage) * 0.5; // Convert voltage value to TDS value
  adjustTDS(tdsValue, receivedFloatTemp); // Calculate the adjusted TDS
  Serial.print("Reported Voltage on the PIN = ");
  Serial.print(Voltage);        // Print voltage to serial monitor
  Serial.println(" volts");
  Serial.print("TDS Value = ");
  Serial.print(adjustedTDS);       // Print TDS value to serial monitor
  Serial.println(" ppm");
  transmitSlave();             // Transmit to slave;
  printValue();                 // Print TDS value on LCD
}

// Function to print TDS value on LCD
void printValue() {
    lcd.clear();                   // Clear LCD display
    lcd.setCursor(0, 0);          // Set cursor position
    lcd.print("TDS Value");       // Display label
    lcd.setCursor(0, 1);          // Set cursor position
    lcd.print(tdsValue);          // Display TDS value
    lcd.setCursor(6, 1);          // Set cursor position
    lcd.print("ppm");             // Display units
}

// Function to calculate adjusted TDS reading based on temperature
float adjustTDS(float rawTDS, float temperature) {
    // Calculate temperature correction factor
    float tempCorrection = tempCoefficient * (temperature - referenceTemp);
    
    // Adjust TDS reading based on temperature
    float adjustedTDS = rawTDS + tempCorrection;
    
    return adjustedTDS;
}

// Transmit to slave
void transmitSlave() {
  Wire.beginTransmission(slaveAddress); // Start I2C transmission with slave (Arduino)
  Wire.write(messageType);              // Send message type
  Wire.write(messageLength);            // Send message length
  uint8_t byteArray[sizeof(float)];    // Create byte array to store temperature
  memcpy(byteArray, &adjustedTDS, sizeof(float)); // Copy temperature value to byte array
  for (int i = 0; i < sizeof(float); i++) {
    Wire.write(byteArray[i]);          // Send temperature byte by byte
  }
  Wire.endTransmission();               // End transmission
  Serial.println("Message Sent");       // Print status message
}