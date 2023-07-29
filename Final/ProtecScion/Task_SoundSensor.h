 /**
 * @file Task_SoundSensor.h
 * @author Skkeye's coleague
 * @brief Header file for Task_SoundSensor.cpp
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-06 - Initial code entry
 * @date 2023-05-18 - Final code entry 
 * 
 */

#include "global.h"

#define SOUND_SENSOR_PIN 39 // A3
#define RESOLUTION 1024
#define VOLTAGE_REFERENCE 3.3

// Task declaration
void vTaskSoundSensor(void *pvParameters);