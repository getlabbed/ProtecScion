/**
 * @file ProtecScion.ino
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Programme permettant d'améliorer la fonctionnalité d'un banc de scie.
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-04-30 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * @note Utilisation de Doxygen pour la documentation:
 *       https://www.doxygen.nl/manual/docblocks.html
 */

/// --------- INCLUDES --------- ///
#include "global.h"

/// --------- TASKS --------- ///
// Inclusion des tâches
#include "Task_AsservissementScie.h"
#include "Task_IOFlash.h"
#include "Task_SoundSensor.h"
#include "Task_Apprentissage.h"
#include "Task_LCD.h"
#include "Task_DHT11.h"
#include "Task_Menu.h"
#include "Task_Keypad.h"
#include "Task_LED.h"

// Définition des handles
TaskHandle_t xTaskAsservissementScie;
TaskHandle_t xTaskIOFlash;
TaskHandle_t xTaskSoundSensor;
TaskHandle_t xTaskApprentissage;
TaskHandle_t xTaskLCD;
TaskHandle_t xTaskDHT11;
TaskHandle_t xTaskMenu;
TaskHandle_t xTaskKeypad;
TaskHandle_t xTaskLED;

// Définition des sémaphores
SemaphoreHandle_t xSemaphoreSerial;
SemaphoreHandle_t xSemaphoreSPI;
SemaphoreHandle_t xSemaphoreI2C;
SemaphoreHandle_t xSemaphoreLCDCommand;
SemaphoreHandle_t xSemaphoreLog;

// Définition des files
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
QueueHandle_t xQueueApprentissageControl;
QueueHandle_t xQueueSound;
QueueHandle_t xQueueAmbiant;
QueueHandle_t xQueueWoodTemp;
QueueHandle_t xQueueLED;
QueueHandle_t xQueueAverageFeedRate;

// I2C
TwoWire xWireBus = TwoWire(0);

/// --------- FONCTIONS --------- ///

/**
 * @brief Simplifier l'impression sur l'écran LCD
 * @author Yanick Labelle (getlabbed@proton.me)
 * 
 * @param message - Message à afficher
 * @param line - Ligne à afficher
 * @param duration - Durée d'affichage (0 = infini)
 */
void vSendLCDCommand(String message, unsigned int line, unsigned int duration)
{
  LCDCommand_t cmdBuffer = {message, line, duration};
  xQueueSend(xQueueLCD, &cmdBuffer, portMAX_DELAY);   // Envoyer le message à écrire dans la file de l'écran LCD
  vTaskDelay(pdMS_TO_TICKS(10));
  xSemaphoreTake(xSemaphoreLCDCommand, portMAX_DELAY); // Assurer une synchronisation de l'écran LCD
}

/**
 * @brief Simplifier l'écriture de la commande de log
 * @author Yanick Labelle (getlabbed@proton.me)
 * 
 * @param level - Niveau du message à journaliser
 * @param message - Message à journaliser
 */
void vSendLog(LogLevel_t level, String message)
{
  Log_t logBuffer = {level, message};
  xQueueSend(xQueueLog, &logBuffer, portMAX_DELAY); // Envoyer le message à journaliser dans la file correspondante
  xSemaphoreTake(xSemaphoreLog, portMAX_DELAY); // Assurer une synchronisation des messages de journalisation
}
 
 /**
  * @brief Création des tâches 
  *
  * @author Olivier David Laplante
  */
void vCreateAllTasks()
{
  xTaskCreatePinnedToCore(vTaskAsservissementScie, "AsservissementScie", TASK_STACK_SIZE, NULL, TASK_ASSERVISSEMENTSCIE_PRIORITY, &xTaskAsservissementScie, TASK_ASSERVISSEMENTSCIE_CORE);
  xTaskCreatePinnedToCore(vTaskIOFlash, "IOFlash", TASK_STACK_SIZE, NULL, TASK_IOFLASH_PRIORITY, &xTaskIOFlash, TASK_IOFLASH_CORE);
  xTaskCreatePinnedToCore(vTaskSoundSensor, "SoundSensor", TASK_STACK_SIZE, NULL, TASK_SOUNDSENSOR_PRIORITY, &xTaskSoundSensor, TASK_SOUNDSENSOR_CORE);
  xTaskCreatePinnedToCore(vTaskApprentissage, "Apprentissage", TASK_STACK_SIZE, NULL, TASK_APPRENTISSAGE_PRIORITY, &xTaskApprentissage, TASK_APPRENTISSAGE_CORE);
  xTaskCreatePinnedToCore(vTaskLCD, "LCD", TASK_STACK_SIZE, NULL, TASK_LCD_PRIORITY, &xTaskLCD, TASK_LCD_CORE);
  xTaskCreatePinnedToCore(vTaskDHT11, "DHT11", TASK_STACK_SIZE, NULL, TASK_DHT11_PRIORITY, &xTaskDHT11, TASK_DHT11_CORE);
  xTaskCreatePinnedToCore(vTaskMenu, "Menu", TASK_STACK_SIZE, NULL, TASK_MENU_PRIORITY, &xTaskMenu, TASK_MENU_CORE);
  xTaskCreatePinnedToCore(vTaskKeypad, "Keypad", TASK_STACK_SIZE, NULL, TASK_KEYPAD_PRIORITY, &xTaskKeypad, TASK_KEYPAD_CORE);
  xTaskCreatePinnedToCore(vTaskLED, "LED", TASK_STACK_SIZE, NULL, TASK_LED_PRIORITY, &xTaskLED, TASK_LED_CORE);
}

/**
 * @brief Création des sémaphores
 * 
 * @author Olivier David Laplante
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
 * @brief Création des files
 * 
 * @author Olivier David Laplante
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
  xQueueApprentissageControl = xQueueCreate(1, sizeof(int));
  xQueueSound = xQueueCreate(1, sizeof(unsigned int));
  xQueueAmbiant = xQueueCreate(1, sizeof(unsigned int));
  xQueueWoodTemp = xQueueCreate(1, sizeof(unsigned int));
}

/// --------- SETUP & LOOP --------- ///

/**
 * @brief Fonction d'initialisation du programme.
 * 
 * @author Olivier David Laplante
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
 * @brief Fonction de boucle principale du programme.
 * @note Non utilisée, car les tâches sont gérées par le RTOS.
 * 
 */
void loop()
{
}