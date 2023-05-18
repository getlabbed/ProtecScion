/**
 * @file Task_LCD.cpp
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @brief Fichier d'implémentation de la tâche d'asservissement pour contrôler l'écran LCD
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-07 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code
 * 
 */

#include "Task_LCD.h"
#include "Adafruit_LiquidCrystal.h"

// Variables globales
TimerHandle_t xTimerLCDLine[4];

LCDBuffer_t buffer;

// pointer
Adafruit_LiquidCrystal *lcd;

struct lineNumbers
{
  unsigned int line;
};

void vTaskLCD(void *pvParameters)
{
  // Initialisation des timers
  xTimerLCDLine[0] = xTimerCreate("TimerLCDLine1", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine0);
  xTimerLCDLine[1] = xTimerCreate("TimerLCDLine2", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine1);
  xTimerLCDLine[2] = xTimerCreate("TimerLCDLine3", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine2);
  xTimerLCDLine[3] = xTimerCreate("TimerLCDLine4", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine3);

  xSemaphoreTake(xSemaphoreI2C, portMAX_DELAY);

  // Connection via I2c, DAT: Broche 3, CLK: Broche 5
  Adafruit_LiquidCrystal tempLCD(LCD_I2C_ADDR, &xWireBus);

  // Laisser le temps a l'écran d'initialiser
  vTaskDelay(100 / portTICK_PERIOD_MS);
  lcd = &tempLCD;

  lcd->begin(20, 4);
  lcd->clear();
  vTaskDelay(10 / portTICK_PERIOD_MS);
  xSemaphoreGive(xSemaphoreI2C);

  LCDCommand_t cmdBuffer;

  while (1)
  {
    // Attendre de recevoir une commande dans la file
    if (!xQueueReceive(xQueueLCD, &cmdBuffer, portMAX_DELAY)) continue;

    // Prendre le sémaphore pour avoir accès à l'écran LCD
    while (!xSemaphoreTake(xSemaphoreI2C, portMAX_DELAY)) vTaskDelay(10 / portTICK_PERIOD_MS);

    // if the duration is 0, we don't need to set a timer
    // use something like this : u8Line = (u8Line == 1) ? 2 : (u8Line == 2) ? 1 : u8Line; to get the right line
    // line = (cmdBuffer.line == 1) ? 2 : (cmdBuffer.line == 2) ? 1 : cmdBuffer.line;
    cmdBuffer.message = cmdBuffer.message.substring(0, 19);
    // if (cmdBuffer.duration == 0)
    // {
    //   // set the buffer
    //   buffer.line[cmdBuffer.line] = cmdBuffer.message;
    // }
    // else
    // {
    //   // set the timer
    //   setResetLine(cmdBuffer.line, cmdBuffer.duration);
    // }
    // Serial.println("vSendLCDCommand");
    // Serial.println(cmdBuffer.message);
    // Serial.println(cmdBuffer.line);
    // set the line on the lcd
    lcd->setCursor(0, cmdBuffer.line);
    lcd->print(cmdBuffer.message);
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    xSemaphoreGive(xSemaphoreLCDCommand);
    xSemaphoreGive(xSemaphoreI2C);
  }
}

void setResetLine(unsigned int line, unsigned int duration)
{
  // set the right timer to the duration
  if (xTimerIsTimerActive(xTimerLCDLine[line]) == pdTRUE)
  {
    xTimerStop(xTimerLCDLine[line], 0);
  }
  xTimerChangePeriod(xTimerLCDLine[line], duration / portTICK_PERIOD_MS, 0);
  xTimerStart(xTimerLCDLine[line], 0);
}

void resetLine(int line)
{
  if (xSemaphoreTake(xSemaphoreI2C, portMAX_DELAY) == pdTRUE)
  {
    lcd->setCursor(0, line);
    lcd->print("                    ");
    lcd->setCursor(0, line);
    lcd->print(buffer.line[line]);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    xSemaphoreGive(xSemaphoreI2C);
  }
}

// On ne peux pas passer de paramètre à une fonction appelée par un timer
void resetLine0(void*) {resetLine(0);}
void resetLine1(void*) {resetLine(1);}
void resetLine2(void*) {resetLine(2);}
void resetLine3(void*) {resetLine(3);}
