/*

 */

#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Wire.h> //Library for Wire transmission

LiquidCrystal_I2C lcd(0x20, 16, 2); // I2C address 0x20, 16 column and 2 rows
char inData[] = {'0','0'};
byte index = 0;

void setup() {
  Wire.begin();
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  lcd.init(); // initialize the lcd
  lcd.backlight(); //initialize the backlight
}

void loop() {

  lcd.clear();                 // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("Arduino");        // print message at (0, 0)
  lcd.setCursor(2, 1);         // move cursor to   (2, 1)
  lcd.print("GetStarted.com"); // print message at (2, 1)
  delay(2000);                 // display the above for two seconds

  lcd.clear();                  // clear display
  lcd.setCursor(3, 0);          // move cursor to   (3, 0)
  lcd.print("DIYables");        // print message at (3, 0)
  lcd.setCursor(0, 1);          // move cursor to   (0, 1)
  lcd.print("www.diyables.io"); // print message at (0, 1)
  delay(2000);   

  Wire.requestFrom(8, 2);    // request 2 bytes from peripheral device #8
  index = 0;

  while (Wire.available()) { // peripheral may send less than requested
    if(index < 5) {
      char c = Wire.read(); // receive a byte as character
      inData[index] = c;
      index++;
    }
    
  if (index > 0) {
    char * ptr;
    long dist;
    TDS = strtol(inData, &ptr, 10);
    Serial.print("TDS: ");
    Serial.print(TDS);
    Serial.println(" ppm");
  }
  else {
    Serial.println("No data");
  }
}
