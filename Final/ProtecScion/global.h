
#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/timers.h>
#include <esp_task_wdt.h>
#include "SPIFFS.h"

/*/ ------------------ ALL PINS ------------------ ///
*
*	GPIO 26 A0		:		FBK Moteur
*	GPIO 25 A1		:		PWM Moteur
*	GPIO 34 A2		:		IR Distance
*	GPIO 39 A3		:		Sound					|		GPIO 13				:		LCD
*	GPIO 36 A4		:		KEYPAD				|		GPIO 12				:		LCD
*	GPIO 4  A5		:		KEYPAD				|		GPIO 27				:		LCD
*	GPIO 5 SCK		:		KEYPAD				|		GPIO 33				:		DHT11
*	GPIO 18 MOSI	:		KEYPAD				|		GPIO 15				:		""
*	GPIO 19 MISO	:		KEYPAD				|		GPIO 32				:		""
*	GPIO 16				:		KEYPAD				|		GPIO 14				:		""
*	GPIO 17				:		KEYPAD				|		GPIO 22 SCL		:		Temp
*	GPIO 21				:		KEYPAD				|		GPIO 23 SDA		:		Temp
*/
// ---------------------------- ///

/// --------- CONFIGURATION --------- ///
// Tâches
#define TASK_STACK_SIZE 10000
// AsservissementScie
#define TASK_ASSERVISSEMENTSCIE_PRIORITY 1
#define TASK_ASSERVISSEMENTSCIE_CORE 0 // La tâche d'asservissement occupe à elle seule le coeur 0
// IOFlash
#define TASK_IOFLASH_PRIORITY 4
#define TASK_IOFLASH_CORE 1
// SoundSensor
#define TASK_SOUNDSENSOR_PRIORITY 1
#define TASK_SOUNDSENSOR_CORE 1
// SonarSensor
#define TASK_IRSENSOR_PRIORITY 2
#define TASK_IRSENSOR_CORE 1
// LCD
#define TASK_LCD_PRIORITY 3
#define TASK_LCD_CORE 1

// Niveaux de log
#define DEBUG 1 // 0: INFO, 1: WARNING, 2: ERROR, 3: DUMP

/// --------- HANDLES --------- ///
/// --------- TACHES --------- ///
// AsservissementScie
extern TaskHandle_t xTaskAsservissementScie;
// IOFlash
extern TaskHandle_t xTaskIOFlash;
// SoundSensor
extern TaskHandle_t xTaskSoundSensor;
// SonarSensor
extern TaskHandle_t xTaskIRSensor;
// LCD
extern TaskHandle_t xTaskLCD;

/// --------- SEMAPHORES --------- ///
// Serial
extern SemaphoreHandle_t xSemaphoreSerial;
// SPIFFS
extern SemaphoreHandle_t xSemaphoreSPIFFS;
// LCD
extern SemaphoreHandle_t xSemaphoreLCD;

/// --------- FILES --------- ///
// IO de bois
extern QueueHandle_t xQueueReadWood;
extern QueueHandle_t xQueueWriteWood;
extern QueueHandle_t xQueueRequestWood;
// IO de log
extern QueueHandle_t xQueueLog;
// Vitesse de la scie
extern QueueHandle_t xQueueSawSpeed;
// LCD
extern QueueHandle_t xQueueLCD;

/// --------- TYPES --------- ///
// Structure de données pour le bois
typedef struct __attribute__((packed))
{
	String name;	// nom du bois
	int code;			// code du bois
	int sawSpeed; // vitesse de la lame
	int feedRate; // vitesse d'avancement
} Wood_t;

// Enumération pour les niveaux de log
typedef enum
{
	INFO,
	WARNING,
	ERROR,
	DUMP
} LogLevel_t;

// Structure de données pour les logs
typedef struct __attribute__((packed))
{
	LogLevel_t level;
	String message;
} Log_t;

// Structure de données pour une commande LCD
typedef struct __attribute__((packed))
{
	String message; // max 20 caractères
	unsigned int line;
	unsigned int duration; // en ms, 0 pour infini
} LCDCommand_t;

#endif // GLOBAL_H