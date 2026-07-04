# SoftwareSerial library

The SoftwareSerial library allows serial communication on other digital pins of an Arduino board, using software to replicate the functionality (hence the name "SoftwareSerial"). It is possible to have multiple software serial ports with speeds up to 115200 bps. A parameter enables inverted signaling for devices which require that protocol.

The version of SoftwareSerial included in 1.0 and later is based on the [NewSoftSerial library](http://arduiniana.org/libraries/newsoftserial/) by Mikal Hart.

To use this library:

```
#include <SoftwareSerial.h>
```

## Limitations of this library

SoftwareSerial library has the following known limitations:

* If using multiple software serial ports, only one can receive data at a time.
* Not all pins on the Mega and Mega 2560 boards support change interrupts, so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).
Not all pins on the Leonardo and Micro boards support change interrupts, so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
* On Arduino or Genuino 101 boards the current maximum RX speed is 57600bps. 
* On Arduino or Genuino 101 boards RX doesn't work on digital pin 13.

If your project requires simultaneous data flows, see Paul Stoffregen's [AltSoftSerial library](https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html).

## Examples

* [SoftwareSerial example](https://www.arduino.cc/en/Tutorial/LibraryExamples/SoftwareSerialExample): sometimes one serial port just isn't enough!
* [Two port receive](https://www.arduino.cc/en/Tutorial/LibraryExamples/TwoPortReceive): Work with multiple software serial ports.