/*
 * Testing button inputs
 */
byte SWITCH_PINS[4] = {A0, A1, A2, A3};
byte LED_PINS[4] = {13, 12, 11, 10};

int buttonState = 0;

void setup() {
  for (byte i = 0; i < sizeof(LED_PINS)/sizeof(LED_PINS[0]); i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    pinMode(SWITCH_PINS[i], INPUT);
  }
}

void loop() {
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    if (digitalRead(SWITCH_PINS[i]) == HIGH) {
      digitalWrite(LED_PINS[i], HIGH);
    }
    else {
      digitalWrite(LED_PINS[i], LOW);
    }
  }
}
