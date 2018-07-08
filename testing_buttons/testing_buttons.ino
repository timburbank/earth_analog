/*
 * Testing button inputs
 */
#include "MIDIUSB.h"

//// MIDI signal bytes
#define CLOCK_TICK 0xF8
#define CLOCK_START 0xFA
#define CLOCK_STOP 0xFC
#define CONTROL_CHANGE 0xB
#define NOTE_ON 0x9
#define NOTE_OFF 0x8

#define MEASURE_BEATS 4
#define MIDI_CHANNEL 1

byte SWITCH_PINS[4] = {A0, A1, A2, A3};
byte LED_PINS[4] = {13, 12, 11, 10};
byte SWITCH_CC[4] = {20, 21, 22, 23};

bool switchState[4] = {false, false, false, false};

//// Metronome tracking
int ppqn = 0;
int beat = 0;
bool clockActive = false;


void setup() {
  for (byte i = 0; i < sizeof(LED_PINS)/sizeof(LED_PINS[0]); i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    pinMode(SWITCH_PINS[i], INPUT);
  }
}


void loop() {
  midiEventPacket_t rx;

  //// Do while...makes sure all MIDI messages are read before continuing?
  do {
    rx = MidiUSB.read();

    if (rx.byte1 == CLOCK_TICK){
      ppqn++;
      
      if (ppqn == 24){
          beat++;
          if (beat >= MEASURE_BEATS) {
            beat = 0;
            lightsOff();
          }
          MidiUSB.flush();      
          ppqn = 0;
      }
    }
    //Clock start byte
    else if (rx.byte1 == CLOCK_START){
      MidiUSB.flush();
      beat = 0;
      ppqn = 0;
      clockActive = true;
    }
    //Clock stop byte
    else if (rx.byte1 == CLOCK_STOP){
      MidiUSB.flush();
      ppqn = 0;
      clockActive = false;
    }
    
  } while (rx.header != 0);
  
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    if (digitalRead(SWITCH_PINS[i]) == HIGH) {
      //// Only send one CC event when switch is pressed
      if (!switchState[i]) {
        switchState[i] = true;
        controlChange(MIDI_CHANNEL, SWITCH_CC[i], 127);
        digitalWrite(LED_PINS[i], HIGH);
      }
    }
    else {
      if (switchState[i]) {
        switchState[i] = false;
        controlChange(MIDI_CHANNEL, SWITCH_CC[i], 0);
        if (!clockActive) {
          digitalWrite(LED_PINS[i], LOW);
        }
      }
    }
  }
}

void lightsOff() {
  for (byte i = 0; i < sizeof(LED_PINS)/sizeof(LED_PINS[0]); i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
}


// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).
// https://github.com/arduino-libraries/MIDIUSB/blob/master/examples/MIDIUSB_write/MIDIUSB_write.ino
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {CONTROL_CHANGE, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

