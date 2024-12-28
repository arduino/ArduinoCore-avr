// Wire Master Reader Custom Buffer

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender Custom Buffer" example for use with this

// Created 31 Dec 2024

// This example code is in the public domain.


#include <Wire.h>
#include <TwoWireBuffers.h>

// request 6 bytes from slave device #8
size_t constexpr REQUESTED_BYTE_COUNT = 6;

/**** Begin Customize buffers ****/
// Note: If you spell namespace 'WireBuffers' wrongly, all buffer
// sizes of the 'Wire' object stay at default (32 bytes).
namespace WireBuffers {
  size_t constexpr RECEIVE_BUFFER_SIZE  = REQUESTED_BYTE_COUNT;
  size_t constexpr TRANSMIT_BUFFER_SIZE = 0; // There is no transmit in this sketch.
  // We operate as a master only, so we use the macro that will set slave buffers to zero.
  SET_BUFFERS_FOR_MASTER_ONLY(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE);
}
/**** End Customize buffers ******/

// This is just for curiosity.
// Set to false if you don't want to see actual buffer sizes on serial monitor.
#define VERBOSE true

void setup() {
  Wire.begin();        // join I2C bus (address optional for master)
  Serial.begin(9600);  // start serial for output
#if VERBOSE
  printWireBuffers();
#endif
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
