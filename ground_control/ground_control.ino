/*
 * Arduino based MIDI foot controller, built on an Adafruit Feather
 */

#include "MIDIUSB.h"

// Some of these could be constants as well
byte LED_PINS[4] = {13, 12, 11, 10};
byte SWITCH_PINS[4] = {A0, A1, A2, A3};
byte POT_PIN = A4;
int POT_MIN = 0;
int POT_MAX = 1023;

char last_pot_sent = 0;
byte current_led = 0;
long pot_val = 0; // this long might just be needed for implicit casting of result
long pot_cc = 0;


void setup() {
  // put your setup code here, to run once:

}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

void loop() {
  pot_val = analogRead(POT_PIN);

  pot_cc = pot_val * 127 / POT_MAX;
  if (pot_cc != last_pot_sent) {
     Serial.println(pot_cc);
     last_pot_sent = pot_cc; 
  }

}
