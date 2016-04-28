// button states
#define BUTTON_OFF    0       // currently not pressed - no debounce
#define BUTTON_ON     1       // currently pressed - no debounce
#define BUTTON_SHORT_PRESS 2  // pressed long enough to not be a bounce
#define BUTTON_LONG_PRESS  3  // pressed long enough to be a long press

byte isButtonPressed() {
  if (digitalRead(PIN_BUTTON) == HIGH) { // button not pressed
//    Serial.println("button: not pressed");
    return BUTTON_NO_PRESS;
  }
  
  unsigned long pressStart = millis();
  delay(TIME_BUTTON_SHORT_PRESS);  // wait for debounce

  // we wait for user to let go of button
  while (digitalRead(PIN_BUTTON) == LOW) {
  }
  delay(TIME_BUTTON_SHORT_PRESS);  // wait for debounce

  unsigned long pressTime = millis() - pressStart;  // how long pressed
//  Serial.print("button: ");
//  Serial.println(pressTime >= TIME_BUTTON_LONG_FIRST ? "long press" : "short press");
  return pressTime >= TIME_BUTTON_LONG_FIRST ? BUTTON_LONG_PRESS : BUTTON_SHORT_PRESS;
}

long getKnobReading(long maxReading) {
  // read the knob value and map it to the max value desired
  long sensorValue = analogRead(PIN_KNOB);
  long knobValue = map(sensorValue, 0,1024,0,maxReading);
//  Serial.print("knob: "); Serial.println(knobValue);
  return knobValue;
}

