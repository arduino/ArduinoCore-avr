# SoftwareSerial library

## Methods

### `SoftwareSerial()`

Create an instance of a SoftwareSerial object. Multiple SoftwareSerial objects may be created, however only one can be active at a given moment.

#### Syntax 

```
SoftwareSerial(rxPin, txPin, inverse_logic)
```

#### Parameters

* _rxPin_: the pin on which to receive serial data.
* _txPin_: the pin on which to transmit serial data.
* _inverse_logic_: used to invert the sense of incoming bits (the default is normal logic). If set, SoftwareSerial treats a LOW (0v on the pin, normally) on the RX pin as a 1-bit (the idle state) and a HIGH (5V on the pin, normally) as a 0-bit. It also affects the way that it writes to the TX pin. Default value is false.

#### Returns

None.

#### Example

```arduino
#include <SoftwareSerial.h>

const byte rxPin = 2;
const byte txPin = 3;

// Set up a new SoftwareSerial object
SoftwareSerial mySerial (rxPin, txPin);
```

#### See also

* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `available()`

Get the number of bytes (characters) available for reading from a software serial port. This is data that has already arrived and stored in the serial receive buffer.

#### Syntax 

```
mySerial.available()
```

#### Parameters

None.

#### Returns

The number of bytes available to read.

#### Example

```arduino
#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

void setup()  {
    // Define pin modes for TX and RX
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    
    // Set the baud rate for the SoftwareSerial object
    mySerial.begin(9600);
}

void loop() {
    if (mySerial.available() > 0) {
        mySerial.read();
    }
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `begin()`

Sets the speed (baud rate) for the serial communication. Supported baud rates are: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, and 115200 bauds.

#### Syntax 

```
mySerial.begin(speed)
```

#### Parameters

* _speed_: the desired baud rate (long). Supported baud rates are: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, and 115200 bauds.

#### Returns

None.

#### Example

```arduino
#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

void setup()  {
    // Define pin modes for TX and RX
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);

    // Set the baud rate for the SoftwareSerial object
    mySerial.begin(9600);
}

void loop() {
    // ...
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `isListening()`

Tests to see if requested software serial object is actively listening.

#### Syntax 

```
mySerial.isListening()
```

#### Parameters

None.

#### Returns

Boolean.

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial portOne(10, 11);

void setup() {
    // Set the baud rate for the Serial port
    Serial.begin(9600);

    // Set the baud rate for the SerialSoftware object
    portOne.begin(9600);
}

void loop() {
    if (portOne.isListening()) { 
        Serial.println("portOne is listening!");
    }

    // ...
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `overflow()`

Tests to see if a SoftwareSerial buffer overflow has occurred. Calling this function clears the overflow flag, meaning that subsequent calls will return false unless another byte of data has been received and discarded in the meantime. The SoftwareSerial buffer can hold up to 64 bytes.

#### Syntax 

```
mySerial.overflow()
```

#### Parameters

None.

#### Returns

Boolean.

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial portOne(10, 11);

void setup() {
    // Set the baud rate for the Serial port
    Serial.begin(9600);

    // Set the baud rate for the SerialSoftware object
    portOne.begin(9600);
}

void loop() {
    if (portOne.overflow()) {
        Serial.println("portOne overflow!");
    }

    // ...
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `peek()`

Return a character that was received on the RX pin of the software serial port. Unlike read(), however, subsequent calls to this function will return the same character. Note that only one SoftwareSerial object can receive incoming data at a time (select which one with the listen() function).

#### Syntax 

```
mySerial.peek()
```

#### Parameters

None.

#### Returns

The character read or -1 if none is available.

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial mySerial(10, 11);

void setup() {
    // Set the baud rate for the SerialSoftware object
    mySerial.begin(9600);
}

void loop() {
    char c = mySerial.peek();
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `read()`

Return a character that was received on the RX pin of the SoftwareSerial objecto. Note that only one SoftwareSerial object can receive incoming data at a time (select which one with the listen() function).

#### Syntax 

```
mySerial.read()
```

#### Parameters

None.

#### Returns

The character read or -1 if none is available.

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial mySerial(10, 11);

void setup() {
    // Set the baud rate for the SerialSoftware object
    mySerial.begin(9600);
}

void loop() {
    char c = mySerial.read();
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `print()`

Prints data to the transmit pin of the SoftwareSerial object. Works the same as the Serial.print() function.

#### Syntax 

```
mySerial.print(val)
```

#### Parameters

* _val_: the value to print.

#### Returns

The number of bytes written (reading this number is optional).

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial mySerial(10, 11);

int analogValue;

void setup() {
    // Set the baud rate for the SerialSoftware object
    mySerial.begin(9600);
}

void loop() {
    // Read the analog value on pin A0
    analogValue = analogRead(A0);

    // Print analogValue in the Serial Monitor in many formats:
    mySerial.print(analogValue);         // Print as an ASCII-encoded decimal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, DEC);    // Print as an ASCII-encoded decimal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, HEX);    // Print as an ASCII-encoded hexadecimal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, OCT);    // Print as an ASCII-encoded octal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, BIN);    // Print as an ASCII-encoded binary
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue/4, BYTE); // Print as a raw byte value (divide the
                                         // value in 4 because analogRead() function returns numbers
                                         // from 0 to 1023, but a byte can only hold values up to 255)

    mySerial.print("\t");                // Print a tab character    
    mySerial.println();                  // Print a line feed character

    // Pause for 10 milliseconds before the next reading
    delay(10);
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)
* [write()](#write)

### `println()`

Prints data to the transmit pin of the SoftwareSerial object followed by a carriage return and line feed. Works the same as the Serial.println() function.

#### Syntax 

```
mySerial.println(val)
```

#### Parameters

* _val_: the value to print.

#### Returns

The number of bytes written (reading this number is optional).

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial mySerial(10, 11);

int analogValue;

void setup() {
    // Set the baud rate for the SerialSoftware object
    mySerial.begin(9600);
}

void loop() {
    // Read the analog value on pin A0
    analogValue = analogRead(A0);

    // Print analogValue in the Serial Monitor in many formats:
    mySerial.print(analogValue);         // Print as an ASCII-encoded decimal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, DEC);    // Print as an ASCII-encoded decimal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, HEX);    // Print as an ASCII-encoded hexadecimal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, OCT);    // Print as an ASCII-encoded octal
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue, BIN);    // Print as an ASCII-encoded binary
    mySerial.print("\t");                // Print a tab character
    mySerial.print(analogValue/4, BYTE); // Print as a raw byte value (divide the
                                         // value in 4 because analogRead() function returns numbers
                                         // from 0 to 1023, but a byte can only hold values up to 255)

    mySerial.print("\t");                // Print a tab character    
    mySerial.println();                  // Print a line feed character

    // Pause for 10 milliseconds before the next reading
    delay(10);
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [listen()](#listen)
* [write()](#write)

### `listen()`

Enables the selected SoftwareSerial object to listen. Only one SoftwareSerial object can listen at a time; data that arrives for other ports will be discarded. Any data already received is discarded during the call to listen() function (unless the given instance is already listening).

#### Syntax 

```
mySerial.listen()
```

#### Parameters

None.

#### Returns

Returns true if it replaces another.

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial portOne(10, 11);

// Set up a new SoftwareSerial object with RX in digital pin 8 and TX in digital pin 9
SoftwareSerial portTwo(8, 9);

void setup() {
    // Set the baud rate for the Serial object
    Serial.begin(9600);

    // Set the baud rate for the SerialSoftware objects
    portOne.begin(9600);
    portTwo.begin(9600);
}

void loop() {
    // Enable SoftwareSerial object to listen
    portOne.listen();
    
    if (portOne.isListening()) {
        Serial.println("portOne is listening!");
    } else {
        Serial.println("portOne is not listening!");
    }

    if (portTwo.isListening()) {
        Serial.println("portTwo is listening!");
    } else {
        Serial.println("portTwo is not listening!");
    }
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [write()](#write)

### `write()`

Prints data to the transmit pin of the SoftwareSerial object as raw bytes. Works the same as the Serial.write()function.

#### Syntax 

```
mySerial.write(val)
```

#### Parameters

* _val_: the binary value to print.

#### Returns

The number of bytes written (reading this number is optional).

#### Example

```arduino
#include <SoftwareSerial.h>

// Set up a new SoftwareSerial object with RX in digital pin 10 and TX in digital pin 11
SoftwareSerial mySerial(10, 11);

void setup() {
    // Set the baud rate for the SerialSoftware object
    mySerial.begin(9600);
}

void loop() {
    // Send a byte with the value 45
    mySerial.write(45);

    //Send the string “hello” and return the length of the string.
    int bytesSent = mySerial.write(“hello”);
}
```

#### See also

* [SoftwareSerial()](#softwareserial)
* [available()](#available)
* [begin()](#begin)
* [isListening()](#islistening)
* [overflow()](#overflow)
* [peek()](#peek)
* [read()](#read)
* [print()](#print)
* [println()](#println)
* [listen()](#listen)