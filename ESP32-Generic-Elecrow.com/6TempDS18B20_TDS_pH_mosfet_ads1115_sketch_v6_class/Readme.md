# Readme Notes regarding this sketch

We are using an esp32 with 3.3V logic. The pH sensor in use is a DFRobot v1.1 sensor with 5V logic, with signal values higher than the pins can handle on the MCU. Attempts to use a MOSFET to control power supply, and and ADS1115 to convert the analog reading to a digital signal so it can be level shifted, result in this:


ESP Exception Decoder
Sketch: TempDS18B20_TDS_pH_sketch_v6_class FQBN: esp32:esp32:esp32

A6      : 0x00000000  A7      : 0x00000000  A8      : 0x800d3a4c  A9      : 0x3ffb21a0  
A10     : 0x3ffbdbcc  A11     : 0x3f4054cf  A12     : 0x00000002  A13     : 0xffffffff  
A14     : 0x3ffb833c  A15     : 0x00000000  SAR     : 0x0000000a  EXCCAUSE: 0x0000001c  
EXCVADDR: 0x0000000c  LBEG    : 0x40086851  LEND    : 0x40086861  LCOUNT  : 0xffffffff  


Backtrace: 0x400d36d0:0x3ffb21c0 0x400d35d9:0x3ffb21e0 0x400d360b:0x3ffb2200 0x400d36ba:0x3ffb2220 0x400d19f8:0x3ffb2240 0x400d20c3:0x3ffb2270 0x400d6bd5:0x3ffb2290




ELF file SHA256: 481bc96a4dd10c00


EXCVADDR: 0x0000000c

Decoding stack results
- 0x400d36d0:  is in Adafruit_I2CDevice::write(unsigned char const*, unsigned int, bool, unsigned char const*, unsigned int) (c:\Users\phili\Documents\GitHub\arduino-ide\libraries\Adafruit_BusIO\Adafruit_I2CDevice.cpp:101).
- 0x400d35d9:  is in Adafruit_ADS1X15::writeRegister(unsigned char, unsigned short) (c:\Users\phili\Documents\GitHub\arduino-ide\libraries\Adafruit_ADS1X15\Adafruit_ADS1X15.cpp:386).
- 0x400d360b:  is in Adafruit_ADS1X15::startADCReading(unsigned short, bool) (c:\Users\phili\Documents\GitHub\arduino-ide\libraries\Adafruit_ADS1X15\Adafruit_ADS1X15.cpp:356).
- 0x400d36ba:  is in Adafruit_ADS1X15::readADC_SingleEnded(unsigned char) (c:\Users\phili\Documents\GitHub\arduino-ide\libraries\Adafruit_ADS1X15\Adafruit_ADS1X15.cpp:120).
- 0x400d19f8: pHSensorStateMachine() at C:\Users\phili\Documents\GitHub\arduino-ide\ESP32-Generic-Elecrow.com\TempDS18B20_TDS_pH_sketch_v6_class\AnalogpH.h:48
- 0x400d20c3: loop() at C:\Users\phili\Documents\GitHub\arduino-ide\ESP32-Generic-Elecrow.com\TempDS18B20_TDS_pH_sketch_v6_class\TempDS18B20_TDS_pH_sketch_v6_class.ino:123
- 0x400d6bd5: loopTask(void*) at C:\Users\phili\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.16\cores\esp32\main.cpp:50




### At this point, it is unresolved.