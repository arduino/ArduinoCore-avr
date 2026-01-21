// Wire Slave Receiver Custom Buffer

// Demonstrates use of the Wire library with customized buffers
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <WireBuffer.h>
#include "Arduino.h"

constexpr size_t RECEIVE_BUFFER_SIZE  = 42; // Be able receive up to 42 characters in one message.
constexpr size_t TRANSMIT_BUFFER_SIZE = 0;  // There is no transmit in this sketch.

SET_Wire_BUFFERS(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE,
    false /* no master buffers needed */, true /* slave buffers needed */ );

void setup() {
  Wire.begin(8);                // join I2C bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  // This is just for curiosity and could be removed
  printWireBufferSize(Serial);
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
//
// Hint: This function is called within an interrupt context.
// That means, that there must be enough space in the Serial output
// buffer for the characters to be printed. Otherwise the
// Serial.print() call will lock up.
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    const char c = Wire.read();  // receive byte as a character
    Serial.print(c);             // print the character
  }
  const int x = Wire.read(); // receive byte as an integer
  Serial.println(x);         // print the integer
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
  delay(250); // Give time to free up Serial output buffer.
}
