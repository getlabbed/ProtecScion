 /**
 * @file Task_SoundSensor.h
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief File d'en-tête du fichier Task_SoundSensor.cpp.
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-06 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "global.h"

#define SOUND_SENSOR_PIN 39 // A3
#define RESOLUTION 1024
#define VOLTAGE_REFERENCE 3.3

// Déclaration des tâches
void vTaskSoundSensor(void *pvParameters);