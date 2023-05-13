
#include "global.h"

#define KEYPAD1 36

void vTaskKeypad(void *pvParameters);

#define PIN_KEYPAD_1 21
#define PIN_KEYPAD_2 17
#define PIN_KEYPAD_3 16
#define PIN_KEYPAD_4 19
#define PIN_KEYPAD_5 18
#define PIN_KEYPAD_6 5
#define PIN_KEYPAD_7 4

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '#', '0', '*' }
};