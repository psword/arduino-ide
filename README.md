# arduino-ide
 Various code snips for MCUs. This includes short sketches for:

 - Arduino R3 UNO
 - Arduino R4 UNO
 - Huzzah Feather ESP8266
 - FireBeetle ESP8266
 - ESP32 Dev Boards
------------------------------------------------------------------------
This is primarily a learning repository. However, it also contains work related to a university learning project. See [Water Quality Measuring Project](https://github.com/psword/arduino-ide/tree/de442c8f53f811c71cad42c74612d5cd95f93f08/Water%20Quality%20Measuring%20Project).

[Version 1](https://github.com/psword/arduino-ide/tree/33ed06192bf1e0ca8ec16ae9a3d8f0646361ff46/Water%20Quality%20Measuring%20Project) prototype code allows:

 - I2C communication between 4 MCUs
 - [Arduino R3](https://store.arduino.cc/en-fi/products/arduino-uno-rev3?srsltid=AfmBOoqjvXa_Z3smDLSFfSScGgidhrbx-66E7KziQySzwaVlbBPr1Xnl), [Huzzah Feather 8266](https://www.adafruit.com/product/2821), and [Firebeetle ESP8266](https://www.dfrobot.com/product-1634.html)
 - Multi-sensor parameter measurement unified into one system (for water)
 - Code to connect to thinger.io and deliver basic data
 - Code designed in Arduino IDE

There are several operational issues with the Version 1 design, and it is suitable for lab only. ***Version 1 was designed as part of a HAMK University Program module on learning microcontrollers.*** While it may have the intended functionality, the real purpose was to learn MCU functions, operations, basic C++, and the I2C protocol. However, the objectives can easily be reduced into a smaller, contained system.

[Version 2](https://github.com/psword/arduino-ide/tree/a27fc44e874fe428e2d5003f90bf01ba792a1445/Water%20Quality%20Measuring%20Project/version%202) prototype code allows:

- I2C communication between 2 MCUs
- Initially [ESP32 Dev Module](https://www.elecrow.com/esp32-wifi-ble-board.html), [LilyGo SIM808](https://www.lilygo.cc/products/t-call-v1-4?variant=42868250869941)
- Multi-sensor parameter measurement unified into one system (for water)
- Code to connect to thinger.io and deliver basic data
- Code designed in Visual Studio Code using [Platform IO](https://github.com/platformio)

*Version 2 is in design, and will be the first implementation to be used in an underwater environment.*

Note: I use Wi-Fi for initial testing. However, once the operational design matures, it will naturally transition to low-power communications.


Further notes, documents, and ideated designs see [here](https://github.com/psword/arduino-ide/tree/a27fc44e874fe428e2d5003f90bf01ba792a1445/Water%20Quality%20Measuring%20Project).



