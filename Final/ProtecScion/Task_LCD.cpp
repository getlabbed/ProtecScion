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
  struct lineNumbers
  {
    unsigned int line;
  } line1, line2, line3, line4;
  line1.line = 0;
  line2.line = 1;
  line3.line = 2;
  line4.line = 3;

  // init timers
  xTimerLCDLine[0] = xTimerCreate("TimerLCDLine1", 1000 / portTICK_PERIOD_MS, pdFALSE, &line1, resetLine);
  xTimerLCDLine[1] = xTimerCreate("TimerLCDLine2", 1000 / portTICK_PERIOD_MS, pdFALSE, &line2, resetLine);
  xTimerLCDLine[2] = xTimerCreate("TimerLCDLine3", 1000 / portTICK_PERIOD_MS, pdFALSE, &line3, resetLine);
  xTimerLCDLine[3] = xTimerCreate("TimerLCDLine4", 1000 / portTICK_PERIOD_MS, pdFALSE, &line4, resetLine);

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
    if (cmdBuffer.duration == 0 || line != 3)
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
  if (xTimerIsTimerActive(xTimerLCDLine[3]) == pdTRUE)
  {
    xTimerStop(xTimerLCDLine[3], 0);
  }
  xTimerChangePeriod(xTimerLCDLine[3], duration / portTICK_PERIOD_MS, 0);
  xTimerStart(xTimerLCDLine[3], 0);
}

void resetLine(void *pvParameters)
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

