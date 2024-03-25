#include <LiquidCrystal_I2C.h>   // Library for LCD
#include <OneWire.h>             // Library for Signal on Digital Pin for OneWire Device
#include <DallasTemperature.h>   // Library for Temperature Probe

#define ONE_WIRE_BUS 2           // Data wire is plugged into port 2 on the Feather
#define GPIO_PIN 15              // Power Port is GPIO PIN 15
#define samplingInterval 20000   // Interval for sampling

OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass oneWire reference to Dallas Temperature

float temp = 0;                  // Variable to store temperature
int slaveAddress = 8;           // Define slave address for I2C communication
uint8_t messageType = 0x02;     // Message type correlates with boot sequence
uint8_t messageLength = 8;      // Length of the message
bool codeExecuted = false;       // Flag for code execution

// LCD Address
LiquidCrystal_I2C lcd(0x20, 16, 2); // I2C address 0x20, 16 columns and 2 rows

void setup(void) {
  Serial.begin(9600);          // Start serial port for debugging
  // Wait for serial port to connect
  while (!Serial) {
    ;
  }
  pinMode(GPIO_PIN, OUTPUT);   // Set GPIO PIN as OUTPUT for controlling power
  Wire.begin();                // Join I2C bus as master device (message sender)
  int bootSequence = 2;        // Initialize boot priority
  delay(2500*bootSequence);    // Wait for "x" seconds after boot to reduce init competition for the LCD
  /*
  lcd.init();                  // Initialize the LCD
  lcd.backlight();             // Turn on the LCD backlight
  delay(5000);                 // Delay for the LCD
  lcd.setCursor(0, 0);         // Set the LCD Cursor
  lcd.print("Master - F2");    // Display device info on LCD
  lcd.setCursor(0, 1);         // Set the LCD Cursor
  lcd.print("Huzzah Feather 2"); // Indicate Extended Feather ID
  lcd.setCursor(0, 0);         // Reset cursor position
  delay(5000);                 // Wait for 5 seconds
  lcd.print("F2-SetupComplete");// Indicate setup completion on LCD
  */
  sensors.begin();             // Start up the temperature sensor library
  if (!codeExecuted) {
    delay(5000);
    codeExecuted = true;
  }
}

void loop() {
  
  static unsigned long samplingTime = millis();
  if (millis() - samplingTime > samplingInterval) {
    digitalWrite(GPIO_PIN, HIGH); // Power on the sensor
    delay(500);                   // Wait for sensor stabilization
    sensors.requestTemperatures(); // Send command to get temperatures
    temp = sensors.getTempCByIndex(0); // Read temperature value
    Serial.println(temp); // Print temperature to serial monitor
    delay(500);                   // Wait for sensor stabilization
    digitalWrite(GPIO_PIN, LOW);  // Power off the sensor
    // printValue();                 // Print temperature on LCD
    samplingTime = millis();      // Set the sampling Time
  }
  delay(5000);
  transmitSlave();              // Transmit temperature to slave device
  
}

/*
// Function to print temperature on LCD
void printValue() {
  lcd.clear();                  // Clear LCD display
  lcd.setCursor(0, 0);         // Set cursor position
  lcd.print("Temp Value");      // Display label
  lcd.setCursor(0, 1);         // Set cursor position
  lcd.print(temp);              // Display temperature value
  lcd.setCursor(6, 1);         // Set cursor position
  lcd.print("deg C");           // Display temperature unit
}
*/

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
  Serial.println("Message Sent");       // Print status message
}
