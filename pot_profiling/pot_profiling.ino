#define POT_PIN A4
#define POT_MIN 0
#define POT_MAX 1023

long pot_val = 0;

void setup() {
  pinMode(POT_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  pot_val = analogRead(POT_PIN);

  if (pot_val > POT_MIN && pot_val < POT_MAX) {
//    Serial.print(millis());
//    Serial.print("\t");
    Serial.println(pot_val);
  }
  delay(1);
}
