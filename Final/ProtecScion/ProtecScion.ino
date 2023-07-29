/**
 * @file ProtecScion.ino
 * @author Skkeye
 * @author Skkeye's coleague
 * @brief The goal of this progrm is to improve the functionality of a table saw.
 * @note restricted to ESP32 Feather
 * @version 1.0
 * @date 2023-04-30 - Initial code entry
 * @date 2023-05-18 - Final code entry 
 * @note Doxygen used for documentation:
 *       https://www.doxygen.nl/manual/docblocks.html
 */

/// --------- INCLUDES --------- ///
#include "global.h"

/// --------- TASKS --------- ///
// Task includes
#include "Task_AsservissementScie.h"
#include "Task_IOFlash.h"
#include "Task_SoundSensor.h"
#include "Task_Apprentissage.h"
#include "Task_LCD.h"
#include "Task_DHT11.h"
#include "Task_Menu.h"
#include "Task_Keypad.h"
#include "Task_LED.h"

// Handle definitions
TaskHandle_t xTaskSawServo;
TaskHandle_t xTaskIOFlash;
TaskHandle_t xTaskSoundSensor;
TaskHandle_t xTaskLearning;
TaskHandle_t xTaskLCD;
TaskHandle_t xTaskDHT11;
TaskHandle_t xTaskMenu;
TaskHandle_t xTaskKeypad;
TaskHandle_t xTaskLED;

// Semaphore definitions
SemaphoreHandle_t xSemaphoreSerial;
SemaphoreHandle_t xSemaphoreSPI;
SemaphoreHandle_t xSemaphoreI2C;
SemaphoreHandle_t xSemaphoreLCDCommand;
SemaphoreHandle_t xSemaphoreLog;

// Queue definitions
QueueHandle_t xQueueReadWood;
QueueHandle_t xQueueWriteWood;
QueueHandle_t xQueueRequestWood;
QueueHandle_t xQueueLog;
QueueHandle_t xQueueSawSpeed;
QueueHandle_t xQueueLCD;
QueueHandle_t xQueueAmbiantHumidity;
QueueHandle_t xQueueAmbiantTemperature;
QueueHandle_t xQueueHeatIndex;
QueueHandle_t xQueueKeypad;
QueueHandle_t xQueueLearningControl;
QueueHandle_t xQueueSound;
QueueHandle_t xQueueAmbiant;
QueueHandle_t xQueueWoodTemp;
QueueHandle_t xQueueLED;
QueueHandle_t xQueueAverageFeedRate;

// I2C
TwoWire xWireBus = TwoWire(0);

/// --------- FUNCTIONS --------- ///

/**
 * @brief LCD screen printing simplification
 * @author Skkeye's coleague
 * 
 * @param message - Message to print
 * @param line - Line to print on
 * @param duration - Shown duration (0 = infini)
 */
void vSendLCDCommand(String message, unsigned int line, unsigned int duration)
{
  LCDCommand_t cmdBuffer = {message, line, duration};
  xQueueSend(xQueueLCD, &cmdBuffer, portMAX_DELAY);   // Send the message to write in the LCD queue
  vTaskDelay(pdMS_TO_TICKS(10));
  xSemaphoreTake(xSemaphoreLCDCommand, portMAX_DELAY); // Make sure the LCD screen is synchronized
}

/**
 * @brief Log command send simplification
 * @author Skkeye's coleague
 * 
 * @param level - Message level to log
 * @param message - Message to log
 */
void vSendLog(LogLevel_t level, String message)
{
  Log_t logBuffer = {level, message};
  xQueueSend(xQueueLog, &logBuffer, portMAX_DELAY); // Send the message to log in the corresponding queue
  xSemaphoreTake(xSemaphoreLog, portMAX_DELAY); // Make sure the log messages are synchronized
}
 
 /**
  * @brief Task creation
  *
  * @author Skkeye
  */
void vCreateAllTasks()
{
  xTaskCreatePinnedToCore(vTaskAsservissementScie, "SawServo", TASK_STACK_SIZE, NULL, TASK_SAWSERVO_PRIORITY, &xTaskSawServo, TASK_SAWSERVO_CORE);
  xTaskCreatePinnedToCore(vTaskIOFlash, "IOFlash", TASK_STACK_SIZE, NULL, TASK_IOFLASH_PRIORITY, &xTaskIOFlash, TASK_IOFLASH_CORE);
  xTaskCreatePinnedToCore(vTaskSoundSensor, "SoundSensor", TASK_STACK_SIZE, NULL, TASK_SOUNDSENSOR_PRIORITY, &xTaskSoundSensor, TASK_SOUNDSENSOR_CORE);
  xTaskCreatePinnedToCore(vTaskApprentissage, "Learning", TASK_STACK_SIZE, NULL, TASK_LEARNING_PRIORITY, &xTaskLearning, TASK_LEARNING_CORE);
  xTaskCreatePinnedToCore(vTaskLCD, "LCD", TASK_STACK_SIZE, NULL, TASK_LCD_PRIORITY, &xTaskLCD, TASK_LCD_CORE);
  xTaskCreatePinnedToCore(vTaskDHT11, "DHT11", TASK_STACK_SIZE, NULL, TASK_DHT11_PRIORITY, &xTaskDHT11, TASK_DHT11_CORE);
  xTaskCreatePinnedToCore(vTaskMenu, "Menu", TASK_STACK_SIZE, NULL, TASK_MENU_PRIORITY, &xTaskMenu, TASK_MENU_CORE);
  xTaskCreatePinnedToCore(vTaskKeypad, "Keypad", TASK_STACK_SIZE, NULL, TASK_KEYPAD_PRIORITY, &xTaskKeypad, TASK_KEYPAD_CORE);
  xTaskCreatePinnedToCore(vTaskLED, "LED", TASK_STACK_SIZE, NULL, TASK_LED_PRIORITY, &xTaskLED, TASK_LED_CORE);
}

/**
 * @brief Semaphore creation
 * 
 * @author Skkeye
 */
void vSetupSemaphores()
{
  xSemaphoreSerial = xSemaphoreCreateBinary();
  xSemaphoreSPI = xSemaphoreCreateBinary();
  xSemaphoreI2C = xSemaphoreCreateBinary();
  xSemaphoreLCDCommand = xSemaphoreCreateBinary();
  xSemaphoreLog = xSemaphoreCreateBinary();

  xSemaphoreGive(xSemaphoreSerial);
  xSemaphoreGive(xSemaphoreSPI);
  xSemaphoreGive(xSemaphoreI2C);
}

/**
 * @brief Queue creation
 * 
 * @author Skkeye
 */
void vSetupQueues()
{
  xQueueReadWood = xQueueCreate(1, sizeof(Wood_t));
  xQueueWriteWood = xQueueCreate(1, sizeof(Wood_t));
  xQueueLog = xQueueCreate(100, sizeof(Log_t));
  xQueueSawSpeed = xQueueCreate(1, sizeof(unsigned int));
  xQueueRequestWood = xQueueCreate(1, sizeof(unsigned int));
  xQueueLCD = xQueueCreate(10, sizeof(LCDCommand_t));
  xQueueAmbiantHumidity = xQueueCreate(1, sizeof(float));
  xQueueAmbiantTemperature = xQueueCreate(1, sizeof(float));
  xQueueKeypad = xQueueCreate(10, sizeof(char));
  xQueueLED = xQueueCreate(1, sizeof(LedState_t));
  xQueueLearningControl = xQueueCreate(1, sizeof(int));
  xQueueSound = xQueueCreate(1, sizeof(unsigned int));
  xQueueAmbiant = xQueueCreate(1, sizeof(unsigned int));
  xQueueWoodTemp = xQueueCreate(1, sizeof(unsigned int));
}

/// --------- SETUP & LOOP --------- ///

/**
 * @brief Program's init function
 * 
 * @author Skkeye
 */
void setup()
{
  // setup serial
  Serial.begin(115200);

  // setup SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // setup I2C
  xWireBus.begin(PIN_SDA, PIN_SCL);

  // setup semaphores
  vSetupSemaphores();

  // setup queues
  vSetupQueues();

  // setup tasks
  vCreateAllTasks();

  // disable watchdog
  esp_task_wdt_delete(NULL);
  esp_task_wdt_deinit();
}

/**
 * @brief Main loop function.
 * @note Not used, because the tasks are managed by the RTOS.
 * 
 */
void loop()
{
}