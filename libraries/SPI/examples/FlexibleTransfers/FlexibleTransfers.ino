/*
 * This is an example of usage of the new flexible transfer functions
 * 
 * Author: Owen Lyke
 * July 2018
 */

#include <SPI.h>

uint8_t txBuffer[5] = {0xC0, 0xAA, 0xFF, 0x55, 0x03};   // A unique pattern that can be identified on a logic analyzer
uint8_t rxBuffer[5] = {0xFA, 0xFB, 0xFC, 0xFD, 0xFE};   // A unique pattern that can be identified on a logic analyzer

uint8_t csPin = 4;                                      // CS helps sync a logic analyzer

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){};

  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  
  SPI.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(csPin, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));

  /* Because SPI hardware is based on transfers data must be sent 
   *  out as it is being received. In some cases only the received 
   *  data is needed. In this case it can be reassuring, or even 
   *  necessary, to know exactly what will be on the MOSI line at
   *  that time. 
   *  
   *  The transferIn(bufIn, size, junk) method allows you to specify a byte to tranmit
   *  while receiving into a buffer.
   */
  SPI.transferIn(rxBuffer, 2, 0xC3);
  delay(1);
  
  /* Again, SPI hardware is based on transfers. The normal 
   *  transfer() method replaces the transmit buffer contents
   *  with whatever was read on the MISO line. This is 
   *  undesireable when hardware SPI is used to control devices
   *  such as addressable LEDs and OLED displays. 
   *  
   *  The transferOut(bufOut, size) method discards any data 
   *  that was received on the MISO line, preserving the contents 
   *  of bufOut.
   */
  SPI.transferOut(rxBuffer, 5);
  SPI.endTransaction();
  digitalWrite(csPin, HIGH);
  delay(5);
}
