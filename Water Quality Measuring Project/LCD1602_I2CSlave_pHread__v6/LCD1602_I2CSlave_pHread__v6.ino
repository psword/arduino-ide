#include <LiquidCrystal_I2C.h>  // Library for LCD
#include <Wire.h>               // Library for Wire transmission

// Constants
#define LED 13                  // Pin for LED indicator
#define POWER_PIN 8
#define sensorPin A0
#define MAX_DATA_LENGTH 20         // Define max data for sending
#define samplingInterval 60000
#define receivedPrintInterval 10000      // Print of incoming samples (use for debugging)
#define printInterval 20000           // Print of pH reading (use for debugging)
#define Offset 0.37             // Deviation compensation for pH sensor
#define ArrayLength  40         // Length of array for averaging pH values

// Global Variables
uint8_t receivedData[MAX_DATA_LENGTH]; // Array to store received data
uint8_t receivedMessageType;            // Received message type
uint8_t messageType = 0x01;             // Message type correlates with boot sequence
uint8_t messageLength = 8;              // Length of message to transmit
uint8_t receivedMessageLength;          // Length of received message
int pHArray[ArrayLength];               // Array to store pH values
int pHArrayIndex = 0;                    // Index for pH array
int slaveAddress = 8;                    // Slave address
bool relayMessageFlag = false;           // Flag to indicate relay message
bool relayMessagePrint = false;          // Flag to indicate relay message print (use for debugging)
float pHValue_global = 0;                // Global pH value
float relayFloatValue;
bool codeExecuted = false;               // Flag for code execution
unsigned long samplingTime = 0;           // Initialize timer
unsigned long receivedPrintTime = 0;      // Initialize timer
unsigned long printTime = 0;              // Initialize timer
static bool powerOn;                      // Flag to track sensor power

LiquidCrystal_I2C lcd(0x20, 16, 2); // LCD with I2C address 0x20, 16 columns and 2 rows

void setup() {
  // Initialize I2C communication as slave with address 8
  Wire.begin(slaveAddress); 
  // TWAR = (slaveAddress << 1) | 1; // Enable listening on broadcast 

  // Register functions to handle I2C data reception and requests
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent); 

  // Set LED pin as output
  pinMode(LED, OUTPUT);

  // Set PIN 8 as output
  pinMode(POWER_PIN, OUTPUT);

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
  
  // Delay for the LCD
  delay(1000);
  
  // Display slave information on the LCD
  lcd.setCursor(0, 0); 
  lcd.print("Slave - A3"); 
  
  // Display device information on the LCD
  lcd.setCursor(0, 1); 
  lcd.print("Arduino R3"); 
  delay(5000);
  
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Initial pH");
  lcd.setCursor(0, 1);
  lcd.print("Processing...");
  delay(5000);
  
  // First code execution to normalize pH level
  if (!codeExecuted) {
    int pHsenseIterations = 60;
    // Turn on the power just before pH sensing
    digitalWrite(POWER_PIN, HIGH);
    delay(1000); 
    for (int i = 0; i < pHsenseIterations; i++) {
      // Perform pH sensing
      static float pHValue, voltage;
      // Collect pH readings
      pHArray[pHArrayIndex++] = analogRead(sensorPin);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      voltage = avergearray(pHArray, ArrayLength) * 5.0 / 1024;
      pHValue = 3.5 * voltage + Offset;
      digitalWrite(LED,digitalRead(LED)^1);
      pHValue_global = pHValue;
      delay(250);
    }
    digitalWrite(POWER_PIN, LOW);
    Serial.println("Setup Complete.");
    lcd.clear();
    powerOn = false;   // Set power flag
    codeExecuted = true;
  }
}

void loop() {
  unsigned long currentMillis = millis(); // Current time

  // Task 1: LCD Printing
  if (currentMillis - receivedPrintTime > receivedPrintInterval) {
    lcd.setCursor(0, 0);
    lcd.print(receivedMessageType);
    lcd.print(" Remote Msg");
    lcd.setCursor(0, 1);
    lcd.print(relayFloatValue);
    lcd.print("00");
    receivedPrintTime = currentMillis; // Update printing time
  }

  // Task 2: pH Sampling
  if (currentMillis - samplingTime > samplingInterval) {
    samplingTime = currentMillis; // Update sampling time
    
    // Initialize variables for nested timer
    unsigned long nestedTimer = millis(); // Timer for the nested loop
    int iterations = 0; // Number of iterations completed
    
    // Nested timer loop for pH sampling
    while (iterations < 40) {
      if (millis() - nestedTimer >= 250) { // Check if 500ms has passed
        samplepH(); // Function to sample pH values
        nestedTimer = millis(); // Reset nested timer
        iterations++; // Increment iterations
        
        // Check if all iterations are completed
        if (iterations >= 40) {
          digitalWrite(POWER_PIN, LOW); // Turn off power
          powerOn = false; // Reset power flag
        }
      }
    }
  }

  // Task 3: Periodic Printing
  if (currentMillis - printTime > printInterval) {
    lcd.setCursor(0, 0);
    lcd.print(messageType);
    lcd.print(" Local Msg ");
    lcd.setCursor(0, 1);
    lcd.print(pHValue_global);
    lcd.print("00");
    printTime = currentMillis; // Update printing time
  }
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

    if (receivedMessageType == 0x02) {
      float receivedFloatValue;
      // Reconstruct the float value from receivedData
      memcpy(&receivedFloatValue, receivedData, sizeof(float));
      relayFloatValue = receivedFloatValue;
    }
    if (receivedMessageType == 0x03) {
      float receivedFloatValue;
      // Reconstruct the float value from receivedData
      memcpy(&receivedFloatValue, receivedData, sizeof(float));
      relayFloatValue = receivedFloatValue;
    }
  }
}


// Function to handle outgoing I2C data
void requestEvent() {
  if (relayMessageFlag) {
    // Send relay message
    Wire.write(receivedMessageType);
    Wire.write(receivedMessageLength);
    uint8_t byteArray[sizeof(float)];
    memcpy(byteArray, &relayFloatValue, sizeof(float));
    for (int i = 0; i < sizeof(float); i++) {
      Wire.write(byteArray[i]);
      // Serial.println("sent remote"); (temporary debugging only)
    }
    // Reset relay message flag
    relayMessageFlag = false;
    relayMessagePrint = true;
  } else {
    // Send pH transmission message
    Wire.write(messageType);
    Wire.write(messageLength);
    uint8_t byteArray[sizeof(float)];
    memcpy(byteArray, &pHValue_global, sizeof(float));
    for (int i = 0; i < sizeof(float); i++) {
      Wire.write(byteArray[i]);
    // Serial.println("sent local"); (temporary debugging only)
    }
  }
}

// pH sampling function
void samplepH() {

 if (!powerOn) {
    digitalWrite(POWER_PIN, HIGH); // Turn on power if not already on
    powerOn = true; // Set power flag
  }
  
  // Perform pH sensing
  pHArray[pHArrayIndex++] = analogRead(sensorPin);
  if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
  float voltage = avergearray(pHArray, ArrayLength) * 5.0 / 1024;
  float pHValue = 3.5 * voltage + Offset;
  digitalWrite(LED, digitalRead(LED) ^ 1);
  pHValue_global = pHValue;
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
