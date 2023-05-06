#include "global.h"

#define SOUND_SENSOR_PIN A3
#define RESOLUTION 1024
#define VOLTAGE_REFERENCE 3.3

// Déclaration des tâches
void vTaskSoundSensor(void *pvParameters);