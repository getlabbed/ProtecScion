 /**
 * @file Task_LED.h
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @brief Fichier d'en-tête du fichier Task_Menu.cpp
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-14 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "global.h"

#define PIN_LED 27

void vTaskLED(void *pvParameters);