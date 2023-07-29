/**
 * @file Task_Apprentissage.h
 * @author Skkeye's coleague
 * @brief Header file for the Task_Apprentissage.cpp file
 * @note restrictions: ESP32 Feather board type
 * 
 * @version 1.0
 * @date 2023-04-30 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

#include "global.h"

// SHARP GP2D2F IR Distance Sensor pin
#define PIN_IR_SENSOR 34 // A2

// Filter parameters
#define FILTER_GAIN 5.0

// fDistance limiting
#define LOW_LIMIT 0.001
#define HIGH_LIMIT 300.0

// Averaging interval
#define AVG_INTERVAL_TIME 30000

// Function prototypes
float convertToDistance(int iSensorValue);

// Task declaration
void vTaskApprentissage(void *pvParameters);