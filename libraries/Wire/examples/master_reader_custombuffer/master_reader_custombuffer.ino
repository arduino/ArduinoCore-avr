// Wire Master Reader Custom Buffer

// Demonstrates use of the Wire library with customized buffers
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <WireBuffer.h>
#include "Arduino.h"

// request 6 bytes from slave device #8
constexpr size_t REQUESTED_BYTE_COUNT = 6;

constexpr size_t RECEIVE_BUFFER_SIZE  = REQUESTED_BYTE_COUNT;
constexpr size_t TRANSMIT_BUFFER_SIZE = 0; // There is no transmit in this sketch.

SET_Wire_BUFFERS(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE,
    true /* master buffers needed */, false /* no slave buffers needed */ );

void setup() {
  Wire.begin();        // join I2C bus (address optional for master)
  Serial.begin(9600);  // start serial for output

  // This is just for curiosity and could be removed
  printWireBufferSize(Serial);
}

void loop() {
  Wire.requestFrom(8, REQUESTED_BYTE_COUNT);

  size_t count = 0;
  while (Wire.available()) { // slave may send less than requested
    ++count;
    char c = Wire.read();    // receive a byte as character
    Serial.print(c);         // print the character
  }
  if(count) {
    Serial.println();
  }
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
