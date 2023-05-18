
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
