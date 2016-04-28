#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "constants.h"
#include "font.h"

// global variables
byte pins[] = {PIN_MATRIX_1,  PIN_MATRIX_2,  PIN_MATRIX_3,  PIN_MATRIX_4, 
               PIN_MATRIX_5,  PIN_MATRIX_6,  PIN_MATRIX_7,  PIN_MATRIX_8,
               PIN_MATRIX_9,  PIN_MATRIX_10, PIN_MATRIX_11, PIN_MATRIX_12,
               PIN_MATRIX_13, PIN_MATRIX_14, PIN_MATRIX_15, PIN_MATRIX_16};
boolean _knownPins[16];
byte _rows[8];
byte _cols[8];
byte _columnVoltage;
byte _rowVoltage;
int _brightness[sizeof(_cols)];

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUTTON, INPUT_PULLUP);  // initialize button pin
  easyConfig();  // optional configuration
  if (getMessageLength() == 0) {
    createMessage((char*)DEFAULT_MESSAGE);
  }
}

  
void loop() {
  displayMessage();
  enterMessage();
} // loop

