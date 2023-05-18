#include "Task_LCD.h"
#include "Adafruit_LiquidCrystal.h"

// Global variables
TimerHandle_t xTimerLCDLine[4];

LCDBuffer_t buffer;

// lcd pointer
Adafruit_LiquidCrystal *lcd;

struct lineNumbers
{
  unsigned int line;
};

void vTaskLCD(void *pvParameters)
{
  // init timers
  xTimerLCDLine[0] = xTimerCreate("TimerLCDLine1", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine0);
  xTimerLCDLine[1] = xTimerCreate("TimerLCDLine2", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine1);
  xTimerLCDLine[2] = xTimerCreate("TimerLCDLine3", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine2);
  xTimerLCDLine[3] = xTimerCreate("TimerLCDLine4", 1000 / portTICK_PERIOD_MS, pdFALSE, NULL, resetLine3);

  xSemaphoreTake(xSemaphoreI2C, portMAX_DELAY);

  // Connect via SPI. Data pin is #4, clock pin is #5
  Adafruit_LiquidCrystal tempLCD(LCD_I2C_ADDR, &xWireBus);

  // delay to let the lcd initialise
  vTaskDelay(100 / portTICK_PERIOD_MS);
  lcd = &tempLCD;

  lcd->begin(20, 4);
  lcd->clear();
  vTaskDelay(10 / portTICK_PERIOD_MS);
  xSemaphoreGive(xSemaphoreI2C);

  LCDCommand_t cmdBuffer;

  while (1)
  {
    // wait for a command from the queue
    if (!xQueueReceive(xQueueLCD, &cmdBuffer, portMAX_DELAY)) continue;

    // take the semaphore to access the LCD
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
