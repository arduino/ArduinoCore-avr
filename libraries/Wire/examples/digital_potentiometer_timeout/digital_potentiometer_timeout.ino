// I2C Digital Potentiometer with timeout
// originally
// by Nicholas Zambetti <http://www.zambetti.com>
// and Shawn Bonkowski <http://people.interaction-ivrea.it/s.bonkowski/>

// Demonstrates use of the Wire library with timeout
// Controls AD5171 digital potentiometer via I2C/TWI

// Created 7 July 2020

// This example code is in the public domain.

const uint32_t timeout = 5000;  // microseconds, 0 = infinite, default = 25000
const bool reset = true;    // true = force TWI reset if timeout occurs, default = false

#include <Wire.h>

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.setWireTimeout( timeout, reset );
}

byte val = 0;
int count = 0;

void loop() {
  Wire.beginTransmission(44); // transmit to device #44 (0x2c)
  // device address is specified in datasheet
  Wire.write(byte(0x00));            // sends instruction byte
  Wire.write(val);             // sends potentiometer value byte
  Wire.endTransmission();     // stop transmitting

  if ( Wire.getWireTimeoutFlag() ) {
    // handle timeout
    count++;
  }

  val++;        // increment value
  if (val == 64) { // if reached 64th position (max)
    val = 0;    // start over from lowest value
  }
  delay(500);
}
