#include <Wire.h>               // Library for Wire transmission

float F1 = 0;
float F2 = 0;

void setup() {
  Wire.begin(9);  // Initialize I2C communication as slave with address 9
  Wire.onReceive(receiveEvent); // Register receiveEvent function to handle I2C data reception
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() {
  
  // insert delay
  delay(100);

}

void receiveEvent(int bytes) {
  while (Wire.available()) { // Loop through all received bytes
    char c = Wire.read(); // Read byte as a character
    Serial.print(c); // Print the received character to serial monitor
  }
  Serial.println(); // Print a newline character after receiving all bytes
}