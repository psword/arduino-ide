#define THINGER_SERIAL_DEBUG
#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define _DISABLE_TLS_

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ThingerESP8266.h>
#include "arduino_secrets.h"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // open serial for monitoring
  Serial.begin(115200);

  // add WiFi credentials
  thing.add_wifi(ssid, ssid_pass);

  // initialize sensor
  dht.begin();

  // Define Thinger Resources
  thing["ESP8266_dht22"] >> [](pson& out){
    out["temperature"] = dht.readTemperature();
    out["humidity"] = dht.readHumidity();
  };

  // more details at http://docs.thinger.io/arduino/
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