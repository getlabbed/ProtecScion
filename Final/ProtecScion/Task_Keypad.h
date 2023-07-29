 /**
 * @file Task_SoundSensor.h
 * @author Skkeye
 * @brief Header file of the Task_SoundSensor.cpp file
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-12 - Initial code entry
 * @date 2023-05-18 - Final code entry 
 * 
 */

#include "global.h"

void vTaskKeypad(void *pvParameters);

// Keypad pin definitions
#define PIN_KEYPAD_1 21
#define PIN_KEYPAD_2 17
#define PIN_KEYPAD_3 16
#define PIN_KEYPAD_4 19
#define PIN_KEYPAD_5 18
#define PIN_KEYPAD_6 5
#define PIN_KEYPAD_7 4

// Rows and columns of the keypad
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

// Keypad keys definition
const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '#', '0', '*' }
};