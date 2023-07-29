/**
 * @file Task_DHT11.h
 * @author Skkeye
 * @brief Header file of the Task_DHT11.cpp file
 * @note restrictions: ESP32 Feather board type
 * 
 * @version 1.0
 * @date 2023-05-08 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

#include "global.h"

#define PIN_DHT11 33

// Task definition
void vTaskDHT11(void *pvParameters);