// information about the pins, columns, and rows
#define UNKNOWN  99

// randomly do all combinations of pins repeatedly.  The know will control the
// speed at which the dots display.
// do it until button is pressed
void showRandomDots() {
//  Serial.println("showRandomDots");
  while (1) {  // do until break
    for (byte x=0; x<16; x++) {
      for (byte y=0; y<16; y++) {
        if (x==y) continue;
        
        byte p1 = x;
        p1 = pins[p1];  // arduino pins
        byte p2 = y;
        p2 = pins[p2];
        // light LED for a safe amount of time and let it rest
        pinMode(p1, OUTPUT);
        digitalWrite(p1, HIGH);
        pinMode(p2, OUTPUT);
        digitalWrite(p2, LOW);
        delayMicroseconds(LED_1_ON);

        // stop the current and give the LED time to rest
        pinMode(p1, INPUT);
        pinMode(p2, INPUT);

        if (isButtonPressed()) {
          goto quit;
        }

        // read the knob and delay depending on the value
//        delay(15-getKnobReading(16));
      } // y
    } // x
  } // forever
  quit: ;
} // showRandomDots

// combinations of pins and voltages to try
byte _combinationCount;
byte _combinationPin1[30];
byte _combinationPin1Voltage[30];
byte _combinationPin2[30];

// add a single combination to the list
void addCombination(byte pin1, byte voltage, byte pin2) {
  _combinationPin1[_combinationCount] = pin1;
  _combinationPin2[_combinationCount] = pin2;
  _combinationPin1Voltage[_combinationCount] = voltage;
  _combinationCount++;
}

// make all combinations of specified pin and all other pins
void makeCombinations(byte pin) {
  _combinationCount = 0;  // clear combinations
  
  for (byte i=0; i<sizeof(pins); i++) {
    if (i == pin) continue;
    addCombination(pin, HIGH, i);
    addCombination(pin, LOW, i);
//    Serial.print("add combination "); Serial.print(pin); Serial.print(" "); Serial.println(i); delay(1000);
  }
} // makeCombinations

// make combinations to show a column voltage display
void makeVoltageCombinations(byte columnVoltage) {
  _combinationCount = 0;  // clear combinations

  // we will always show column 0 row 0 even if the voltage is wrong
  // that way it will light up
  addCombination(_cols[0], HIGH, _rows[0]);
  addCombination(_cols[0], LOW, _rows[0]);

  // we only show column 1, row 0 with the specified voltage.  If the
  // voltage is wrong, you will not see it.
  addCombination(_cols[1], columnVoltage, _rows[0]);
} // makeVoltageCombinations

// show a line of text based on the pin selected by the knob.  Return the pin
// that was chosen by pressing the button.
byte showLine() {
//  Serial.println("showLine");
  byte pin;
  byte oldKnob = 99;  // detect when knob changes
  while (1) { // loop until button pressed
    pin = getKnobReading(16);  // pick a pin to show
    if (pin != oldKnob) {  // a change in the knob
      oldKnob = pin;  // remember new position

      // make all combinations of the selected pin with all other pins
      makeCombinations(pin);
//      Serial.print("pin "); Serial.println(pin);
    }

    // if we areeady know this pin, don't do anything.  User needs to pick
    // a different one
    if (_knownPins[pin]) continue;

    // try all combinations of pins to light the line
    for (int combination = 0; combination < _combinationCount; combination++) {
      byte p1 = _combinationPin1[combination];
      p1 = pins[p1]; // the real arduino pin
      byte p2 = _combinationPin2[combination];
      p2 = pins[p2]; // the real arduino pin
      byte p1Voltage = _combinationPin1Voltage[combination];
      byte p2Voltage = !p1Voltage;
//      Serial.print(p1); Serial.print(" "); Serial.println(p2);// delay(1000);

      // send current between the two pins for the time needed to show 1 LED
      pinMode(p1, OUTPUT);
      digitalWrite(p1, p1Voltage);
      pinMode(p2, OUTPUT);
      digitalWrite(p2, p2Voltage);
      delayMicroseconds(LED_1_ON);

      // stop the current
      pinMode(p1, INPUT);
      pinMode(p2, INPUT);
    }

    // quit when button pressed
    if (isButtonPressed()) {
      break;
    }
  } // loop until button pressed

  // blink 3 times to show that it was selected
  for (byte i=0; i<3; i++) {
    for (byte j=0; j<20; j++) {
      for (int combination = 0; combination < _combinationCount; combination++) {
        byte p1 = _combinationPin1[combination];
        p1 = pins[p1]; // the real arduino pin
        byte p2 = _combinationPin2[combination];
        p2 = pins[p2]; // the real arduino pin
        byte p1Voltage = _combinationPin1Voltage[combination];
        byte p2Voltage = !p1Voltage;

        // send current between the two pins for the time needed to show 1 LED
        pinMode(p1, OUTPUT);
        digitalWrite(p1, p1Voltage);
        pinMode(p2, OUTPUT);
        digitalWrite(p2, p2Voltage);
        delayMicroseconds(LED_1_ON);

        // stop the current
        pinMode(p1, INPUT);
        pinMode(p2, INPUT);
      } // combinations
    } // keep on for awhile
    delay(100);
  } // blink
  
//  Serial.print("showLine returns "); Serial.println(pin);
  return pin;
} // showLine

byte calculateColumnVoltage() {
//  Serial.println("calculateColumnVoltage");
  byte columnVoltage;

  // loop until button pressed
  while(1) {
    // column voltage depends on how you turn the knob
    columnVoltage = getKnobReading(2) == 0 ? HIGH : LOW;

    // display the pins for this column voltage
    makeVoltageCombinations(columnVoltage);
    for (int combination = 0; combination < _combinationCount; combination++) {
      byte p1 = _combinationPin1[combination];
      p1 = pins[p1]; // the real arduino pin
      byte p2 = _combinationPin2[combination];
      p2 = pins[p2]; // the real arduino pin
      byte p1Voltage = _combinationPin1Voltage[combination];
      byte p2Voltage = !p1Voltage;

      // send current between the two pins for the time needed to show 1 LED
      pinMode(p1, OUTPUT);
      digitalWrite(p1, p1Voltage);
      pinMode(p2, OUTPUT);
      digitalWrite(p2, p2Voltage);
      delayMicroseconds(LED_1_ON);

      // stop the current
      pinMode(p1, INPUT);
      pinMode(p2, INPUT);

      // this is such a small number of dots, we will delay to not make the LEDs overheat
      delay(10);
    } // combinations

    // quit when button pressed
    if (isButtonPressed()) {
      break;
    }
    
  } // until button pressed

//  Serial.print("calculateColumnVoltage returns "); Serial.println(columnVoltage);
  return columnVoltage;
} // calculateColumnVoltage

void calibrateBrightness() {
//  Serial.println("calibrateBrightness");
  // initialize LED brightness values
  for (int i=0; i<8; i++) _brightness[i] = 0;

  byte columnVoltage = _columnVoltage;
  byte rowVoltage = columnVoltage == HIGH ? LOW : HIGH;
  long onTime;
  long offTime;
  byte c1 = _cols[0];
  c1 = pins[c1]; // the real arduino pin
  byte c2 = _cols[1];
  c2 = pins[c2]; // the real arduino pin
  byte c3 = _cols[2];
  c3 = pins[c3]; // the real arduino pin
  byte c4 = _cols[3];
  c4 = pins[c4]; // the real arduino pin
  byte c5 = _cols[4];
  c5 = pins[c5]; // the real arduino pin
  byte c6 = _cols[5];
  c6 = pins[c6]; // the real arduino pin
  byte c7 = _cols[6];
  c7 = pins[c7]; // the real arduino pin
  byte c8 = _cols[7];
  c8 = pins[c8]; // the real arduino pin
  byte r1 = _rows[4];
  r1 = pins[r1]; // the real arduino pin
  byte r2 = _rows[5];
  r2 = pins[r2]; // the real arduino pin

  // show 1 LED and vary its brightness with the knob
  while (1) {
    onTime = getKnobReading(LED_MAX_ON);
    offTime = REFRESH_TIME - onTime;
//    Serial.print(onTime); Serial.print("\t"); Serial.println(offTime);

    // send current between the two pins for the knob time
    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    // stop the current for the rest of the refresh cycle
    pinMode(c1, INPUT);
    pinMode(r1, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time of 1 LED
    if (isButtonPressed()) {
      _brightness[0] = onTime;
//      Serial.print("1 LED "); Serial.println(onTime);
      break;
    }
  } // showing 1 LED

  // show 2 LEDs and vary their brightness with the knob
  // always show 1 LED with the specified brightness for comparison
  while (1) {
    onTime = _brightness[0];
    offTime = REFRESH_TIME - onTime;
    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(r1, INPUT);

    onTime = getKnobReading(LED_MAX_ON);
    offTime = offTime - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(c2, OUTPUT);
    digitalWrite(c2, columnVoltage);
    pinMode(r2, OUTPUT);
    digitalWrite(r2, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(c2, INPUT);
    pinMode(r2, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time of 1 LED
    if (isButtonPressed()) {
      _brightness[1] = onTime;
//      Serial.print("2 LEDs "); Serial.println(onTime);
      break;
    }
    
  } // showing 2 LEDs

  // show 3 LEDs and vary their brightness with the knob
  // always show 1 LED with the specified brightness for comparison
  while (1) {
    onTime = _brightness[0];
    offTime = REFRESH_TIME - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(r1, INPUT);

    onTime = getKnobReading(LED_MAX_ON);
    offTime = offTime - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(c2, OUTPUT);
    digitalWrite(c2, columnVoltage);
    pinMode(c3, OUTPUT);
    digitalWrite(c3, columnVoltage);
    pinMode(r2, OUTPUT);
    digitalWrite(r2, rowVoltage);
    delayMicroseconds(onTime);

    // stop the current for the rest of the refresh cycle
    pinMode(c1, INPUT);
    pinMode(c2, INPUT);
    pinMode(c3, INPUT);
    pinMode(r2, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time
    if (isButtonPressed()) {
      _brightness[2] = onTime;
//      Serial.print("3 LEDs "); Serial.println(onTime);
      break;
    }
    
  } // showing 3 LEDs

  // show 4 LEDs and vary their brightness with the knob
  // always show 1 LED with the specified brightness for comparison
  while (1) {
    onTime = _brightness[0];
    offTime = REFRESH_TIME - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(r1, INPUT);

    onTime = getKnobReading(LED_MAX_ON);
    offTime = offTime - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(c2, OUTPUT);
    digitalWrite(c2, columnVoltage);
    pinMode(c3, OUTPUT);
    digitalWrite(c3, columnVoltage);
    pinMode(c4, OUTPUT);
    digitalWrite(c4, columnVoltage);
    pinMode(r2, OUTPUT);
    digitalWrite(r2, rowVoltage);
    delayMicroseconds(onTime);

    // stop the current for the rest of the refresh cycle
    pinMode(c1, INPUT);
    pinMode(c2, INPUT);
    pinMode(c3, INPUT);
    pinMode(c4, INPUT);
    pinMode(r2, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time
    if (isButtonPressed()) {
      _brightness[3] = onTime;
//      Serial.print("4 LEDs "); Serial.println(onTime);
      break;
    }
    
  } // showing 4 LEDs

  // show 5 LEDs and vary their brightness with the knob
  // always show 1 LED with the specified brightness for comparison
  while (1) {
    onTime = _brightness[0];
    offTime = REFRESH_TIME - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(r1, INPUT);

    onTime = getKnobReading(LED_MAX_ON);
    offTime = offTime - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(c2, OUTPUT);
    digitalWrite(c2, columnVoltage);
    pinMode(c3, OUTPUT);
    digitalWrite(c3, columnVoltage);
    pinMode(c4, OUTPUT);
    digitalWrite(c4, columnVoltage);
    pinMode(c5, OUTPUT);
    digitalWrite(c5, columnVoltage);
    pinMode(r2, OUTPUT);
    digitalWrite(r2, rowVoltage);
    delayMicroseconds(onTime);

    // stop the current for the rest of the refresh cycle
    pinMode(c1, INPUT);
    pinMode(c2, INPUT);
    pinMode(c3, INPUT);
    pinMode(c4, INPUT);
    pinMode(c5, INPUT);
    pinMode(r2, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time
    if (isButtonPressed()) {
      _brightness[4] = onTime;
//      Serial.print("5 LEDs "); Serial.println(onTime);
      break;
    }
    
  } // showing 5 LEDs

  // show 6 LEDs and vary their brightness with the knob
  // always show 1 LED with the specified brightness for comparison
  while (1) {
    onTime = _brightness[0];
    offTime = REFRESH_TIME - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(r1, INPUT);

    onTime = getKnobReading(LED_MAX_ON);
    offTime = offTime - onTime;

    // send current between the two pins for the knob time
    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(c2, OUTPUT);
    digitalWrite(c2, columnVoltage);
    pinMode(c3, OUTPUT);
    digitalWrite(c3, columnVoltage);
    pinMode(c4, OUTPUT);
    digitalWrite(c4, columnVoltage);
    pinMode(c5, OUTPUT);
    digitalWrite(c5, columnVoltage);
    pinMode(c6, OUTPUT);
    digitalWrite(c6, columnVoltage);
    pinMode(r2, OUTPUT);
    digitalWrite(r2, rowVoltage);
    delayMicroseconds(onTime);

    // stop the current for the rest of the refresh cycle
    pinMode(c1, INPUT);
    pinMode(c2, INPUT);
    pinMode(c3, INPUT);
    pinMode(c4, INPUT);
    pinMode(c5, INPUT);
    pinMode(c6, INPUT);
    pinMode(r2, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time
    if (isButtonPressed()) {
      _brightness[5] = onTime;
//      Serial.print("6 LEDs "); Serial.println(onTime);
      break;
    }
    
  } // showing 6 LEDs
  
  // show 7 LEDs and vary their brightness with the knob
  // always show 1 LED with the specified brightness for comparison
  while (1) {
    onTime = _brightness[0];
    offTime = REFRESH_TIME - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(r1, INPUT);

    onTime = getKnobReading(LED_MAX_ON);
    offTime = offTime - onTime;

    // send current between the two pins for the knob time
    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(c2, OUTPUT);
    digitalWrite(c2, columnVoltage);
    pinMode(c3, OUTPUT);
    digitalWrite(c3, columnVoltage);
    pinMode(c4, OUTPUT);
    digitalWrite(c4, columnVoltage);
    pinMode(c5, OUTPUT);
    digitalWrite(c5, columnVoltage);
    pinMode(c6, OUTPUT);
    digitalWrite(c6, columnVoltage);
    pinMode(c7, OUTPUT);
    digitalWrite(c7, columnVoltage);
    pinMode(r2, OUTPUT);
    digitalWrite(r2, rowVoltage);
    delayMicroseconds(onTime);

    // stop the current for the rest of the refresh cycle
    pinMode(c1, INPUT);
    pinMode(c2, INPUT);
    pinMode(c3, INPUT);
    pinMode(c4, INPUT);
    pinMode(c5, INPUT);
    pinMode(c6, INPUT);
    pinMode(c7, INPUT);
    pinMode(r2, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time
    if (isButtonPressed()) {
      _brightness[6] = onTime;
//      Serial.print("7 LEDs "); Serial.println(onTime);
      break;
    }
    
  } // showing 7 LEDs

  // show 8 LEDs and vary their brightness with the knob
  // always show 1 LED with the specified brightness for comparison
  while (1) {
    onTime = _brightness[0];
    offTime = REFRESH_TIME - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(r1, OUTPUT);
    digitalWrite(r1, rowVoltage);
    delayMicroseconds(onTime);

    pinMode(c1, INPUT);
    pinMode(r1, INPUT);

    onTime = getKnobReading(LED_MAX_ON);
    offTime = offTime - onTime;

    pinMode(c1, OUTPUT);
    digitalWrite(c1, columnVoltage);
    pinMode(c2, OUTPUT);
    digitalWrite(c2, columnVoltage);
    pinMode(c3, OUTPUT);
    digitalWrite(c3, columnVoltage);
    pinMode(c4, OUTPUT);
    digitalWrite(c4, columnVoltage);
    pinMode(c5, OUTPUT);
    digitalWrite(c5, columnVoltage);
    pinMode(c6, OUTPUT);
    digitalWrite(c6, columnVoltage);
    pinMode(c7, OUTPUT);
    digitalWrite(c7, columnVoltage);
    pinMode(c8, OUTPUT);
    digitalWrite(c8, columnVoltage);
    pinMode(r2, OUTPUT);
    digitalWrite(r2, rowVoltage);
    delayMicroseconds(onTime);

    // stop the current for the rest of the refresh cycle
    pinMode(c1, INPUT);
    pinMode(c2, INPUT);
    pinMode(c3, INPUT);
    pinMode(c4, INPUT);
    pinMode(c5, INPUT);
    pinMode(c6, INPUT);
    pinMode(c7, INPUT);
    pinMode(c8, INPUT);
    pinMode(r2, INPUT);
    delayMicroseconds(offTime);

    // when button pressed, store the on time
    if (isButtonPressed()) {
      _brightness[7] = onTime;
//      Serial.print("8 LEDs "); Serial.println(onTime);
      break;
    }
    
  } // showing 8 LEDs

  // store brightness values in EEPROM
  for (int i=0; i<8; i++) {
    // we scale the brightness to fit in a byte
    byte brightness = map(_brightness[i], 0,LED_MAX_ON,0,255);
    EEPROM.update(EE_LED_ON_TIMES+i, brightness);
  }

} // calibrateBrightness

void easyConfig() {
  // if the magic number is ok and if the user is not holding in the button
  // when this is called, we don't have to configure anything
  byte magicNumber = EEPROM.read(EE_MAGIC_NUMBER);
  if (magicNumber == MAGIC_NUMBER) {  // already setup
    byte pressed = isButtonPressed();
    if (pressed == BUTTON_NO_PRESS) {
      // we don't want to do a manual config
//      Serial.println("skipping configuring");
      restoreConfigVars();
      return;  // already set up
    } else if (pressed == BUTTON_SHORT_PRESS) {
      // we just calibrate brightness
//      Serial.println("calibrate brightness");
      restoreConfigVars();
      calibrateBrightness();
      return;
    } // manual config
  } // does not need config

  // if we get here, do a full configuration
  
//  Serial.println("start configuring");

  // initialize variables
  for (byte pin=0; pin<sizeof(pins); pin++) {
    _knownPins[pin] = false;
  }
  for (byte i=0; i<sizeof(_cols); i++) {
    _rows[i] = UNKNOWN;
    _cols[i] = UNKNOWN;
  }
  
  // initialize pins
  clearMatrix();
  showRandomDots();

  // the user selects the pins in order: col 0-7, row 0-7
  for (byte col=0; col<8; col++) {
    byte pin = showLine();
    _cols[col] = pin;
    _knownPins[pin] = true;
//    Serial.print("pin "); Serial.print(pin); Serial.print(" col "); Serial.println(col);
  }
  for (byte row=0; row<8; row++) {
    byte pin = showLine();
    _rows[row] = pin;
    _knownPins[pin] = true;
//    Serial.print("pin "); Serial.print(pin); Serial.print(" row "); Serial.println(row);
  }

  // store column and row pins into EEPROM
  for (int i=0; i<sizeof(_cols); i++) {
    EEPROM.update(EE_COL_PINS+i, _cols[i]);
    EEPROM.update(EE_ROW_PINS+i, _rows[i]);
  }

  _columnVoltage = calculateColumnVoltage();
  _rowVoltage = _columnVoltage == 1 ? 0 : 1;
  setMessageLength(0);  // how many rows in message

  // store voltage values into EEPROM
  EEPROM.update(EE_COL_VOLTAGE, _columnVoltage);
  EEPROM.update(EE_ROW_VOLTAGE, _rowVoltage);
  
  calibrateBrightness();

  // store the magic number in EEPROM since we are finished configuring
  EEPROM.update(EE_MAGIC_NUMBER, MAGIC_NUMBER);  // indicate memory initialized
  setMessageLength(0);  // no message yet

  // print out the EEPROM values stored
//  for (byte i=0; i<EE_DISPLAY_COLS; i++) {
//    Serial.print(i); Serial.print("\t"); Serial.println(EEPROM.read(i));
//  }
//  Serial.println("done configuring");
} // easyConfig

void restoreConfigVars() {
//  Serial.println("restoreConfigVars");

  // store column pins, row pins, and brightness values
  for (int i=0; i<sizeof(_cols); i++) {
    _cols[i] = EEPROM.read(EE_COL_PINS+i);
    _rows[i] = EEPROM.read(EE_ROW_PINS+i);
    // we scale the brightness from a byte to the real brightness value
    int brightness = map(EEPROM.read(EE_LED_ON_TIMES+i), 0,255,0,LED_MAX_ON);
    _brightness[i] = brightness;
  }

  _columnVoltage = EEPROM.read(EE_COL_VOLTAGE);
  _rowVoltage = EEPROM.read(EE_ROW_VOLTAGE);
}

int getMessageLength() {
  int l;
  EEPROM.get(EE_DISPLAY_LEN, l);
  return l;
}

int setMessageLength(int l) {
  EEPROM.put(EE_DISPLAY_LEN, l);
}

