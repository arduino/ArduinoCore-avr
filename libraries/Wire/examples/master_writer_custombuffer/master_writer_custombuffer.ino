// Wire Master Writer Custom Buffer

// Demonstrates use of the Wire library with customized buffers
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <WireBuffer.h>
#include "Arduino.h"

// The following text will not fit into the default buffer of 32 bytes.
static const char text[] = "You really won't believe it, but x is ";

constexpr size_t RECEIVE_BUFFER_SIZE  = 0;  // There is no receive in this sketch.
constexpr size_t TRANSMIT_BUFFER_SIZE = 42; // Enhance the buffer to 42 characters.

SET_Wire_BUFFERS(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE,
    true /* master buffers needed */, false /* no slave buffers needed */ );

void setup() {
  Wire.begin(); // join I2C bus (address optional for master)

  // This is just for curiosity and could be removed
  Serial.begin(9600);   // start serial for output
  printWireBufferSize(Serial);
}

static byte x = 0;

void loop() {
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(text);          // sends multiple bytes
  Wire.write(x);             // sends one byte
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(500);
}

void printWireBufferSize(Stream& stream) {
  stream.print("Wire receive buffer size is ");
  stream.println(WireBuffer::RX_BUFFER_SIZE);
  stream.print("Wire transmit buffer size is ");
  stream.println(WireBuffer::TX_BUFFER_SIZE);
  stream.print("twi_masterBuffer size is ");
  stream.println(twiBuffer::TWI_MASTER_BUFFER_SIZE);
  stream.print("twi_rxBuffer size is ");
  stream.println(twiBuffer::TWI_RX_BUFFER_SIZE);
  stream.print("twi_txBuffer size is ");
  stream.println(twiBuffer::TWI_TX_BUFFER_SIZE);
}
