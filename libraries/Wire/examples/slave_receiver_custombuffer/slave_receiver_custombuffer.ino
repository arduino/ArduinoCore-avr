// Wire Slave Receiver Custom Buffer

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <TwoWireBuffers.h>
#include "Arduino.h"

size_t constexpr RECEIVE_BUFFER_SIZE  = 42; // Be able receive up to 42 characters in one message.
size_t constexpr TRANSMIT_BUFFER_SIZE = 0;  // There is no transmit in this sketch.

SET_WIRE_BUFFERS(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE,
    true /* master buffers needed */, false /* no slave buffers needed */ );

void setup() {
  Wire.begin(8);                // join I2C bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  // This is just for curiosity and can be removed
  printWireBuffersCapacity(Serial);
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
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
