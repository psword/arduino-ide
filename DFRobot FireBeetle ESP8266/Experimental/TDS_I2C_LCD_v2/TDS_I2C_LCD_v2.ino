#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Wire.h>

#define sensor_Input_Pin A0   // Data Wire is plugged into ADC Port
#define GPIO_PIN 15           // Power Port is GPIO PIN 15

int sensorValue = 0;         // Variable to store sensor value
float tdsValue = 0;          // Variable to store tdsValue
float Voltage = 0;           // Variable to store Voltage
char tdsChar[8];             // Character array to store tds value as string
float VC = 3.3;              // Voltage Constant
int slaveAddress = 8;        // Define slave address
String MCUid = "F1: ";     // Device ID

// LCD Address
LiquidCrystal_I2C lcd(0x20, 16, 2); // I2C address 0x20, 16 columns and 2 rows

void setup() {
    Serial.begin(9600);    // Initialize serial communication
    pinMode(GPIO_PIN, OUTPUT); // Set GPIO PIN as OUTPUT for controlling power
    Wire.begin();          // Join I2C bus as master device (message sender)
    // lcd.init();            // Initialize the LCD
    // lcd.backlight();       // Turn on the LCD backlight
    // lcd.setCursor(0, 0);  // Set the LCD Cursor
    // lcd.print("Master - F1"); // Display device info on LCD
    // lcd.setCursor(0, 1);  // Set the LCD Cursor
    // lcd.print("FireBeetle8266 1");  // Set the LCD Cursor
    // lcd.setCursor(0, 0);  // Set the LCD Cursor
    // delay(5000);           // Wait for 5 seconds
    // lcd.print("F1-SetupComplete"); // Indicate setup completion on LCD
    // delay(1000);           // Wait for 1 second
}

void loop() {
    digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
    delay(5000);                   // Wait for sensor stabilization
    sensorValue = analogRead(sensor_Input_Pin); // Read analog value from sensor
    Serial.print("Reported Raw Sensor Value on the PIN = ");
    Serial.println(sensorValue);        // Print analog value to serial monitor
    Serial.print("Reported Raw Voltage Value on the PIN = ");
    Serial.println(sensorValue * VC / 1024.0);        // Print voltage to serial monitor after conversion to voltage
    Voltage = sensorValue * VC / 1024.0; // Convert analog reading to voltage (10-bit resolution for ESP8266, 3.3V)
    Serial.print("Reported Adjusted Voltage = ");
    Serial.print(Voltage);        // Print voltage to serial monitor
    Serial.println(" volts");
    tdsValue = ((133.42/Voltage*Voltage*Voltage) - (255.86*Voltage*Voltage) + (857.39*Voltage))*0.5; // Convert voltage value to TDS value
    Serial.print("TDS Value = ");
    Serial.print(tdsValue);       // Print TDS value to serial monitor
    Serial.println(" ppm");
    delay(5000);                   // Wait for sensor stabilization
    digitalWrite(GPIO_PIN, LOW);  // Power off the sensor
    dtostrf(tdsValue, 4, 4, tdsChar); // Convert float to char array with 6 characters and 2 decimal places
    // String MCUid = "F1: ";     // Device ID
    // Wire.beginTransmission(0);     // Start I2C transmission with slaves on broadcast address
    // Wire.write(MCUid.c_str()); // Send device ID
    // Wire.write(tdsChar);          // Send TDS value
    // Wire.endTransmission();       // End transmission
    transmitSlave();             // Transmit to slave;
    printValue();                 // Print TDS value on LCD
    delay(5000);                  // Wait before next iteration
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

// Transmit to slave
void transmitSlave() {
  Wire.beginTransmission(slaveAddress);    // Start I2C transmission with slave (Arduino)
  // Wire.write(MCUid.c_str()); // Send device ID
  Wire.write(tdsChar);         // Send temperature value
  Wire.endTransmission();       // End transmission
}