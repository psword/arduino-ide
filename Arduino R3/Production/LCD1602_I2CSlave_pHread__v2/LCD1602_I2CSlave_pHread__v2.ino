#include <LiquidCrystal_I2C.h>  // Library for LCD
#include <Wire.h>               // Library for Wire transmission

// Pin Definitions
#define SensorPin A0            // pH meter Analog output to Arduino Analog Input 0
#define POWER_PIN 8             // Define digital pin 8 as the power pin for the pH Sensor
#define LED 13                  // Pin for LED indicator

// Constants
#define Offset 0.37             // Deviation compensation for pH sensor
#define samplingInterval 20000  // Sampling interval for pH reading
#define printInterval 20000     // Interval for printing pH value
#define ArrayLength  40         // Length of array for averaging pH values
#define MAX_DATA_LENGTH 20      // Maximum length of received data, adjust as needed

// Global Variables
uint8_t receivedData[MAX_DATA_LENGTH]; // Array to store received data
uint8_t receivedMessageType;            // Received message type
uint8_t messageType = 0x01;             // Example message type
uint8_t messageLength = 8;              // Length of message to transmit
uint8_t receivedMessageLength;          // Length of received message
int pHArray[ArrayLength];               // Array to store pH values
int pHArrayIndex = 0;                    // Index for pH array
int slaveAddress = 8;                    // Slave address
int duration = 60;                       // Duration for pH sensing
bool relayMessageFlag = false;           // Flag to indicate relay message
float pHValue_global = 0;                // Global pH value

LiquidCrystal_I2C lcd(0x20, 16, 2); // LCD with I2C address 0x20, 16 columns and 2 rows

void setup() {
  // Initialize I2C communication as slave with address 8
  Wire.begin(slaveAddress); 
  TWAR = (slaveAddress << 1) | 1; // Enable listening on broadcast 

  // Register functions to handle I2C data reception and requests
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent); 

  // Set LED pin as output
  pinMode(LED, OUTPUT);

  // Initialize serial communication
  Serial.begin(9600); 

  // Wait for serial port to connect
  while (!Serial) {
    ;
  }

  // Initialize the LCD
  lcd.init(); 
  // Turn on the LCD backlight
  lcd.backlight(); 
  // Display slave information on the LCD
  lcd.setCursor(0, 0); 
  lcd.print("Slave - A3"); 
  // Display device information on the LCD
  lcd.setCursor(0, 1); 
  lcd.print("Arduino R3"); 
}

void loop() {
  // Delay for stability
  delay(500); 
  // Perform pH sensing
  pHsense();
}

// Function to handle incoming I2C data
void receiveEvent(int bytes) {
  int index = 0;
  // Loop through all received bytes
  while (Wire.available()) {
    // Read the message type byte
    receivedMessageType = Wire.read(); 
    // Read the message length byte
    receivedMessageLength = Wire.read(); 
    // Read each byte of the message
    for (int i = 0; i < receivedMessageLength; i++) {
      receivedData[i] = Wire.read(); 
    }
    // Set relay message flag to true
    relayMessageFlag = true;
  }
}

// Function to handle outgoing I2C data
void requestEvent() {
  if (relayMessageFlag) {
    // Send relay message
    Wire.write(receivedMessageType);
    Wire.write(receivedMessageLength);
    for (int i = 0; i < receivedMessageLength; i++) {
      Wire.write(receivedData[i]); 
    }
    // Reset relay message flag
    relayMessageFlag = false; 
  } else {
    // Send pH transmission message
    Wire.write(messageType);
    Wire.write(messageLength);
    uint8_t byteArray[sizeof(float)];
    memcpy(byteArray, &pHValue_global, sizeof(float));
    for (int i = 0; i < sizeof(float); i++) {
      Wire.write(byteArray[i]);
    }
  }
}

// pH sensing function
void pHsense() {
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;
  if (millis() - samplingTime > samplingInterval) {
    // Turn on the power
    digitalWrite(POWER_PIN, HIGH); 
    delay(500);
    // Collect pH readings
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
    voltage = avergearray(pHArray, ArrayLength) * 5.0 / 1024;
    pHValue = 3.5 * voltage + Offset;
    samplingTime = millis();
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
