These files has been tested. It should be used with a sensor that is compatible with 3.3V logic.

# Update 1

This implementation will support a Grove TDS sensor, DFRobot pH version 2.0 sensor, and a onewire temperature sensor.
The code assumes the presence of a MOSFET to pass 5.0V through to the pH sensor, since it requires 5.0V to operate. It is written for
an ESP32 DEV board, but can easily be adapted to a different platform.