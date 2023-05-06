

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <esp_task_wdt.h>
#include "SPIFFS.h"

/// --------- CONFIGURATION --------- ///
// Tâches
#define TASK_STACK_SIZE 10000
// AsservissementScie
#define TASK_ASSERVISSEMENTSCIE_PRIORITY 1
#define TASK_ASSERVISSEMENTSCIE_CORE 0
// IOFlash
#define TASK_IOFLASH_PRIORITY 1
#define TASK_IOFLASH_CORE 1
// SoundSensor
#define TASK_SOUNDSENSOR_PRIORITY 3
#define TASK_SOUNDSENSOR_CORE 1

/// --------- HANDLES --------- ///
/// --------- TACHES --------- ///
// AsservissementScie
extern TaskHandle_t xTaskAsservissementScie;
// IOFlash
extern TaskHandle_t xTaskIOFlash;
// SoundSensor
extern TaskHandle_t xTaskSoundSensor;

/// --------- SEMAPHORES --------- ///
// Serial
extern SemaphoreHandle_t xSemaphoreSerial;
// SPIFFS
extern SemaphoreHandle_t xSemaphoreSPIFFS;

/// --------- FILES --------- ///
// IO de bois
extern QueueHandle_t xQueueReadWood;
extern QueueHandle_t xQueueWriteWood;
// IO de log
extern QueueHandle_t xQueueLog;
// Vitesse de la scie
extern QueueHandle_t xQueueSawSpeed;

/// --------- TYPES --------- ///
// Structure de données pour le bois
typedef struct
{
	String name; // nom du bois
	int code;			 // code du bois
	int sawSpeed;	 // vitesse de la lame
	int feedRate;	 // vitesse d'avancement
} Wood_t;

// Enumération pour les niveaux de log
typedef enum {INFO, WARNING, ERROR} LogLevel_t;

// Structure de données pour les logs
typedef struct
{
  LogLevel_t level;
  String message;
} Log_t;