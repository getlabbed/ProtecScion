/**
 * Nom du fichier :
 *  @name Task_SoundSensor.cpp
 * Description :
 *  @brief Fichier d'en-tête du fichier Task_SoundSensor.cpp.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-12 @author Yanick Labelle - Entrée initiale du code.
 */

#include "global.h"

void vTaskKeypad(void *pvParameters);

// Définition des broches du clavier
#define PIN_KEYPAD_1 21
#define PIN_KEYPAD_2 17
#define PIN_KEYPAD_3 16
#define PIN_KEYPAD_4 19
#define PIN_KEYPAD_5 18
#define PIN_KEYPAD_6 5
#define PIN_KEYPAD_7 4

// Définition du nombre de lignes et de colonnes du clavier
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

// Définition de la disposition des touches du clavier
const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '#', '0', '*' }
};