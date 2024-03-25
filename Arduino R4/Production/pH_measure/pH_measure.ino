// Pin Definitions
#define SensorPin A0            // pH meter Analog output to Arduino Analog Input 0
#define POWER_PIN 8             // Define digital pin 8 as the power pin for the pH Sensor
#define LED 13                  // Pin for LED indicator

// Thinger.io and Wifi
#define _DISABLE_TLS_
#define THINGER_SERIAL_DEBUG // more details at http://docs.thinger.io/arduino/
#define THINGER_USE_STATIC_MEMORY
#define THINGER_STATIC_MEMORY_SIZE 512
#include <WiFiS3.h>
#include <ThingerWifi.h>
#include "arduino_secrets.h"

// Constants
#define Offset 0.37             // Deviation compensation for pH sensor
#define ArrayLength  40         // Length of array for averaging pH values
#define MAX_DATA_LENGTH 8      // Maximum length of received data, adjust as needed

// Secrets File
#include "arduino_secrets.h"

// Global Variables
int pHArray[ArrayLength];               // Array to store pH values
int pHArrayIndex = 0;                    // Index for pH array
float pHValue_global = 0;                // Global pH value
bool codeExecuted = false;               // Flag for code execution
int status = WL_IDLE_STATUS;
ThingerWifi thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup() {
  // put your setup code here, to run once:
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

  // configure wifi network
  thing.add_wifi(ssid, ssid_pass);

    // Define Thinger Resources
  thing["1001"] >> [](pson& out){
    out["pH"] = pHValue_global;
  };

  // First code execution to normalize pH level
  if (!codeExecuted) {
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
    digitalWrite(POWER_PIN, LOW);
    codeExecuted = true;
  }
 }
  // put your main code here, to run repeatedly:
void loop() {
      delay(60000);
      int pHsenseIterations = 10;
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
      // Thinger handler
      thing.handle();
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
