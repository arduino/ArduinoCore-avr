// Wire Master Writer Custom Buffer

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <TwoWireBuffers.h>

// The following text will not fit into the default buffer of 32 bytes.
static const char text[] = "You really won't believe it, but x is ";

/**** Begin Customize buffers ****/
// Note: If you spell namespace 'WireBuffers' wrongly, all buffer sizes
// of the 'Wire' object stay at default (32 bytes).
namespace WireBuffers {
  size_t constexpr RECEIVE_BUFFER_SIZE  = 0;  // There is no receive in this sketch.
  size_t constexpr TRANSMIT_BUFFER_SIZE = 42; // Enhance the buffer to 42 characters.
  // We operate as a master only, so we use the macro that will set slave buffers to zero.
  SET_BUFFERS_FOR_MASTER_ONLY(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE);
}
/**** End Customize buffers ******/

// This is just for curiosity.
// Set to false if you don't want to see actual buffer sizes on serial monitor.
#define VERBOSE true

void setup() {
  Wire.begin(); // join I2C bus (address optional for master)
#if VERBOSE
  Serial.begin(9600);           // start serial for output
  printWireBuffers();
#endif
}

static byte x = 0;

void loop() {
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(text);        // sends five bytes
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(500);
}

#if VERBOSE
void printWireBuffers() {
  delay(100);
  Serial.println();
  Serial.print("Wire transmit buffer size is ");
  Serial.println(Wire.txBufferCapacity());
  Serial.print("Wire receive buffer size is ");
  Serial.println(Wire.rxBufferCapacity());

  TwoWireBuffers::Interface& buffers = WireBuffers::instance();
  Serial.print("twi_masterBuffer size is ");
  Serial.println(buffers.twi_masterBufferCapacity());

  Serial.print("twi_rxBuffer size is ");
  Serial.println(buffers.twi_rxBufferCapacity());

  Serial.print("twi_txBuffer size is ");
  Serial.println(buffers.twi_txBufferCapacity());
  delay(500);
}
#endif
