// Data Wire is plugged into ADC Port
#define sensor_Input_Pin A0

// Power Port is GPIO PIN 15
#define GPIO_PIN 15

// float multiplier = 0.448; // Voltage Multiplier
int sensorValue = 0;
float tdsValue = 0;
float Voltage = 0;

// Begin Setup
void setup() {
    Serial.begin(9600);
    pinMode(GPIO_PIN, OUTPUT); // Set GPIO PIN as OUTPUT
}

void loop() {
    digitalWrite(GPIO_PIN, HIGH); // Power the sensor
    delay(5000);
    sensorValue = analogRead(sensor_Input_Pin);
    delay(1000);
    Voltage = sensorValue*3.3/1024.0; //Convert analog reading to Voltage (10-bits for ESP8266 3.3V)
    // Serial.print("Divided Voltage on the PIN = ");
    // Serial.print(Voltage);
    // Serial.println(" volts");
    // Voltage = Voltage/multiplier;
    // Serial.print("Multiplied Voltage on the PIN = ");
    // Serial.print(Voltage);
    //Serial.println(" volts");
    // delay(1000);
    tdsValue=(133.42/Voltage*Voltage*Voltage - 255.86*Voltage*Voltage + 857.39*Voltage)*0.5; //Convert voltage value to TDS value
    delay(5000);
    digitalWrite(GPIO_PIN, LOW); // Power off the sensor
    Serial.print("TDS Value = ");
    Serial.print(tdsValue);
    Serial.println(" ppm");
    delay(5000);
}