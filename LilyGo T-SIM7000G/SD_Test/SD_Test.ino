// Defined for LilyGo TSIM7000G

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "debug.h"
#include "uRTCLib.h"

#define SD_MISO     2
#define SD_MOSI     15
#define SD_SCLK     14
#define SD_CS       13

#define SerialMon Serial

char daysOfTheWeek[7][12] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
float sensorData = 123.45;
// Define the filename
const char* filename = "/logs/sensor_log.txt";
  
// Define the directory
const char* directory = "/logs";

// uRTCLib rtc;
uRTCLib rtc(0x68);

// URTCLIB_MODEL_DS3232
byte rtcModel = URTCLIB_MODEL_DS3231;

void logData(fs::FS &fs,String filename, const char *data) {
  File file = fs.open(filename, FILE_APPEND);
  if (file) {
    file.print(data);
    file.close();
    DEBUG_PRINT("Data Written: ");
    DEBUG_PRINTLN(data);
  } else {
    DEBUG_PRINTLN("Failed to open file for writing!");
  }
}

void createDir(fs::FS &fs, const char *path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    DEBUG_PRINTLN("Dir created");
  } else {
    DEBUG_PRINTLN("mkdir failed");
  }
}

// Function to log data with timestamp
void logDataWithTimestamp(fs::FS &fs, String filename, uRTCLib rtc) {
    
    char logEntry[128];  // Buffer to store formatted log string

    rtc.refresh();

    // Format: YYYY/MM/DD Day HH:MM:SS
    sprintf(logEntry, "%02d/%02d/%02d %s %02d:%02d:%02d; Sensor Junk Data: %.2f\n",
            rtc.year(), rtc.month(), rtc.day(),
            daysOfTheWeek[rtc.dayOfWeek() - 1],  // Adjust day index (1-7 → 0-6)
            rtc.hour(), rtc.minute(), rtc.second(), sensorData);

    // Write the formatted log entry to the file
    logData(fs, filename, logEntry);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {;}
  
  Wire.begin();  // Initialize I2C

  // Configure RTC
  URTCLIB_WIRE.begin();
  rtc.set_model(rtcModel);
  rtc.refresh();
  delay(5000);

  // Set the RTC time (should be initial setup only)
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
  // rtc.set(0, 22, 13, 3, 12, 3, 25);

  // RTC Verify
  char timeStamp[128]; // Buffer to store timeStamp test
  sprintf(timeStamp, "%02d/%02d/%02d %s %02d:%02d:%02d",
      rtc.year(), rtc.month(), rtc.day(),
      daysOfTheWeek[rtc.dayOfWeek() - 1],  // Adjust day index (1-7 → 0-6)
      rtc.hour(), rtc.minute(), rtc.second());
  DEBUG_PRINTLN("========TimeStamp Print.=======");
  DEBUG_PRINTLN(timeStamp);
  DEBUG_PRINTLN("===========================");
  // Initialize SD Card
  DEBUG_PRINTLN("========SDCard Detect.======");
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS)) {
      DEBUG_PRINTLN("SDCard MOUNT FAIL");
      return;
  } else {
      uint32_t cardSize = SD.cardSize() / (1024 * 1024);
      String str = "SDCard Size: " + String(cardSize) + "MB";
      DEBUG_PRINTLN(str);
      DEBUG_PRINTLN("SDCard Mounted Successfully");
  }
  DEBUG_PRINTLN("===========================");

  // Create directory if it doesn't exist
  createDir(SD, directory);

  // Append sample data
  logDataWithTimestamp(SD, filename, rtc);
}

void loop() {
  rtc.refresh();

  DEBUG_PRINT("Current Date & Time: ");
  DEBUG_PRINT(rtc.year());
  DEBUG_PRINT('/');
  DEBUG_PRINT(rtc.month());
  DEBUG_PRINT('/');
  DEBUG_PRINT(rtc.day());

  DEBUG_PRINT(" (");
  DEBUG_PRINT(daysOfTheWeek[rtc.dayOfWeek()-1]);
  DEBUG_PRINT(") ");

  DEBUG_PRINT(rtc.hour());
  DEBUG_PRINT(':');
  DEBUG_PRINT(rtc.minute());
  DEBUG_PRINT(':');
  DEBUG_PRINTLN(rtc.second());

  DEBUG_PRINT("Temperature: ");
  DEBUG_PRINT(rtc.temp()  / 100);
  DEBUG_PRINT("\xC2\xB0");   //shows degrees character
  DEBUG_PRINTLN("C");

  DEBUG_PRINTLN();
  delay(10000);
  DEBUG_PRINTLN("========TimeStamp Print.=======");
  logDataWithTimestamp(SD, filename, rtc);
  DEBUG_PRINTLN("===========================");
}
