/*
 * Arduino based MIDI foot controller, built on an Adafruit Feather
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
#define MIDI_CHANNEL 0
#define POT_PIN A4
#define POT_MIN 0
#define POT_MAX 1023
#define POT_CC 1
#define POT_LOG_TYPE 10 //logarithmic base for log-lin conversion
#define POT_LOG_SCALE 330 //idk 

byte SWITCH_PINS[4] = {A0, A1, A2, A3};
byte LED_PINS[4] = {13, 12, 11, 10};
byte SWITCH_CC[4] = {20, 21, 22, 23};

//// Switch vars
bool switchState[4] = {false, false, false, false};
bool signalSent[4] = {false, false, false, false};
unsigned long switchTime[4] = {0, 0, 0, 0};
unsigned long currentTime;

//// Pot vars 
char last_pot_sent = 0;
byte current_led = 0;
long pot_val = 0; // this long might just be needed for implicit casting of result
long pot_val_lin = 0;
long pot_cc_val = 0;

//// Metronome tracking
int ppqn = 0; 
int beat = 0;
bool clockActive = false;

//// This array takes up 50% of feather memory
//// Should be able to use PROGMEM to store in flash,
//// but that has...wierd results
const byte POT_LUT[] = {0, 0, 2, 3, 4, 6, 7, 9, 10, 12, 14, 15, 17, 18, 20, 22, 23, 25, 26, 27, 28, 29, 29, 30, 30, 31, 32, 32, 34, 34, 35, 35, 35, 35, 36, 37, 37, 38, 38, 38, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 46, 46, 47, 47, 48, 48, 49, 50, 50, 50, 50, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 58, 58, 59, 59, 60, 60, 60, 62, 62, 62, 62, 63, 63, 63, 64, 64, 65, 65, 66, 66, 66, 66, 67, 67, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 72, 73, 73, 73, 74, 74, 75, 75, 75, 75, 76, 76, 77, 77, 77, 78, 78, 78, 78, 79, 79, 80, 80, 80, 80, 80, 80, 81, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 84, 84, 84, 84, 84, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 105, 105, 105, 105, 105, 105, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 118, 118, 118, 118, 118, 118, 118, 118, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 121, 121, 121, 121, 121, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 124, 124, 124, 124, 124, 124, 125, 125, 125, 125, 125, 126, 126, 126, 127, 127, 127, 127};

void setup() {
  for (byte i = 0; i < sizeof(LED_PINS)/sizeof(LED_PINS[0]); i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    pinMode(SWITCH_PINS[i], INPUT);
  }
  pinMode(POT_PIN, INPUT);
  Serial.begin(115200);
  Serial.print('meep');
}


void loop() {

  /* Read clock signals and calculate beats */
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
            //// I think we can initialize array to zero as per
            //// https://stackoverflow.com/a/4066536/5266110
            for (byte i = 0; i < sizeof(switchState)/sizeof(switchState[0]); i++) {
              switchState[i] = false;
            }
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

  /* Listen for switch changes and send CCs */
  //// switchState is used here to make sure the switch only registers 
  //// once each time it's pressed
  //// switchTime ensures a delay after the last switch event to prevent
  //// multiple on/off events with every switch push
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    if (digitalRead(SWITCH_PINS[i]) == HIGH) {
      if (!switchState[i] && currentTime > switchTime[i] + 10) {
        switchState[i] = true;
        switchTime[i] = millis();
        controlChange(MIDI_CHANNEL, SWITCH_CC[i], 127);
        digitalWrite(LED_PINS[i], HIGH);
      }
    } 
    else {
      currentTime = millis();
      if (switchState[i] && currentTime > switchTime[i] + 100) { //// if switchState
        switchState[i] = false;
        switchTime[i] = millis();
        controlChange(MIDI_CHANNEL, SWITCH_CC[i], 0);
        if (!clockActive) {
          digitalWrite(LED_PINS[i], LOW);
        }
      }
    }
  }

  /* Listen for pot changes and send CCs */
  pot_val = analogRead(POT_PIN);
//  pot_cc_val = pot_val * 127 / POT_MAX;
  pot_cc_val = POT_LUT[pot_val];

  if (pot_cc_val != last_pot_sent) {
//    Serial.println(pot_cc_val);
     controlChange(MIDI_CHANNEL, POT_CC, pot_cc_val);
     last_pot_sent = pot_cc_val; 
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
  MidiUSB.flush();
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

