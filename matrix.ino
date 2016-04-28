// The frame is 64 bits.  Each byte holds one column of display data.
// 64 bits, 8 columns for the 64 LEDs in the matrix.
byte _frame[8] = {0,0,0,0,0,0,0,0};

// clear all pins going to matrix (input mode)
void clearMatrix() {
  for (byte i=0; i<sizeof(pins); i++) {
    pinMode(pins[i], INPUT);
  }
}

// copy a font character (row-bytes) into matrix frame (column-bytes)
void copyFontCharacterToFrame(char ch) {
//  Serial.println("copyFontCharacterToFrame");
  int fontOffset = ch*8;  // first byte of character in font table
  for (byte row=0; row<8; row++) {
//    Serial.print("row: "); Serial.print(row);
    byte rowOfDots = pgm_read_byte_near(FONT + fontOffset++);
//    Serial.print(" rowOfDots: "); Serial.println(rowOfDots,BIN);
    for (byte col=0; col<8; col++) {
//      Serial.print("  col: "); Serial.print(col);
      // get the column bit in the row data from the font
      int bit = rowOfDots << col;
      bit &= 0x80;
      bit >>= 7;
//      Serial.print(bit==1?". ":"  ");

      // get the column byte in the frame
      byte columnByte = _frame[col];

      // add the bit to the right most row, shifting the others to the left
      columnByte = (columnByte << 1) + bit;
      _frame[col] = columnByte;
//      Serial.print(" col byte: "); Serial.print(bit,BIN);
    } // columns
//    Serial.println();
  } // rows
} // copyFontCharacterToFrame

// print frame to serial
void printFrame() {
  Serial.println("printFrame");
  for (byte row=0; row<8; row++) {
    for (byte col=0; col<8; col++) {
      byte colOfDots = _frame[col];
      // get the row bit in the column data from the frame
      int bit = colOfDots << row;
      bit &= 0x80;
      bit >>= 7;

      // print the bit in the row
      Serial.print(bit==1 ? ". " : "  ");
    } // columns
    Serial.println();
  } // rows
  Serial.println("end printFrame");
} // printFrame

// display frame to matrix once - one refresh cycle
void displayFrame() {
  long frameStart = millis();
//  Serial.println("displayFrame");
  long timeLeft = REFRESH_TIME;
  for (byte col=0; col<8; col++) {
    byte rowOfDots = _frame[col];
    byte numLEDs = 0;  // how many LEDs lit in column
    for (byte row=0; row<8; row++) {
      // get the row bit in the column data from the frame
      int bit = rowOfDots << row;
      bit &= 0x80;
      bit >>= 7;

      // set the pin for that row bit if it is a 1
      if (bit == 1) { // light this row
        numLEDs++;  // count lit LEDs
        byte pin = _rows[row];
        pin = pins[pin];  // real arduino pin

        // turn on the LED
        pinMode(pin, OUTPUT);
        digitalWrite(pin, _rowVoltage);
      } // if row lit
    } // rows

    // we lit all the rows.  Calculate the delay for showing that many LEDs
    if (numLEDs > 0) {  // at least 1 light in this column lit
      long onTime = _brightness[numLEDs-1];
      timeLeft -= onTime;  // how much left of the refresh cycle

      // turn on the column pin to light all column dots selected
      byte pin = _cols[col];
      pin = pins[pin];  // real arduino pin
      pinMode(pin, OUTPUT);
      digitalWrite(pin, _columnVoltage);
      delayMicroseconds(onTime);
      clearMatrix(); // turn off all the pins
    } // at least one light in column
  } // columns

  // if there is still time left in the refresh cycle, wait it out
  if (timeLeft > 0) {
      delayMicroseconds(timeLeft);
  }
//  Serial.println("end displayFrame");
  long frameEnd = millis();
} // displayFrame

// temporary message length variable so we don't have to change EEPROM too much
int _messageLength;

void startMessage() {
  // add 8 blank columns to message so it can scroll the first letter
  _messageLength = 0;
  setMessageLength(_messageLength);  // store the length permanently at start and end of message creation
  for (int i=0; i<8; i++) {
    addColumnToMessage(0);
  }
}

void endMessage() {
  // add 8 blank columns to end of message so it can scroll off the last letter
  for (int i=0; i<8; i++) {
    addColumnToMessage(0);
  }
  setMessageLength(_messageLength);
}

void addColumnToMessage(byte c) {
//  Serial.print("addColumnToMessage: "); Serial.print(c); Serial.print(" location: "); Serial.println(_messageLength);
  EEPROM.update(EE_DISPLAY_COLS+_messageLength++, c);
//  Serial.println("end addColumnToMessage");
}

void addCharToMessage(char ch) {
  // if EEPROM is almost full, don't add the character
  int bytesLeft = EEPROM.length() - EE_DISPLAY_COLS - _messageLength;
  if (bytesLeft < 10) {
    return;
  }
  
//  Serial.print("addCharToMessage: "); Serial.println(ch);
  copyFontCharacterToFrame(ch);  // 8 column bytes
//  printFrame();

  // we only leave 1 blank column between characters unless it's a space
  byte firstByte=99, lastByte=99;
  for (byte i=0; i<sizeof(_frame); i++) {
    if (_frame[i] != 0) {
      lastByte = i;
      if (firstByte == 99) {
        firstByte = i;
      } // first non-blank
    } // last non-blank
  } // read all columns of frame

  // if we have a space, add some blank columns to message since the last
  // column of the message is already blank
  if (firstByte == 99) {  // a blank frame
//    Serial.println("blank frame");
    addColumnToMessage(0);
    addColumnToMessage(0);
    addColumnToMessage(0);
  } else {
//    Serial.print("firstByte = "); Serial.println(firstByte);
//    Serial.print("lastByte = "); Serial.println(lastByte);
    // add columns to message from first to last byte
    for (byte i=firstByte; i<=lastByte; i++) {
      addColumnToMessage(_frame[i]);
    }
    addColumnToMessage(0); // end character with one blank column
  }
//  Serial.println("end addCharToMessage");
}

void addStringToMessage(char s[]) {
  for (int i=0; s[i] != 0; i++) {
    addCharToMessage(s[i]);
  }
}

void createMessage(char s[]) {
  startMessage();
  addStringToMessage(s);
  endMessage();
}

void displayMessage() {
  _messageLength = getMessageLength();
  while (1) {  // loop until button long pressed
    for (int i=0; i<_messageLength-8; i++) {
      // get knob reading and use it to control speed
      int reduceDelay = getKnobReading(SCROLL_MAX_DELAY - SCROLL_MIN_DELAY);
      // copy 8 bytes of message into display frame
//      Serial.print("frame: ");
      for (byte j=0; j<8; j++) {
        _frame[j] = EEPROM.read(EE_DISPLAY_COLS+i+j);
//        Serial.print(_frame[j]); Serial.print(" ");
      } // copy into frame
//      Serial.println();

      // display frame for a bit
      long endTime = millis()+SCROLL_MAX_DELAY-reduceDelay; // when to stop displaying
      while (millis() < endTime) {
        displayFrame();
      } // display frame until time is up

      // test for button press.  Short press is ignored, long press means exit
      byte pressed = isButtonPressed();
      if (pressed == BUTTON_LONG_PRESS) {
        goto endLoop;
      }
      
    } // scroll one column through message
  } // loop forever
  endLoop: ;

} // displayMessage

void enterMessage() {
  byte numChars = sizeof(MESSAGE_CHARACTERS);
  begin:  // start entering message
  startMessage();
  while (1) {  // enter characters over and over
    // read the knob and display that character
    byte charIndex = getKnobReading(numChars);
    char ch = MESSAGE_CHARACTERS[charIndex];
    copyFontCharacterToFrame(ch);
    displayFrame();

    // read button.  A short press adds the character to the message,
    // a long press starts the message over
    byte pressed = isButtonPressed();
    if (pressed == BUTTON_LONG_PRESS) {
      goto begin;  // start over, abandoning entered message
    } else if (pressed == BUTTON_SHORT_PRESS) {
      // if the knob is turned to the last character, that means we are done
      // with the message.  Instead of entering the character, go to the end
      // of the loop.
      if (charIndex == numChars-1) {  // last character
        break;
      } else {  // enter character at end of message
        addCharToMessage(ch);

        // blink character to show it was entered
        for (byte i=0; i<3; i++) {
          for (byte j=0; j<5; j++) {
            displayFrame();
          } // keep on for awhile
          delay(100);
        } // blink
        
      }
    }

  } // entering characters
  end:   // done entering message
  endMessage();
}

