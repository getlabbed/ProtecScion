/**
 * @file Task_LCD.cpp
 * @author Skkeye
 * @brief Header file of the Task_LCD.cpp file
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-07 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

#include "global.h"

// I2C LCD address
#define LCD_I2C_ADDR 0x20


// Task declaration
void vTaskLCD(void *pvParameters);

// Function prototypes
void setResetLine(unsigned int line, unsigned int duration);
void resetLine(int line);
void resetLine0(void*);
void resetLine1(void*);
void resetLine2(void*);
void resetLine3(void*);

// lcd 20x4 buffer struct with String
typedef struct
{
  String line[4];
} LCDBuffer_t;
