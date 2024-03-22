#include <LiquidCrystal_I2C.h>  // Library for LCD
#include <Wire.h>               // Library for Wire transmission

LiquidCrystal_I2C lcd(0x20, 16, 2); // LCD with I2C address 0x20, 16 columns and 2 rows
int slaveAddress = 9;

void setup() {
  Wire.begin(slaveAddress);  // Initialize I2C communication as slave with address 8
  Wire.onReceive(receiveEvent); // Register receiveEvent function to handle I2C data reception
  Serial.begin(9600); // Initialize serial communication
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB port only)
  }

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight
  lcd.setCursor(0, 0); // Set cursor to position (0, 0)
  lcd.print("Slave - F3"); // Display slave information on the LCD
  lcd.setCursor(0, 1); // Set cursor to position (0, 1)
  lcd.print("Huzzah Feather 3"); // Display device information on the LCD
}

void loop() {
  delay(100); // Delay for stability
}

// Function to handle incoming I2C data
void receiveEvent(int bytes) {
  while (Wire.available()) { // Loop through all received bytes
    char c = Wire.read(); // Read byte as a character
    Serial.print(c); // Print the received character to serial monitor
  }
  Serial.println(); // Print a newline character after receiving all bytes
}
