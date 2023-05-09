
#include "global.h"

#define PIN_DATA 15
#define PIN_CLOCK 32
#define PIN_LATCH 14

// Déclarations des tâches
void vTaskLCD(void *pvParameters);

// Déclarations des fonctions
void setResetLine(unsigned int line, unsigned int duration);
void resetLine0(void *pvParameters);
void resetLine1(void *pvParameters);
void resetLine2(void *pvParameters);
void resetLine3(void *pvParameters);

// lcd 20x4 buffer struct with String
typedef struct
{
  String line[4];
} LCDBuffer_t;
