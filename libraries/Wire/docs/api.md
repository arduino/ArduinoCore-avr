# Wire library

## Methods

### `begin()`

This function initializes the Wire library and join the I2C bus as a controller or a peripheral. This function should normally be called only once.

#### Syntax

```
Wire.begin()
Wire.begin(address)
```

#### Parameters

* _address_: the 7-bit slave address (optional); if not specified, join the bus as a controller device.

#### Returns

None. 

#### See also

* [requestFrom()](#requestfrom)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `requestFrom()`

This function is used by the controller device to request bytes from a peripheral device. The bytes may then be retrieved with the available() and read() functions. As of Arduino 1.0.1, requestFrom() accepts a boolean argument changing its behavior for compatibility with certain I2C devices. If true, requestFrom() sends a stop message after the request, releasing the I2C bus. If false, requestFrom() sends a restart message after the request. The bus will not be released, which prevents another master device from requesting between messages. This allows one master device to send multiple requests while in control. The default value is true.

#### Syntax

```
Wire.requestFrom(address, quantity)
Wire.requestFrom(address, quantity, stop)
```

#### Parameters

* _address_: the 7-bit slave address of the device to request bytes from.
* _quantity_: the number of bytes to request.
* _stop_: true or false. true will send a stop message after the request, releasing the bus. False will continually send a restart after the request, keeping the connection active.

#### Returns

* _byte_ : the number of bytes returned from the peripheral device.

#### See also

* [begin()](#begin)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `beginTransmission()`

This function begins a transmission to the I2C peripheral device with the given address. Subsequently, queue bytes for transmission with the write() function and transmit them by calling endTransmission().

#### Syntax

```
Wire.beginTransmission(address)
```

#### Parameters

* _address_: the 7-bit address of the device to transmit to.

#### Returns

None.

#### See also

* [begin()](#begin)
* [requestFrom()](#requestfrom)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `endTransmission()`

This function ends a transmission to a peripheral device that was begun by beginTransmission() and transmits the bytes that were queued by write(). As of Arduino 1.0.1, endTransmission() accepts a boolean argument changing its behavior for compatibility with certain I2C devices. If true, endTransmission() sends a stop message after transmission, releasing the I2C bus. If false, endTransmission() sends a restart message after transmission. The bus will not be released, which prevents another controller device from transmitting between messages. This allows one controller device to send multiple transmissions while in control. The default value is true.

#### Syntax

```
Wire.endTransmission()
Wire.endTransmission(stop)
```

#### Parameters

* _stop_: true or false. True will send a stop message, releasing the bus after transmission. False will send a restart, keeping the connection active.

#### Returns

* _0_: success.
* _1_: data too long to fit in transmit buffer.
* _2_: received NACK on transmit of address.
* _3_: received NACK on transmit of data.
* _4_: other error.

#### See also

* [begin()](#begin)
* [requestFrom()](#requestfrom)
* [beginTransmission()](#begintransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `write()`

This function writes data from a peripheral device in response to a request from a controller device, or queues bytes for transmission from a controller to peripheral device (in-between calls to beginTransmission() and endTransmission()).

#### Syntax

```
Wire.write(value)
Wire.write(string)
Wire.write(data, length)
```

#### Parameters

* _value_: a value to send as a single byte.
* _string_: a string to send as a series of bytes.
* _data_: an array of data to send as bytes.
* _length_: the number of bytes to transmit.
  
#### Returns

The number of bytes written (reading this number is optional).

#### Example

```
#include <Wire.h>

byte val = 0;

void setup() {
  Wire.begin(); // Join I2C bus
}

void loop() {
    Wire.beginTransmission(44);  // Transmit to device number 44 (0x2C)

    Wire.write(val);             // Sends value byte  
    Wire.endTransmission();      // Stop transmitting

    val++;                       // Increment value

    // if reached 64th position (max)
    if(val == 64) {
        val = 0;                   // Start over from lowest value
    }
    
    delay(500);
}
```

#### See also

* [begin()](#begin)
* [requestFrom()](#requestfrom)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `available()`

This function returns the number of bytes available for retrieval with read(). This function should be called on a controller device after a call to requestFrom() or on a peripheral inside the onReceive() handler. available() inherits from the Stream utility class.

#### Syntax

```
Wire.available()
```

#### Parameters

None. 

#### Returns

The number of bytes available for reading.

#### See also

* [begin()](#begin)
* [requestFrom()](#requestFrom)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `read()`

This function reads a byte that was transmitted from a peripheral device to a controller device after a call to requestFrom() or was transmitted from a controller device to a peripheral device. read() inherits from the Stream utility class.

#### Syntax

```
Wire.read()
```

#### Parameters

None. 

#### Returns

The next byte received.

#### Example

```
#include <Wire.h>

void setup() {
  Wire.begin();             // Join I2C bus (address is optional for controller device)
  Serial.begin(9600);       // Start serial for output
}

void loop() {
    Wire.requestFrom(2, 6);    // Request 6 bytes from slave device number two
    
    // Slave may send less than requested
    while(Wire.available()) {
        char c = Wire.read();    // Receive a byte as character
        Serial.print(c);         // Print the character
    }

    delay(500);
}
```

#### See also

* [begin()](#begin)
* [requestFrom()](#requestfrom)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [setClock()](#setclock)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `setClock()`

This function modifies the clock frequency for I2C communication. I2C peripheral devices have no minimum working clock frequency, however 100KHz is usually the baseline.

#### Syntax

```
Wire.setClock(clockFrequency)
```

#### Parameters

* _clockFrequency_: the value (in Hertz) of the desired communication clock. Accepted values are 100000 (standard mode) and 400000 (fast mode). Some processors also support 10000 (low speed mode), 1000000 (fast mode plus) and 3400000 (high speed mode). Please refer to the specific processor documentation to make sure the desired mode is supported.

#### Returns

None.

#### See also

* [begin()](#begin)
* [requestFrom()](#requestfrom)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [onReceive()](#onreceive)
* [onRequest()](#onrequest)

### `onReceive()`

This function registers a function to be called when a peripheral device receives a transmission from a controller device.

#### Syntax

```
Wire.onReceive(handler)
```

#### Parameters

* _handler_: the function to be called when the peripheral device receives data; this should take a single int parameter (the number of bytes read from the controller device) and return nothing.

#### Returns

None.

#### See also

* [begin()](#begin)
* [requestFrom()](#requestfrom)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onRequest()](#onrequest)

### `onRequest()`

This function registers a function to be called when a controller device requests data from a peripheral device.

#### Syntax

```
Wire.onRequest(handler)
```

#### Parameters

* _handler_: the function to be called, takes no parameters and returns nothing.

#### Returns

None.

#### See also

* [begin()](#begin)
* [requestFrom()](#requestfrom)
* [beginTransmission()](#begintransmission)
* [endTransmission()](#endtransmission)
* [write()](#write)
* [available()](#available)
* [read()](#read)
* [setClock()](#setclock)
* [onReceive()](#onreceive)