These files has been tested. It should be used with a sensor that is compatible with 3.3V logic.

# Update 1 (version 5)

- This implementation will support a Grove TDS sensor, DFRobot pH version 2.0 sensor, and a onewire temperature sensor.
The code assumes the presence of a MOSFET to pass 5.0V through to the pH sensor, since it requires 5.0V to operate. It is written for
an ESP32 DEV board, but can easily be adapted to a different platform.

# Update 1.1 (version 6)

- Attempting to use an ADS1115 results in a crash dump. Until this is resolved, I cannot use the ADS1115. At that point, I will most likely
integrate it for all sensors.

# Update 2 (version 7)

- Version 5 assumes a mosfet for operation, and this sketch (version 7) does not. The pH sensor operates on 3.3V logic, and can safely deliver measurements
to the board without level-shifting. This sketch attempts to implement a calibration feature into the setup portion of the code, but it does function
correctly when attempting to save the calibration values to EEPROM.