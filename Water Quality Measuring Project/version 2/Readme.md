# Version 2
## Update to project

Since version 1 was a lab-based learning concept, proceeding to version 2 was about condensing the electronics and creating efficiency. Also, experimenting with successful and failed solutions.

This folder contains the code to convert the design to an ESP32-based architecture. I am not a programmer by trade, nor have I been working in the "coding industry" - I am a network engineer/sysadmin by training, currently a student (back to school) in a unique bioeconomy/IoT program. Adapting that knowledge has been quite entertaining.

## Notes

- For Arduino IDE, choose ESP32 Dev Module
(this was originally built in PlatformIO)

## Version 2.0

- 2 Modules, sensing and comm
- ESP32_sensing_v2.0 is the sensing portion of the project
- ESP32_comm_v2.0 is the comms portion of the project

### Desired features

- Considerations for modularity/adaptability and future sensor additions
- Considerations for power consumption and battery life (sleep functions, etc.)
- Considerations for data transmission (WiFi, Bluetooth, LoRa, etc)
- Considerations for data storage (SD card, cloud, etc)
- Reliable inter-device communication and simple error-handling/reporting features