/**************************************************************
 *
 * For this example, you need to install PubSubClient library:
 *   https://github.com/knolleary/pubsubclient
 *   or from http://librarymanager/all#PubSubClient
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * ArduinoJson
 *   https://arduinojson.org/v7/
 *
 * For more MQTT examples, see PubSubClient library
 *
 **************************************************************/

// Modem Selection
#define TINY_GSM_MODEM_SIM7000

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
#define SerialAT Serial1

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG Serial

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet.
// This is only needed for this example, not in other code.
#define TINY_GSM_USE_GPRS true

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]      = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

// LilyGO T-SIM7000G Pinout
#define UART_BAUD           115200
#define PIN_DTR             25
#define PIN_TX              27
#define PIN_RX              26
#define PWR_PIN             4

// MQTT details
const char* broker = "backend.thinger.io";

const char* topicInit      = "esp32/water1";

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient  mqtt(client);

uint32_t lastReconnectAttempt = 0;
uint32_t lastSendAttempt = 0;

void modemPowerOn(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1500);
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(3000);
  digitalWrite(PWR_PIN, HIGH);
}

void modemRestart(){
  modemPowerOff();
  delay(1000);
  modemPowerOn();
}

void sendMQTT() {
  JsonDocument doc; // Allocate JSON document

  doc["device"] = "esp32";
  doc["status"] = "testing in progress";
  doc["message"] = "Success if you see this";

  char jsonBuffer[256]; // Buffer to hold the JSON string
  serializeJson(doc,jsonBuffer); // Convert the JSON object to string
  Serial.println("Publishing JSON:");
  Serial.println(jsonBuffer); // Debug print

  mqtt.publish(topicInit, jsonBuffer);
}

boolean mqttConnect() {
  Serial.print("Connecting to ");
  Serial.print(broker);

  // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect("esp32-monitor-1", "Betaman6", "lZColKNAmsZO4WLP");

  if (status == false) {
    Serial.println(" fail");
    return false;
  }
  Serial.println(" success");
  sendMQTT();
  return mqtt.connected();
}

bool verifyGPRS() {
  if (!modem.isGprsConnected()) {
    Serial.println("GPRS disconnected! Attempting reconnection...");
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      Serial.println("GPRS connection failed");
      return false;
    }
    Serial.println("GPRS reconnected");
  }
  return true;
}


void setup() {
  // Set console baud rate
  Serial.begin(9600);
  delay(10);

  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

  Serial.println("Wait...");

  // Set GSM module baud rate
  // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.begin(9600);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modemPowerOn();
  // modem.restart();
  // modem.init();

  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }

  // GPRS connection (use creds)
  // modem.gprsConnect(apn, gprsUser, gprsPass);

  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" success");

  if (modem.isNetworkConnected()) { Serial.println("Network connected"); }

  // Let's test a helper function
  if (!verifyGPRS()) {
    delay(10000);
    return;
  }

  String name = modem.getModemName();
  Serial.println("Modem Name: " + name);

  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info: " + modemInfo);

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
}

void loop() {
  
  int counterNetworkRetries = 0;
  const int MAX_RETRIES = 2; 

  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    Serial.println("Network disconnected");
    while (counterNetworkRetries < MAX_RETRIES && !modem.waitForNetwork(180000L, true)) {
      Serial.println("Retrying Network...");
      counterNetworkRetries++;
      return;
    }
    if (modem.isNetworkConnected()) {
      Serial.println("Network reconnection failed after retries. Restarting the modem...");
      modem.restart();
    }

  // Let's test a helper function
    if (!verifyGPRS()) {
      delay(10000);
      return;
    }
  }

  if (!mqtt.connected()) {
    Serial.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) { lastReconnectAttempt = 0; }
    }
    delay(100);
    return;
  }

  unsigned long timer = millis();
  if (timer - lastSendAttempt > 30000L) {
    lastSendAttempt = timer;
    if (!mqtt.connected()) {
      Serial.println("MQTT disconnected. Attempting to reconnect...");
      if (!mqttConnect()) {
        Serial.println("MQTT reconnection failed.");
        return;
      }
      Serial.println("Publishing data...");
      sendMQTT();
    }
  }
}
