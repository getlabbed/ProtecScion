#include "Task_LCD.h"
#include "Adafruit_LiquidCrystal.h"

// Global variables
TimerHandle_t xTimerLCDLine1;
TimerHandle_t xTimerLCDLine2;
TimerHandle_t xTimerLCDLine3;
TimerHandle_t xTimerLCDLine4;

LCDBuffer_t buffer;

void vTaskLCD(void *pvParameters)
{
  // init timers
  xTimerLCDLine1 = xTimerCreate("TimerLCDLine1", 1000 / portTICK_PERIOD_MS, pdFALSE, (void *)1, resetLine);
  xTimerLCDLine2 = xTimerCreate("TimerLCDLine2", 1000 / portTICK_PERIOD_MS, pdFALSE, (void *)2, resetLine);
  xTimerLCDLine3 = xTimerCreate("TimerLCDLine3", 1000 / portTICK_PERIOD_MS, pdFALSE, (void *)3, resetLine);
  xTimerLCDLine4 = xTimerCreate("TimerLCDLine4", 1000 / portTICK_PERIOD_MS, pdFALSE, (void *)4, resetLine);

  // Connect via SPI. Data pin is #3, Clock is #2 and Latch is #4
  Adafruit_LiquidCrystal lcd(27, 33, 15);
  lcd.begin(20, 4);
  lcd.clear();

  LCDCommand_t cmdBuffer;

  unsigned int line;

	while (1)
	{
    vTaskDelay(100 / portTICK_PERIOD_MS);
		// wait for a command from the queue
    if (xQueueReceive(xQueueLCD, &cmdBuffer, 0) == pdFALSE) continue;

    // take the semaphore to access the LCD
    while (xSemaphoreTake(xSemaphoreLCD, 0) == pdFALSE) vTaskDelay(10 / portTICK_PERIOD_MS);

    // if the duration is 0, we don't need to set a timer
    // use something like this : u8Line = (u8Line == 1) ? 2 : (u8Line == 2) ? 1 : u8Line; to get the right line
    line = (cmdBuffer.line == 1) ? 2 : (cmdBuffer.line == 2) ? 1 : cmdBuffer.line;
    if (cmdBuffer.duration == 0)
    {
      // set the buffer
    }
    else
    {
      // set the timer
      setResetLine(line, cmdBuffer.duration);
    }
    // set the line on the lcd
    lcd.setCursor(0, line);
    lcd.print(cmdBuffer.text);
	}
}

void setResetLine(unsigned int line, unsigned int duration)
{
  // set the right timer to the duration
  switch (line)
  {
  case 1:
    if (xTimerIsTimerActive(xTimerLCDLine1) == pdTRUE)
    {
      xTimerStop(xTimerLCDLine1, 0);
    }
    xTimerChangePeriod(xTimerLCDLine1, duration / portTICK_PERIOD_MS, 0);
    xTimerStart(xTimerLCDLine1, 0);
    break;
  case 2:
    if (xTimerIsTimerActive(xTimerLCDLine2) == pdTRUE)
    {
      xTimerStop(xTimerLCDLine2, 0);
    }
    xTimerChangePeriod(xTimerLCDLine2, duration / portTICK_PERIOD_MS, 0);
    xTimerStart(xTimerLCDLine2, 0);
    break;
  case 3:
    if (xTimerIsTimerActive(xTimerLCDLine3) == pdTRUE)
    {
      xTimerStop(xTimerLCDLine3, 0);
    }
    xTimerChangePeriod(xTimerLCDLine3, duration / portTICK_PERIOD_MS, 0);
    xTimerStart(xTimerLCDLine3, 0);
    break;
  case 4:
    if (xTimerIsTimerActive(xTimerLCDLine4) == pdTRUE)
    {
      xTimerStop(xTimerLCDLine4, 0);
    }
    xTimerChangePeriod(xTimerLCDLine4, duration / portTICK_PERIOD_MS, 0);
    xTimerStart(xTimerLCDLine4, 0);
    break;
  default:
    break;
  }
}

void resetLine(void *pvParameters)
{
  // reset the line indicated in the pvParameters using the LCDBuffer_t struct
  unsigned int line = (unsigned int)pvParameters;
  if (xSemaphoreTake(xSemaphoreLCD, portMAX_DELAY) == pdTRUE)
  {
    switch (line)
    {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print(buffer.line1);
      break;
    case 2:
      lcd.setCursor(0, 2);
      lcd.print(buffer.line2);
      break;
    case 3:
      lcd.setCursor(0, 1);
      lcd.print(buffer.line3);
      break;
    case 4:
      lcd.setCursor(0, 3);
      lcd.print(buffer.line4);
      break;
    default:
      break;
    }
    xSemaphoreGive(xSemaphoreLCD);
  }
}