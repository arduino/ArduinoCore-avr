// Wire Master Reader Custom Buffer

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <TwoWireBuffers.h>
#include "Arduino.h"

// request 6 bytes from slave device #8
constexpr size_t REQUESTED_BYTE_COUNT = 6;

constexpr size_t RECEIVE_BUFFER_SIZE  = REQUESTED_BYTE_COUNT;
constexpr size_t TRANSMIT_BUFFER_SIZE = 0; // There is no transmit in this sketch.

SET_WIRE_BUFFERS(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE,
    true /* master buffers needed */, false /* no slave buffers needed */ );

void setup() {
  Wire.begin();        // join I2C bus (address optional for master)
  Serial.begin(9600);  // start serial for output

  // This is just for curiosity and can be removed
  printWireBuffersCapacity(Serial);
}

void loop() {
  Wire.requestFrom(8, REQUESTED_BYTE_COUNT);

  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
  Serial.println();

  delay(500);
}

void printWireBuffersCapacity(Stream& stream) {
  const auto& buffers = GET_WIRE_BUFFERS();

  stream.print("Wire transmit buffer size is ");
  stream.println(buffers.txWireBufferCapacity());

  stream.print("Wire receive buffer size is ");
  stream.println(buffers.rxWireBufferCapacity());

  stream.print("twi_masterBuffer size is ");
  stream.println(buffers.twi_masterBufferCapacity());

  stream.print("twi_rxBuffer size is ");
  stream.println(buffers.twi_rxBufferCapacity());

  stream.print("twi_txBuffer size is ");
  stream.println(buffers.twi_txBufferCapacity());
}
