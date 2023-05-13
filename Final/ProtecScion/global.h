
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
INPUT ONLY *	GPIO 34 A2		:		IR Distance
INPUT ONLY *	GPIO 39 A3		:		Sound					|		GPIO 13				:		""
INPUT ONLY *	GPIO 36 A4		:		""				|		GPIO 12				:		!!!NOCONNECT!!!
*	GPIO 4  A5		:		KEYPAD				|		GPIO 27				:		LED
*	GPIO 5 SCK		:		KEYPAD				|		GPIO 33				:		DHT11
*	GPIO 18 MOSI	:		KEYPAD				|		GPIO 15				:		LCD DAT
*	GPIO 19 MISO	:		KEYPAD				|		GPIO 32				:		LCD CLK
*	GPIO 16				:		KEYPAD				|		GPIO 14				:		LCD LAT
*	GPIO 17				:		KEYPAD				|		GPIO 22 SCL		:		Temp
*	GPIO 21				:		KEYPAD					|		GPIO 23 SDA		:		Temp
*/
// ---------------------------- ///

/// --------- CONFIGURATION --------- ///
// Tâches
#define TASK_STACK_SIZE 10000
// AsservissementScie
#define TASK_ASSERVISSEMENTSCIE_PRIORITY 1
#define TASK_ASSERVISSEMENTSCIE_CORE 0 // La tâche d'asservissement occupe à elle seule le coeur 0
// IOFlash
#define TASK_IOFLASH_PRIORITY 5
#define TASK_IOFLASH_CORE 1
// SoundSensor
#define TASK_SOUNDSENSOR_PRIORITY 3
#define TASK_SOUNDSENSOR_CORE 1
// SonarSensor
#define TASK_IRSENSOR_PRIORITY 2
#define TASK_IRSENSOR_CORE 1
// LCD
#define TASK_LCD_PRIORITY 5
#define TASK_LCD_CORE 1
// DHT11
#define TASK_DHT11_PRIORITY 4
#define TASK_DHT11_CORE 1
// Menu
#define TASK_MENU_PRIORITY 6
#define TASK_MENU_CORE 1
// Keypad
#define TASK_KEYPAD_PRIORITY 6
#define TASK_KEYPAD_CORE 1
// LED
#define TASK_LED_PRIORITY 1
#define TASK_LED_CORE 1

// Niveaux de log
#define DEBUG 2 // 0: INFO, 1: WARNING, 2: ERROR, 3: OFF

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
// DHT11
extern TaskHandle_t xTaskDHT11;
// Menu
extern TaskHandle_t xTaskMenu;
// Keypad
extern TaskHandle_t xTaskKeypad;
// LED
extern TaskHandle_t xTaskLED;

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
//Capteur DHT11
extern QueueHandle_t xQueueAmbiantHumidity;
extern QueueHandle_t xQueueAmbiantTemperature;
extern QueueHandle_t xQueueHeatIndex;
// Boutons du keypad
extern QueueHandle_t xQueueKeypad;
// IR Distance
extern QueueHandle_t xQueueIRDistance;
// LED
extern QueueHandle_t xQueueLED;

/// --------- TYPES --------- ///
// Structure de données pour le bois
typedef struct __attribute__((packed))
{
	String name;	// nom du bois
	int code;			// code du bois
	int sawSpeed; // vitesse de la lame
	float feedRate; // vitesse d'avancement
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
typedef struct //__attribute__((packed))
{
	String message; // max 20 caractères
	unsigned int line;
	unsigned int duration; // en ms, 0 pour infini
} LCDCommand_t;

// Structure de données pour les boutons de la LED
typedef enum {LED_RED, LED_GREEN, LED_BOTH, LED_BLINK, LED_OFF} LedState_t;

/// FONCTIONS GLOBALES ///

extern void vSendLCDCommand(String message, unsigned int line, unsigned int duration);

extern void vSendLog(LogLevel_t level, String message);

#endif // GLOBAL_H