
#include "global.h"

// Déclarations des tâches
void vTaskLCD(void *pvParameters);

// Déclarations des fonctions
void setResetLine(unsigned int line, unsigned int duration);
void resetLine(void *pvParameters);

// lcd 20x4 buffer struct with String
typedef struct
{
  String line1;
  String line2;
  String line3;
  String line4;
} LCDBuffer_t;
