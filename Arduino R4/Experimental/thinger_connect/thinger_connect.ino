#define THINGER_SERIAL_DEBUG
#define THINGER_USE_STATIC_MEMORY
#define THINGER_STATIC_MEMORY_SIZE 512
//#define DHTPIN 2     // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define _DISABLE_TLS_

#include <WiFiS3.h>
#include <ThingerWifi.h>
#include "arduino_secrets.h"
#include <Wire.h>               // Library for Wire transmission

int status = WL_IDLE_STATUS;
ThingerWifi thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // String fv = WiFi.firmwareVersion();
  // if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
  //  Serial.println("Please upgrade the firmware");
  // }
  
  // configure wifi network
  thing.add_wifi(ssid, ssid_pass);
  
  // initialize sensor
  dht.begin();

  // Define Thinger Resources
  thing["Arduino_dht22"] >> [](pson& out){
    out["temperature"] = dht.readTemperature();
    out["humidity"] = dht.readHumidity();
  };

}

void loop() {
  
  // insert delay
  delay(2000);
  
  // Thinger handler
  thing.handle();
  
  // Begin Sensor Code Operation
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  printWifiStatus();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void receiveEvent(int bytes) {
  while (Wire.available()) { // Loop through all received bytes
    char c = Wire.read(); // Read byte as a character
    Serial.print(c); // Print the received character to serial monitor
  }
  Serial.println(); // Print a newline character after receiving all bytes
}