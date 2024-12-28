// Wire Slave Sender Custom Buffer

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <TwoWireBuffers.h>

static const char text[] = "hello "; // respond with message of 6 bytes

/**** Begin Customize buffers ****/
// Note: If you spell namespace 'WireBuffers' wrongly, all buffer sizes
// of the 'Wire' object stay at default (32 bytes).
namespace WireBuffers {
  size_t constexpr RECEIVE_BUFFER_SIZE  = 0; // There is no receive in this sketch.
  size_t constexpr TRANSMIT_BUFFER_SIZE = sizeof(text); // Don't need a byte for the \0
  // We operate as a slave only, so we use the macro that will set master buffers to zero.
  SET_BUFFERS_FOR_SLAVE_ONLY(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE);
}
/**** End Customize buffers ******/

// This is just for curiosity.
// Set to false if you don't want to see actual buffer sizes on serial monitor.
#define VERBOSE true

void setup() {
  Wire.begin(8);                // join I2C bus with address #8
  Wire.onRequest(requestEvent); // register event
#if VERBOSE
  Serial.begin(9600);           // start serial for output
  printWireBuffers();
#endif
}

void loop() {
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(text);
  // as expected by master
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
