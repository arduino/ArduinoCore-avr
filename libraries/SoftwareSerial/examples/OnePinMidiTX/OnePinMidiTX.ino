/*
 Send MIDI notes via Software serial.

 Demonstrates using only one pin (TX), if bidirectional communication isn't needed.

 Based on https://www.arduino.cc/en/tutorial/midi and https://www.arduino.cc/en/Tutorial/SoftwareSerialExample

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>

#define MIDI_OUT_TX_PIN 9

SoftwareSerial midiOutSerial = SoftwareSerial();

void setup() {
  midiOutSerial.setTX(MIDI_OUT_TX_PIN);
  // set the MIDI data rate for the SoftwareSerial port
  midiOutSerial.begin(31250);
}

// Sends a Bb 3 MIDI every other second.
void loop() {
  noteOn(0x90, 0x3A, 0x45);
  delay(1000);
  noteOn(0x90, 0x3A, 0x00);
  delay(1000);
}

void noteOn(int cmd, int pitch, int velocity) {
  midiOutSerial.write(cmd);
  midiOutSerial.write(pitch);
  midiOutSerial.write(velocity);
}
