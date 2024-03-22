#include <LiquidCrystal_I2C.h>  // Library for LCD
#include <Wire.h>               // Library for Wire transmission
#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.37            //deviation compensate
#define LED 13
#define samplingInterval 20000
#define printInterval 5000
#define ArrayLenth  40    //times of collection
#define POWER_PIN 8 // Define digital pin 8 as the power pin for the pH Sensor
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;

LiquidCrystal_I2C lcd(0x20, 16, 2); // LCD with I2C address 0x20, 16 columns and 2 rows
int slaveAddress = 8;
char receivedData[8];
int duration = 60;
float pHValue_global = 0;
char pHValue_char[8];
uint8_t messageType = 0x01; // Example message type
uint8_t messageLength = 8;

void setup() {
  Wire.begin(slaveAddress);  // Initialize I2C communication as slave with address 8
  TWAR = (slaveAddress << 1) | 1; // enable listening on broadcast 
  Wire.onReceive(receiveEvent); // Register receiveEvent function to handle I2C data reception
  Wire.onRequest(requestEvent); // Register requestEvent function to handle I2C data requests
  pinMode(LED,OUTPUT);
  Serial.begin(9600); // Initialize serial communication
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB port only)
  }

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight
  lcd.setCursor(0, 0); // Set cursor to position (0, 0)
  lcd.print("Slave - A3"); // Display slave information on the LCD
  lcd.setCursor(0, 1); // Set cursor to position (0, 1)
  lcd.print("Arduino R3"); // Display device information on the LCD
}

void loop() {
  delay(500); // Delay for stability
  pHsense();
}

// Function to handle incoming I2C data
void receiveEvent(int bytes) {
  int index = 0;
  while (Wire.available() && index < 8 - 1) { // Loop through all received bytes
    char c = Wire.read(); // Read byte as a character
    receivedData[index] = c;
    index++;
    Serial.print(c); // Print the received character to serial monitor
  }
  Serial.println(); // Print a newline character after receiving all bytes
}

// Function to handle outgoing I2C data
void requestEvent() {
  // Serial.print(receivedData);
  // Serial.println();
  Wire.write(receivedData);
  // dtostrf(pHValue_global, 8, 4, pHValue_char);
  Wire.write(messageType);
  Wire.write(messageLength);
  uint8_t byteArray[sizeof(float)];
  memcpy(byteArray, &pHValue_global, sizeof(float));
  for (int i = 0; i < sizeof(float); i++) {
  Wire.write(byteArray[i]);
  }
  //Wire.write(pHValue_char);
}

// pH function
void pHsense()
{
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(millis()-samplingTime > samplingInterval)
  {
      digitalWrite(POWER_PIN, HIGH); // Turn on the power
      delay(500);
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
      digitalWrite(POWER_PIN, LOW); // Turn off the power
      delay(500);
  }
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.print("Voltage:");
        Serial.print(voltage,2);
        Serial.print("    pH value: ");
    Serial.println(pHValue,2);
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
    pHValue_global = pHValue;

  lcd.clear();
  lcd.setCursor(0, 0); // Set cursor to position (0, 0)
  lcd.print("pH Value"); // Display pH header on the LCD
  lcd.setCursor(0, 1); // Set cursor to position (0, 0)
  lcd.print(pHValue_global); // Display pH information on the LCD
  }
}

// For pH reading
double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
