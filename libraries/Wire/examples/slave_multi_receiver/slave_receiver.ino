// Wire Multi-Address Slave Receiver
// by Tijs van Roon

// Demonstrates use of the Wire library with multiple slave addresses
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 26 May 2019

// This example code is in the public domain.

#include <Wire.h>

void setup() {
  uint8_t addr1 = 0x08, addr2 = 0x10; // our slave addresses
  uint8_t addr_mask = (addr1 | addr2) ^ (addr1 & addr2);

  Wire.begin((uint8_t)(addr1 | addr2), addr_mask); // join i2c bus with both addresses
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  int address = Wire.getLastAddress();
  Serial.print("0x"); 
  Serial.print(address, HEX); 
  Serial.print(": ");
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
