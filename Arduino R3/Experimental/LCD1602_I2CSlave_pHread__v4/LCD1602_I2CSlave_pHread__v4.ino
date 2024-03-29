#include <LiquidCrystal_I2C.h>  // Library for LCD
#include <Wire.h>               // Library for Wire transmission

#define LED 13                  // Pin for LED indicator
#define MAX_DATA_LENGTH 20         // Define max data for sending

// Global Variables
uint8_t receivedData[MAX_DATA_LENGTH]; // Array to store received data
uint8_t receivedMessageType;            // Received message type
uint8_t messageType = 0x01;             // Message type correlates with boot sequence
uint8_t messageLength = 8;              // Length of message to transmit
uint8_t receivedMessageLength;          // Length of received message
// int pHArray[ArrayLength];               // Array to store pH values
// int pHArrayIndex = 0;                    // Index for pH array
int slaveAddress = 8;                    // Slave address
bool relayMessageFlag = false;           // Flag to indicate relay message
// float pHValue_global = 0;                // Global pH value
float receivedFloatValue;
bool codeExecuted = false;               // Flag for code execution
unsigned long incrementTime = 0;         // Initialize incrementTime variable

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

  // Set PIN 8 as output
  // pinMode(POWER_PIN, OUTPUT);

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
  /*lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Initial pH");
  lcd.setCursor(0, 1);
  lcd.print("Processing...");
  delay(5000);
  // First code execution to normalize pH level
  */
  if (!codeExecuted) {
    /*
    int pHsenseIterations = 60;
    // Turn on the power just before pH sensing
    digitalWrite(POWER_PIN, HIGH);
    delay(500); 
    for (int i = 0; i < pHsenseIterations; i++) {
      // Perform pH sensing
      static float pHValue, voltage;
      // Collect pH readings
      pHArray[pHArrayIndex++] = analogRead(SensorPin);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      voltage = avergearray(pHArray, ArrayLength) * 5.0 / 1024;
      pHValue = 3.5 * voltage + Offset;
      Serial.print("Voltage:");
          Serial.print(voltage,2);
          Serial.print("    pH value: ");
      Serial.println(pHValue,2);
          digitalWrite(LED,digitalRead(LED)^1);
      pHValue_global = pHValue;
      }
    lcd.clear();
    lcd.setCursor(0, 0); // Set cursor to position (0, 0)
    lcd.print("pH Value"); // Display pH header on the LCD
    lcd.setCursor(0, 1); // Set cursor to position (0, 1)
    lcd.print(pHValue_global); // Display pH information on the LCD
    digitalWrite(POWER_PIN, LOW);
    */
    codeExecuted = true;
  }
}

void loop() {
  static unsigned long uptimeSecs = millis();
  delay(20000);
      /*
      delay(55000);
      int pHsenseIterations = 5;
      digitalWrite(POWER_PIN, HIGH);
      // Turn on the power just before pH sensing
      delay(500); 
      for (int i = 0; i < pHsenseIterations; i++) {
        delay(500); // Delay for stability of power pin
        // Perform pH sensing
        static float pHValue, voltage;
        // Collect pH readings
        pHArray[pHArrayIndex++] = analogRead(SensorPin);
        if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
        voltage = avergearray(pHArray, ArrayLength) * 5.0 / 1024;
        pHValue = 3.5 * voltage + Offset;
        Serial.print("Voltage:");
            Serial.print(voltage,2);
            Serial.print("    pH value: ");
        Serial.println(pHValue,2);
          digitalWrite(LED,digitalRead(LED)^1);
        pHValue_global = pHValue;
      }
      digitalWrite(POWER_PIN, LOW);
      lcd.clear();
      lcd.setCursor(0, 0); // Set cursor to position (0, 0)
      lcd.print("pH Value"); // Display pH header on the LCD
      lcd.setCursor(0, 1); // Set cursor to position (0, 1)
      lcd.print(pHValue_global); // Display pH information on the LCD
      */
    uptimeSecs = (millis() - uptimeSecs)/1000;
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Uptime"); 
    // Display device information on the LCD
    lcd.setCursor(0, 1); 
    lcd.print(uptimeSecs); 
    Serial.print("upTime: ");
    Serial.println(uptimeSecs);
}

// Function to handle incoming I2C data
void receiveEvent(int bytes) {
  int index = 0;
  // Loop through all received bytes
  while (Wire.available()) {
    // Read the message type byte
    receivedMessageType = Wire.read();
    Serial.println(receivedMessageType, HEX);
    // Read the message length byte
    receivedMessageLength = Wire.read();
    Serial.println(receivedMessageLength, HEX);
    // Read each byte of the message
    for (int i = 0; i < receivedMessageLength; i++) {
    receivedData[i] = Wire.read(); 
    }
    // Set relay message flag to true
    Serial.print(Wire.read());
    relayMessageFlag = true;
    if (receivedMessageType == 0x01) {
      float receivedFloatValue;
      // Reconstruct the float value from receivedData
      memcpy(&receivedFloatValue, receivedData, sizeof(float));
      Serial.print("Received float value: ");
      Serial.println(receivedFloatValue, 2); // Print the reconstructed float value
  }
    if (receivedMessageType == 0x02) {
      float receivedFloatValue;
      // Reconstruct the float value from receivedData
      memcpy(&receivedFloatValue, receivedData, sizeof(float));
      Serial.print("Received float value: ");
      Serial.println(receivedFloatValue, 2); // Print the reconstructed float value
      lcd.clear();
      lcd.setCursor(0, 0); // Set cursor to position (0, 0)
      lcd.print("Temp Value"); // Display pH header on the LCD
      lcd.setCursor(0, 1); // Set cursor to position (0, 1)
      lcd.print(receivedFloatValue); // Display pH information on the LCD
  }
    if (receivedMessageType == 0x03) {
      float receivedFloatValue;
      // Reconstruct the float value from receivedData
      memcpy(&receivedFloatValue, receivedData, sizeof(float));
      Serial.print("Received float value: ");
      Serial.println(receivedFloatValue, 2); // Print the reconstructed float value
      lcd.clear();
      lcd.setCursor(0, 0); // Set cursor to position (0, 0)
      lcd.print("Tds Value"); // Display pH header on the LCD
      lcd.setCursor(0, 1); // Set cursor to position (0, 1)
      lcd.print(receivedFloatValue); // Display pH information on the LCD
      }
  }
}


// Function to handle outgoing I2C data
void requestEvent() {
  if (relayMessageFlag) {
    // Send relay message
    // relayMessage(receivedMessageType, receivedMessageType, receivedData);
    Wire.write(receivedMessageType);
    Serial.print("Here is the message being delivered :");
    Serial.println(receivedMessageType);
    Wire.write(receivedMessageLength);
    uint8_t byteArray[sizeof(float)];
    memcpy(byteArray, &receivedFloatValue, sizeof(float));
    for (int i = 0; i < sizeof(float); i++) {
      Wire.write(byteArray[i]);
    //for (int i = 0; i < receivedMessageLength; i++) {
    //  Wire.write(receivedData[i]);
    }
    // Reset relay message flag
    relayMessageFlag = false; 
  } 
  /*
  else {
    // Send pH transmission message
    Wire.write(messageType);
    Serial.print("Here is the message being delivered :");
    Serial.println(messageType);
    Wire.write(messageLength);
    uint8_t byteArray[sizeof(float)];
    memcpy(byteArray, &pHValue_global, sizeof(float));
    for (int i = 0; i < sizeof(float); i++) {
      Wire.write(byteArray[i]);
    }
  }
  */
}

/*
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
*/
