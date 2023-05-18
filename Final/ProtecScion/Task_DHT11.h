/**
 * @file Task_DHT11.h
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Fichier d'en-tête du fichier Task_DHT11.cpp
 * @note restrictions: Pour type de carte ESP32 Feather
 * 
 * @version 1.0
 * @date 2023-05-08 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "global.h"

#define PIN_DHT11 33

// Définition des tâches
void vTaskDHT11(void *pvParameters);