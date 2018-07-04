#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BLEMIDI.h"

#include "BluefruitConfig.h"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEMIDI midi(ble);

#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE_VERSION "0.7.0"

// Some of these could be constants as well
byte LED_PINS[4] = {13, 12, 11, 10};
byte SWITCH_PINS[4] = {A0, A1, A2, A3};
byte POT_PIN = A4;
int POT_MIN = 0;
int POT_MAX = 1023;

char last_pot_sent = 0;
byte current_led = 0;
long pot_val = 0; // this might just be needed for implicit casting of result
long pot_cc = 0;

bool isConnected = false;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("meep");
  delay(1000);
  Serial.println("meep");

  Serial.print(F("Bluefruit Feather: "));

  if ( !ble.begin(VERBOSE_MODE) ) {
    error(F("Couldn't find Bluefruit, check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE ) {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();
  
  /* Set BLE callbacks */
  Serial.println("callback 1...");
  ble.setConnectCallback(connected);
  Serial.println("allback 2...");
  ble.setDisconnectCallback(disconnected);
  Serial.println("callbacks done");
  
  Serial.println(F("Enable MIDI: "));
  
  if ( ! midi.begin(true) ) {
    error(F("Could not enable MIDI"));
  }
    
  ble.verbose(false);
  Serial.print(F("Waiting for a connection..."));
  while(!isConnected) {
    ble.update(500);
  }
  
  //// Set all LED pins to output mode and turn on
  //// From https://learn.adafruit.com/mad-science-test-tube-rack
//  for (byte i = 0; i < sizeof(LED_PINS)/sizeof(LED_PINS[0]); i++){
//    pinMode(LED_PINS[i], OUTPUT);
//    digitalWrite(LED_PINS[i], LOW);
//  }

//  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++){
//
//  }
}

//// the loop function runs over and over again forever
//void loop() {
//  Serial.println(current_led);
//  for (byte j = 0; j < sizeof(LED_PINS)/sizeof(LED_PINS[0]); j++) {
//    digitalWrite(LED_PINS[j], LOW);
//  }
//  digitalWrite(LED_PINS[current_led], HIGH);
//  delay(1000);
//  current_led = (current_led + 1) % 4;
//}

void loop()
{
  pot_val = analogRead(POT_PIN);

  pot_cc = pot_val * 127 / POT_MAX;
  if (pot_cc != last_pot_sent) {
     Serial.println(pot_cc);
     last_pot_sent = pot_cc; 
  }
  delay(100);
}


void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void connected(void) {
  isConnected = true;
  Serial.println(F(" CONNECTED!"));
}

void disconnected(void) {
  Serial.println("disconnected");
  isConnected = false;
}
