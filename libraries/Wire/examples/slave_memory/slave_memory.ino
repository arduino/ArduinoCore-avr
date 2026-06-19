// Wire Slave "256-Byte Memory" Device
// by Jan Wagner

// Demonstrates use of the Wire library to emulate a 256-byte memory device,
// with sequential continued read and sequential continued write access.
// Operates as an I2C/TWI slave device.
//
// The protocol used in this example emulates typical small EEPROMs,
// or i2c-addressible register banks. The master controls the I2C transaction,
// doing one of two things within a single transaction:
//
// 1) master sends an address-byte and sends optional data-byte(s) that the
//    slave shall store into memory starting from the given address,
//
//  or
//
// 2) master sends an address-byte and immediately changes into receive
//    mode, receiving data-byte(s) from slave memory starting from that address.
//  
// The number of data bytes in the transaction is not known in advance.
// The master can at any time stop sending (1) or reading (2) data bytes.
// Either mode, (1) or (2), is carried out as a single multi-byte I2C transaction.
//
// Starting from a base address (between 0 and 255) sent by the master,
// the slave auto-increments the address for each byte sent or read.
// When the end of address space is reached it wraps back to 0.

// Master writing address, then writing data (case 1) is handled simply in the
//   onReceive event - receives starting address plus data bytes from master

// Master writing address, then reading data (2) is handled in the
//   onReceive event - receives starting address from master, no other data
//   onRequest event - sends the first requested data to master
//   onRequestMore event - sends more, continually reinvoked while master keeps reading

// Created 18 December 2023

// This example code is in the public domain.

#include <Wire.h>

static volatile byte shared_memory[256];
static volatile unsigned int memory_addr = 0;

void setup() {
  unsigned int i;

  // initialize memory with a simple pattern
  for(i=0; i<sizeof(shared_memory); i++) {
    shared_memory[i] = i;
  }
  memory_addr = 0;
  
  // initialise Wire
  Wire.begin(8);                // join I2C bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  Wire.onRequestMore(requestMoreEvent); // register event
}

void loop() {
  delay(100);
}

// Helper to get next address within array bounds
unsigned int next_addr(unsigned int curr_addr) {
  return (curr_addr + 1) % (sizeof(shared_memory) + 1);
}

// Function that executes whenever data is sent to slave by master
// This function is registered as an event, see setup()
void receiveEvent(int nbytes) {
  if(nbytes > 0) {

    // receive the memory address that the master wants to access
    memory_addr = Wire.read();
    memory_addr = memory_addr % (sizeof(shared_memory) + 1);
    nbytes--;

    // receive optional data that master might be pushing into client memory
    while(nbytes > 0) {
      shared_memory[memory_addr] = Wire.read();
      memory_addr = next_addr(memory_addr);
      nbytes--;
    }
  }
}

// Function that executes whenever data is first requested by master
// This function is registered as an event, see setup()
void requestEvent() {
  // master started reading: send back the first data byte
  Wire.write(shared_memory[memory_addr]);
  memory_addr = next_addr(memory_addr);
}

// Function that executes each time the master in the current transaction
// tries to read more than initially provided in the onRequest handler above.
// This function is registered as an event, see setup()
void requestMoreEvent() {
  // master continues reading: send back the next data byte
  Wire.write(shared_memory[memory_addr]);
  memory_addr = next_addr(memory_addr);
}
