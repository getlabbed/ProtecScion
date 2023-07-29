 /**
 * @file Task_LED.h
 * @author Skkeye
 * @brief Header file of the Task_Menu.cpp file
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-14 - Initial code entry
 * @date 2023-05-18 - Final code entry 
 * 
 */

#include "global.h"

#define PIN_LED 27

void vTaskLED(void *pvParameters);