#include "global.h"

// Broche du capteur de distance infrarouge SHARP GP2D2F
#define PIN_IR_SENSOR 34 // A2

// Paramamètres du filtre
#define FILTER_GAIN 5.0

// limitation de la distance
#define LOW_LIMIT 0.001
#define HIGH_LIMIT 300.0

// Temps entre chaque moyenne
#define AVG_INTERVAL_TIME 30000

// Entêtes de fonctions
float convertToDistance(int sensorValue);

// Déclaration des tâches

void vTaskIRSensor(void *pvParameters);