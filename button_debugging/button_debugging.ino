/*
 * Debugging button behaviour. Is it hardware or my code?
 */

byte SWITCH_PINS[4] = {A0, A1, A2, A3};
byte LED_PINS[4] = {13, 12, 11, 10};
byte SWITCH_CC[4] = {20, 21, 22, 23};

bool switchState[4] = {false, false, false, false};
bool signalSent[4] = {false, false, false, false};
unsigned long switchTime[4] = {0, 0, 0, 0};
unsigned long currentTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println('meep');

  for (byte i = 0; i < sizeof(LED_PINS)/sizeof(LED_PINS[0]); i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    pinMode(SWITCH_PINS[i], INPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  //// switchState is used here to make sure the switch only registers 
  //// once each time it's pressed
  //// switchTime ensures a delay after the last switch event to prevent
  //// multiple on/off events with every switch push
  for (byte i = 0; i < sizeof(SWITCH_PINS)/sizeof(SWITCH_PINS[0]); i++) {
    if (digitalRead(SWITCH_PINS[i]) == HIGH) {
      if (!switchState[i] && currentTime > switchTime[i] + 10) {
        switchState[i] = true;
        switchTime[i] = millis();
        digitalWrite(LED_PINS[i], HIGH);
        Serial.print(i);
        Serial.println("on");
      }
    } 
    else {
      currentTime = millis();
      if (switchState[i] && currentTime > switchTime[i] + 100) { //// if switchState
        switchState[i] = false;
        switchTime[i] = millis();
        digitalWrite(LED_PINS[i], LOW);
        Serial.print(i);
        Serial.println("off");
      }
    }
  }
}
