/**
 * Nom du fichier :
 *  @name ProtecScion.ino
 * Description :
 *  @brief Programme permettant d'améliorer la fonctionnalité d'un banc de scie.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-04-30 @author Olivier David Laplante @author Yanick Labelle - Entrée initiale du code.
 *  @note Utilisation de Doxygen pour la documentation:
 *         https://www.doxygen.nl/manual/docblocks.html
 */

/// --------- INCLUDES --------- ///
#include "global.h"

/// --------- TASKS --------- ///
// include tasks
#include "Task_AsservissementScie.h"
#include "Task_IOFlash.h"
#include "Task_SoundSensor.h"
#include "Task_IRSensor.h"
#include "Task_LCD.h"
#include "Task_DHT11.h"

// define handles
TaskHandle_t xTaskAsservissementScie;
TaskHandle_t xTaskIOFlash;
TaskHandle_t xTaskSoundSensor;
TaskHandle_t xTaskIRSensor;
TaskHandle_t xTaskLCD;
TaskHandle_t xTaskDHT11;

// define semaphores
SemaphoreHandle_t xSemaphoreSerial;
SemaphoreHandle_t xSemaphoreSPIFFS;
SemaphoreHandle_t xSemaphoreLCD;

// define queues
QueueHandle_t xQueueReadWood;
QueueHandle_t xQueueWriteWood;
QueueHandle_t xQueueRequestWood;
QueueHandle_t xQueueLog;
QueueHandle_t xQueueSawSpeed;
QueueHandle_t xQueueLCD;
QueueHandle_t xQueueAmbiantHumidity;
QueueHandle_t xQueueAmbiantTemperature;
QueueHandle_t xQueueHeatIndex;

/// --------- FONCTIONS --------- ///

/** 
 * @fn vSendLCDCommand
 * @brief Simplifier l'écriture de la commande d'écriture sur l'écran ACL
 * 
 * @author Yanick Labelle @date 09-05-2023
 */
void vSendLCDCommand( String message, unsigned int line, unsigned int duration)
{
    LCDCommand_t cmdBuffer = {message, line, duration};
    xQueueSend(xQueueLCD, &cmdBuffer, portMAX_DELAY);
}

/** 
 * @fn vSendLCDCommand
 * @brief Simplifier l'écriture de la commande de log
 * 
 * @author Yanick Labelle @date 09-05-2023
 */
void vSendLog(LogLevel_t level, String message)
{
    Log_t logBuffer = {level, message};
    xQueueSend(xQueueLog, &logBuffer, portMAX_DELAY);
}


/** 
 * @fn vCreateAllTasks
 * @brief Création des tâches
 * 
 * @author Olivier David Laplante @date 30-04-2023
 */
void vCreateAllTasks() {
  xTaskCreatePinnedToCore(vTaskAsservissementScie, "AsservissementScie", TASK_STACK_SIZE, NULL, TASK_ASSERVISSEMENTSCIE_PRIORITY, &xTaskAsservissementScie, TASK_ASSERVISSEMENTSCIE_CORE);
  xTaskCreatePinnedToCore(vTaskIOFlash, "IOFlash", TASK_STACK_SIZE, NULL, TASK_IOFLASH_PRIORITY, &xTaskIOFlash, TASK_IOFLASH_CORE);
  xTaskCreatePinnedToCore(vTaskSoundSensor, "SoundSensor", TASK_STACK_SIZE, NULL, TASK_SOUNDSENSOR_PRIORITY, &xTaskSoundSensor, TASK_SOUNDSENSOR_CORE);
  xTaskCreatePinnedToCore(vTaskIRSensor, "IRSensor", TASK_STACK_SIZE, NULL, TASK_IRSENSOR_PRIORITY, &xTaskIRSensor, TASK_IRSENSOR_CORE);
  xTaskCreatePinnedToCore(vTaskLCD, "LCD", TASK_STACK_SIZE, NULL, TASK_LCD_PRIORITY, &xTaskLCD, TASK_LCD_CORE);
  xTaskCreatePinnedToCore(vTaskDHT11, "DHT11", TASK_STACK_SIZE, NULL, TASK_DHT11_PRIORITY, &xTaskDHT11, TASK_DHT11_CORE);
}

/** 
 * @fn vSetupSemaphores
 * @brief Création des sémaphores
 * 
 * @author Olivier David Laplante @date 06-05-2023
 */
void vSetupSemaphores() {
  xSemaphoreSerial = xSemaphoreCreateBinary();
  xSemaphoreSPIFFS = xSemaphoreCreateBinary();
  xSemaphoreLCD = xSemaphoreCreateBinary();

  xSemaphoreGive(xSemaphoreSerial);
  xSemaphoreGive(xSemaphoreSPIFFS);
  xSemaphoreGive(xSemaphoreLCD);
}

/** 
 * @fn vSetupQueues
 * @brief Création des files
 * 
 * @author Olivier David Laplante @date 06-05-2023
 */
void vSetupQueues() {
  xQueueReadWood = xQueueCreate(1, sizeof(Wood_t));
  xQueueWriteWood = xQueueCreate(1, sizeof(Wood_t));
  xQueueLog = xQueueCreate(100, sizeof(Log_t));
  xQueueSawSpeed = xQueueCreate(1, sizeof(unsigned int));
  xQueueRequestWood = xQueueCreate(1, sizeof(unsigned int));
  xQueueLCD = xQueueCreate(10, sizeof(LCDCommand_t));
  xQueueAmbiantHumidity = xQueueCreate(1, sizeof(float));
  xQueueAmbiantTemperature = xQueueCreate(1, sizeof(float));
  xQueueHeatIndex = xQueueCreate(1, sizeof(float));
}

/// --------- SETUP & LOOP --------- ///
/**
 * Nom de la fonction :
 *  @name setup
 * Description de la fonction :
 *  @brief Fonction d'initialisation du programme.
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note Aucune
 *
 * Historique :
 *  @date 2023-04-30 @author Olivier David Laplante - Entrée initiale du code.
 */
void setup() {
  // setup serial
  Serial.begin(115200);
  
  // setup SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

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
 * Nom de la fonction :
 *  @name loop
 * Description de la fonction :
 *  @brief Fonction de boucle principale du programme.
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note Aucune
 *
 * Historique :
 *  @date 2023-04-30 Olivier David Laplante - Entrée initiale du code.
 */
void loop() {
}