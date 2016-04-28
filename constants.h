// pins
#define PIN_BUTTON    18
#define PIN_KNOB      A5
#define PIN_MATRIX_1  2
#define PIN_MATRIX_2  3
#define PIN_MATRIX_3  4
#define PIN_MATRIX_4  5
#define PIN_MATRIX_5  6
#define PIN_MATRIX_6  7
#define PIN_MATRIX_7  8
#define PIN_MATRIX_8  9
#define PIN_MATRIX_9  10
#define PIN_MATRIX_10 11
#define PIN_MATRIX_11 12
#define PIN_MATRIX_12 13
#define PIN_MATRIX_13 14
#define PIN_MATRIX_14 15
#define PIN_MATRIX_15 16
#define PIN_MATRIX_16 17

// time constants
#define TIME_IDLE               5000 // assume no more input
#define TIME_BUTTON_SHORT_PRESS 50 // for debouncing
#define TIME_BUTTON_LONG_FIRST  1000 // hold in this long for long press
#define TIME_BUTTON_LONG_NEXT   500 // repeated long press event

// button press constants
#define BUTTON_NO_PRESS    0
#define BUTTON_SHORT_PRESS 1
#define BUTTON_LONG_PRESS  2

// LED display times for on and off
#define LED_1_ON  100        // minimum number of microseconds to show one LED
#define REFRESH_TIME 10000   // microseconds for each display cycle
#define LED_MAX_ON 2000      // the most an LED will stay on
#define SCROLL_MAX_DELAY 200 // longest time between column scrolls
#define SCROLL_MIN_DELAY  10 // minimum time between column scrolls

// EEPROM constants
#define MAGIC_NUMBER 125

// EEPROM locations
#define EE_MAGIC_NUMBER  0
#define EE_COL_PINS      1  // start of 8 column pins
#define EE_ROW_PINS      9  // start of 8 row pins
#define EE_COL_VOLTAGE  17  // pin level for a column select
#define EE_ROW_VOLTAGE  18  // pin level for a row select
#define EE_LED_ON_TIMES 19  // 1 for each LED in row or column, 0 means cannot have that many on
#define EE_DISPLAY_LEN  27  // 2 byte length
#define EE_DISPLAY_COLS 29  // start of 8-bit columns to display

const char MESSAGE_CHARACTERS[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char DEFAULT_MESSAGE[] = "HELLO WORLD!";

