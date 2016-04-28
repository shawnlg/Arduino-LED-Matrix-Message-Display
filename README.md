# Arduino-LED-Matrix-Message-Display
## Introduction ##
This sketch allows you to wire up an 8x8 LED matrix without knowing which pins are for what.  An easy config will guide you through setting up the display.  Then you can enter and display messages on the matrix.  The only external components needed are a potentiometer (volume control) and push-button switch, besides the matrix and Arduino.

## Features ##
1. All 16 pins of the LED matrix are connected directly to a range of pins on the Arduino.  It does not matter which pins are connected where.  The guided setup will configure and remember the correct connections to display the characters on the Matrix
2. Scrolling message with variable speed
3. Easy message entering with a single push-button and knob to control everything
4. Message and wiring config stored in EEPROM, so turning off the Arduino does not lose configuration
5. A tutorial document that explains how to wire it up and how to configure it.

## Tutorial ##
The tutorial assumes that you burn the sketch on a stand-alone ATMega328.  I built it as a kit for my nephews to put together.
