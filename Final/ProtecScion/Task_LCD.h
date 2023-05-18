/**
 * @file Task_LCD.cpp
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @brief Fichier d'en-tête du fichier Task_LCD.cpp
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-07 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code
 * 
 */

#include "global.h"

// Adresse I2C de l'écran ACL
#define LCD_I2C_ADDR 0x20


// Déclarations des tâches
void vTaskLCD(void *pvParameters);

// Déclarations des fonctions
void setResetLine(unsigned int line, unsigned int duration);
void resetLine(int line);
void resetLine0(void*);
void resetLine1(void*);
void resetLine2(void*);
void resetLine3(void*);

// lcd 20x4 buffer struct with String
typedef struct
{
  String line[4];
} LCDBuffer_t;
