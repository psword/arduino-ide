#include "UptimeCalculator.h" // Include the header file for the UptimeCalculator class

// Create an instance of the UptimeCalculator class
UptimeCalculator uptime;

void setup() {
  // Intialize Serial
  Serial.begin(9600);

}

void loop() {
  // Update uptime
  uptime.updateUptime();

  // Get uptime individual values
  unsigned long hours = uptime.getHours();
  unsigned long minutes = uptime.getMinutes();
  unsigned long seconds = uptime.getSeconds();

  // Let's print the uptime values we received
  Serial.println("Here is our uptime: ");
  Serial.println("Hours: ");
  Serial.println(hours);
  Serial.println("Minutes: ");
  Serial.println(minutes);
  Serial.println("Seconds: ");
  Serial.println(seconds);

  // Short delay
  delay(2000);

}
