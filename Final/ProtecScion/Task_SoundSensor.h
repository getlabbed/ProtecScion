/**
 * Nom du fichier :
 *  @name Task_SoundSensor.h
 * Description :
 *  @brief File d'en-tête du fichier Task_SoundSensor.cpp.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-0?-?? @author Yanick Labelle - Entrée initiale du code.
 */

#include "global.h"

#define SOUND_SENSOR_PIN 39 // A3
#define RESOLUTION 1024
#define VOLTAGE_REFERENCE 3.3

// Déclaration des tâches
void vTaskSoundSensor(void *pvParameters);