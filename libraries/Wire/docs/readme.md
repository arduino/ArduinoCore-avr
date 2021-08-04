# Wire library

This library allows you to communicate with I2C/TWI devices. On the Arduino boards with the R3 layout (1.0 pinout), the SDA (data line) and SCL (clock line) are on the pin headers close to the AREF pin. The Arduino Due has two I2C/TWI interfaces SDA1 and SCL1 are near to the AREF pin and the additional one is on pins 20 and 21.

As a reference the table below shows where TWI pins are located on various Arduino boards.

|    Board    |         I2C/TWI pins         |
|-------------|------------------------------|
|UNO, Ethernet|A4 (SDA), A5 (SCL)            |
|Mega2560     |20 (SDA), 21 (SCL)            |
|Leonardo     |2 (SDA), 3 (SCL)              |
|Due          |20 (SDA), 21 (SCL), SDA1, SCL1|

As of Arduino 1.0, the library inherits from the Stream functions, making it consistent with other read/write libraries. Because of this, send() and receive() have been replaced with read() and write().

**Note:** There are both 7 and 8-bit versions of I2C addresses. 7 bits identify the device, and the eighth bit determines if it's being written to or read from. The Wire library uses 7 bit addresses throughout. If you have a datasheet or sample code that uses 8-bit address, you'll want to drop the low bit (i.e. shift the value one bit to the right), yielding an address between 0 and 127. However the addresses from 0 to 7 are not used because are reserved so the first address that can be used is 8. Please note that a pull-up resistor is needed when connecting SDA/SCL pins. Please refer to the examples for more information. MEGA 2560 board has pull-up resistors on pins 20 and 21 onboard. 

**The Wire library implementation uses a 32 byte buffer, therefore any communication should be within this limit. Exceeding bytes in a single transmission will just be dropped.**

To use this library:

```
#include <Wire.h>
```

## Examples

* [Digital Potentiometer](https://www.arduino.cc/en/Tutorial/LibraryExamples/DigitalPotentiometer): Control an Analog Devices AD5171 digital potentiometer.
* [Master Reader/Slave Writer](https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterReader): Program two Arduino boards to communicate with one another in a Master Reader/Slave Sender configuration via the I2C.
* [Master Writer/Slave receive](https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterWriter)r:Program two Arduino boards to communicate with one another in a Master Writer/Slave Receiver configuration via the I2C.
* [SFR Ranger Reader](https://www.arduino.cc/en/Tutorial/LibraryExamples/SFRRangerReader): Read an ultra-sonic range finder interfaced via the I2C.
* [Add SERCOM](https://www.arduino.cc/en/Tutorial/SamdSercom): Adding mores Serial interfaces to SAMD microcontrollers.