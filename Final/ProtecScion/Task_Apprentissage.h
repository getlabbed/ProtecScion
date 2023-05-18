/**
 * @file Task_Apprentissage.h
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Fichier d'en-tête du fichier Task_Apprentissage.cpp
 * @note restrictions: Pour type de carte ESP32 Feather
 * 
 * @version 1.0
 * @date 2023-04-30 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "global.h"

// Broche du capteur de Distance infrarouge SHARP GP2D2F
#define PIN_IR_SENSOR 34 // A2

// Paramamètres du filtre
#define FILTER_GAIN 5.0

// limitation de la fDistance
#define LOW_LIMIT 0.001
#define HIGH_LIMIT 300.0

// Temps entre chaque moyenne
#define AVG_INTERVAL_TIME 30000

// Entêtes de fonctions
float convertToDistance(int iSensorValue);

// Déclaration des tâches
void vTaskApprentissage(void *pvParameters);