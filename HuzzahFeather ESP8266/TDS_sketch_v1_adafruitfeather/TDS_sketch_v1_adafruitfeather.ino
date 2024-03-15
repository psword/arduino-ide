#define SERIAL Serial
#define sensorPin A0
 
int sensorValue = 0;
float tdsValue = 0;
float Voltage = 0;
 
void setup() {
    SERIAL.begin(9600);
}
void loop() {
    sensorValue = analogRead(sensorPin);
    SERIAL.print(sensorValue);
    SERIAL.println();
    // Voltage = sensorValue*5/1024.0; //Convert analog reading to Voltage (10-bits for Arduino 5V)
    // Voltage = sensorValue*3.3/1024.0; //Convert analog reading to Voltage (10-bits for ESP8266 3.3V)
    // tdsValue=(133.42/Voltage*Voltage*Voltage - 255.86*Voltage*Voltage + 857.39*Voltage)*0.5; //Convert voltage value to TDS value
    // SERIAL.print("TDS Value = ");
    // SERIAL.print(tdsValue);
    // SERIAL.println(" ppm");
    delay(1000);
}