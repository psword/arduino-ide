// uptime_calculator.h
// currently experimental - needs adapted and further tested
#ifndef uptimeCalc_h
#define uptimeCalc_h

#include <Arduino.h>

// Define the RTC_DATA_ATTR struct outside of the class
RTC_DATA_ATTR struct RtcData {
  unsigned long hours = 0;
  unsigned long minutes = 0;
  unsigned long seconds = 0;
  unsigned long previousMillis = 0;
  bool isFirstBoot = true;  // Track if this is the first boot
} rtcData;

class UptimeCalculator {
  public:
    // Constructor
    UptimeCalculator() {
      // Constructor left empty since rtcData persistence is handled by RTC_DATA_ATTR
    }

    // Function to update uptime
    void updateUptime() {
      unsigned long currentMillis = millis(); // Get current uptime in milliseconds
      unsigned long elapsedMillis;

      // Check if this is the first boot after a cold start
      if (rtcData.isFirstBoot) {
        rtcData.previousMillis = currentMillis;  // Initialize previousMillis
        rtcData.isFirstBoot = false;  // Mark as no longer the first boot
        Serial.println("First boot, initializing previousMillis.");
      } else {
        // Handle regular operation
        if (currentMillis >= rtcData.previousMillis) {
          elapsedMillis = currentMillis - rtcData.previousMillis;
        } else {
          // Handle the case where currentMillis is less due to overflow
          elapsedMillis = (ULONG_MAX - rtcData.previousMillis + 1) + currentMillis;
        }

        Serial.print("Current Millis: ");
        Serial.println(currentMillis);
        Serial.print("Previous Millis: ");
        Serial.println(rtcData.previousMillis);
        Serial.print("Elapsed Millis: ");
        Serial.println(elapsedMillis);

        rtcData.seconds += elapsedMillis / 1000; // Add elapsed seconds
        normalizeTime(); // Normalize time to ensure minutes and seconds are within correct range

        Serial.print("Updated Hours: ");
        Serial.println(rtcData.hours);
        Serial.print("Updated Minutes: ");
        Serial.println(rtcData.minutes);
        Serial.print("Updated Seconds: ");
        Serial.println(rtcData.seconds);
      }

      rtcData.previousMillis = currentMillis; // Update previousMillis for next wake-up
    }

    // Function to set hours
    void setHours(unsigned long hours) {      
      rtcData.hours = hours;
    }
    // Function to get hours
    unsigned long getHours() {
      return rtcData.hours;
    }

    // Function to set minutes
    void setMinutes(unsigned long minutes) {      
      rtcData.minutes = minutes;
    }
    // Function to get minutes
    unsigned long getMinutes() {
      return rtcData.minutes;
    }

    // Function to set seconds
    void setSeconds(unsigned long seconds) {      
      rtcData.seconds = seconds;
    }
    // Function to get seconds
    unsigned long getSeconds() {
      return rtcData.seconds;
    }

  private:
    // Function to normalize time
    void normalizeTime() {
      rtcData.minutes += rtcData.seconds / 60;
      rtcData.seconds %= 60;
      rtcData.hours += rtcData.minutes / 60;
      rtcData.minutes %= 60;
    }
};

#endif // uptimeCalc_h
