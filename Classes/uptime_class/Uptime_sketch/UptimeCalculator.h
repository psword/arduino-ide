class UptimeCalculator {
  public:
    // Constructor
    UptimeCalculator() {
      // Initialize variables
      hours = 0;
      minutes = 0;
      seconds = 0;
    }

    // Function to update uptime
    void updateUptime() {
      unsigned long uptimeMillis = millis(); // Get uptime in milliseconds
      hours = uptimeMillis / (1000UL * 60UL * 60UL); // Calculate hours
      uptimeMillis %= (1000UL * 60UL * 60UL);
      minutes = uptimeMillis / (1000UL * 60UL); // Calculate minutes
      uptimeMillis %= (1000UL * 60UL);
      seconds = uptimeMillis / 1000UL; // Calculate seconds
    }

    // Function to get hours
    unsigned long getHours() {
      return hours;
    }

    // Function to get minutes
    unsigned long getMinutes() {
      return minutes;
    }

    // Function to get seconds
    unsigned long getSeconds() {
      return seconds;
    }

  private:
    unsigned long hours;
    unsigned long minutes;
    unsigned long seconds;
};
