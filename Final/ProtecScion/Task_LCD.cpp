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

  // Connect via SPI. Data pin is #4, clock pin is #5 and latch pin is #6
  Adafruit_LiquidCrystal tempLCD(PIN_DATA, PIN_CLOCK, PIN_LATCH);
  // delay to let the lcd initialise
  vTaskDelay(100 / portTICK_PERIOD_MS);
  lcd = &tempLCD;

  lcd->begin(20, 4);
  lcd->clear();

  LCDCommand_t cmdBuffer;

  unsigned int line;

  // call dumpLog after 1 second
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Log_t log = {DUMP, "LCD: Initialisation terminée"};
  xQueueSend(xQueueLog, &log, 0);

	while (1)
	{
    vTaskDelay(100 / portTICK_PERIOD_MS);
		// wait for a command from the queue
    if (xQueueReceive(xQueueLCD, &cmdBuffer, 0) == pdFALSE) continue;

    // take the semaphore to access the LCD
    while (xSemaphoreTake(xSemaphoreLCD, 0) == pdFALSE) vTaskDelay(10 / portTICK_PERIOD_MS);

    // if the duration is 0, we don't need to set a timer
    // use something like this : u8Line = (u8Line == 1) ? 2 : (u8Line == 2) ? 1 : u8Line; to get the right line
    //line = (cmdBuffer.line == 1) ? 2 : (cmdBuffer.line == 2) ? 1 : cmdBuffer.line;
    line = cmdBuffer.line;
    if (cmdBuffer.duration == 0)
    {
      // set the buffer
      buffer.line[line] = cmdBuffer.message;
    }
    else
    {
      // set the timer
      setResetLine(line, cmdBuffer.duration);
    }
    // set the line on the lcd
    lcd->setCursor(0, line);
    lcd->print(cmdBuffer.message);
    xSemaphoreGive(xSemaphoreLCD);
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

void resetLine0(void *pvParameters)
{
  if (xSemaphoreTake(xSemaphoreLCD, portMAX_DELAY) == pdTRUE)
  {
    lcd->setCursor(0, 0);
    lcd->print("                    ");
    lcd->setCursor(0, 0);
    lcd->print(buffer.line[0]);
    xSemaphoreGive(xSemaphoreLCD);
  }
}

void resetLine1(void *pvParameters)
{
  if (xSemaphoreTake(xSemaphoreLCD, portMAX_DELAY) == pdTRUE)
  {
    lcd->setCursor(0, 1);
    lcd->print("                    ");
    lcd->setCursor(0, 1);
    lcd->print(buffer.line[1]);
    xSemaphoreGive(xSemaphoreLCD);
  }
}

void resetLine2(void *pvParameters)
{
  if (xSemaphoreTake(xSemaphoreLCD, portMAX_DELAY) == pdTRUE)
  {
    lcd->setCursor(0, 2);
    lcd->print("                    ");
    lcd->setCursor(0, 2);
    lcd->print(buffer.line[2]);
    xSemaphoreGive(xSemaphoreLCD);
  }
}

void resetLine3(void *pvParameters)
{
  if (xSemaphoreTake(xSemaphoreLCD, portMAX_DELAY) == pdTRUE)
  {
    lcd->setCursor(0, 3);
    lcd->print("                    ");
    lcd->setCursor(0, 3);
    lcd->print(buffer.line[3]);
    xSemaphoreGive(xSemaphoreLCD);
  }
}
