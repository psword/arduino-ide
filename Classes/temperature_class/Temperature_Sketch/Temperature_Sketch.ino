#define ONE_WIRE_BUS 2 // Example pin for OneWire bus

// Create an instance of TemperatureSensor
TemperatureSensor tempSensor(ONE_WIRE_BUS);

void setup() {
    Serial.begin(9600); // Initialize serial communication
}

void loop() {
    // Read and adjust temperature
    float adjustedTemp = tempSensor.readAndAdjustTemp();

    // Print adjusted temperature
    Serial.print("Adjusted Temperature: ");
    Serial.println(adjustedTemp);

    delay(1000); // Wait for 1 second
}
