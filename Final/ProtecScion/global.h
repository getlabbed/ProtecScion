/**
 * @file global.h
 * @author Skkeye
 * @author Skkeye's coleague
 * @brief This file contains the global variables of the project, 
 * 				the priorities of the tasks and the configuration of FreeRTOS.
 * @version 1.0
 * @date 2023-03-16 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

// Prevents multiple inclusions
#ifndef GLOBAL_H
#define GLOBAL_H

// --------- INCLUDES --------- ///
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/timers.h>
#include <esp_task_wdt.h>
#include "SPIFFS.h"
#include <Wire.h>



/*/ ------------------ ALL PINS ------------------ ///
            *
            * GPIO 26 A0    : FBK Motor
            * GPIO 25 A1    : PWM Motor
INPUT ONLY  * GPIO 34 A2    : IR Distance Sensor
INPUT ONLY  * GPIO 39 A3    : Sound   | GPIO 13      : ""
INPUT ONLY  * GPIO 36 A4    : ""      | GPIO 12      : !!!NOCONNECT!!!
            * GPIO 4  A5    : KEYPAD  | GPIO 27      : LED
            * GPIO 5 SCK    : KEYPAD  | GPIO 33      : DHT11
            * GPIO 18 MOSI  : KEYPAD  | GPIO 15      : LCD DAT
            * GPIO 19 MISO  : KEYPAD  | GPIO 32      : LCD CLK
            * GPIO 16       : KEYPAD  | GPIO 14      : LCD LAT
            * GPIO 17       : KEYPAD  | GPIO 22 SCL  : Temp
            * GPIO 21       : KEYPAD  | GPIO 23 SDA  : Temp
*/// --------------------------------------------- ///

/// --------- CONFIGURATION --------- ///
// Tasks
#define TASK_STACK_SIZE 10000
// SawServo
#define TASK_SAWSERVO_PRIORITY 10
#define TASK_SAWSERVO_CORE 1 // The servo task occupies core 1 alone
// IOFlash
#define TASK_IOFLASH_PRIORITY 6
#define TASK_IOFLASH_CORE 0
// SoundSensor
#define TASK_SOUNDSENSOR_PRIORITY 3
#define TASK_SOUNDSENSOR_CORE 0
// Learning mode
#define TASK_LEARNING_PRIORITY 9
#define TASK_LEARNING_CORE 1
// LCD
#define TASK_LCD_PRIORITY 7
#define TASK_LCD_CORE 0
// DHT11
#define TASK_DHT11_PRIORITY 4
#define TASK_DHT11_CORE 0
// Menu
#define TASK_MENU_PRIORITY 5
#define TASK_MENU_CORE 0
// Keypad
#define TASK_KEYPAD_PRIORITY 5
#define TASK_KEYPAD_CORE 0
// LED
#define TASK_LED_PRIORITY 1
#define TASK_LED_CORE 0

// Log level
#define DEBUG 2 // 0: INFO, 1: WARNING, 2: ERROR, 3: OFF

/// --------- HANDLES --------- ///
/// --------- TASKS --------- ///
// SawServo
extern TaskHandle_t xTaskSawServo;
// IOFlash
extern TaskHandle_t xTaskIOFlash;
// SoundSensor
extern TaskHandle_t xTaskSoundSensor;
// Learning mode
extern TaskHandle_t xTaskLearning;
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
// SPI
extern SemaphoreHandle_t xSemaphoreSPI;
// I2C
extern SemaphoreHandle_t xSemaphoreI2C;
// LCD
extern SemaphoreHandle_t xSemaphoreLCDCommand;
// LOG
extern SemaphoreHandle_t xSemaphoreLog;

/// --------- FILES --------- ///
// Wood IO
extern QueueHandle_t xQueueReadWood;
extern QueueHandle_t xQueueWriteWood;
extern QueueHandle_t xQueueRequestWood;
// Log IO
extern QueueHandle_t xQueueLog;
// Saw Speed
extern QueueHandle_t xQueueSawSpeed;
// LCD
extern QueueHandle_t xQueueLCD;
// Keypad buttons
extern QueueHandle_t xQueueKeypad;
// Learning mode
extern QueueHandle_t xQueueLearningControl;
// Sound, Ambiant Temperature, Wood Temperature
extern QueueHandle_t xQueueSound;
extern QueueHandle_t xQueueAmbiant;
extern QueueHandle_t xQueueWoodTemp;
// LED
extern QueueHandle_t xQueueLED;

/// --------- I2C --------- ///
// Pins
#define PIN_SDA 23
#define PIN_SCL 22

// I2C
extern TwoWire xWireBus;

/// --------- TYPES --------- ///
// Data struct for wood
typedef struct __attribute__((packed))
{
	int code;			// wood code
	int sawSpeed; // Speed of the saw
	float feedRate; // Speed when feeding wood on the saw (mm/s)
} Wood_t;

// Log levels enum
typedef enum
{
	INFO,
	WARNING,
	ERROR,
	DUMP
} LogLevel_t;

// Log data struct
typedef struct __attribute__((packed))
{
	LogLevel_t level;
	String message;
} Log_t;

// LCD command data struct
typedef struct __attribute__((packed))
{
	String message; // maximum 20 char
	unsigned int line;
	unsigned int duration; // in ms, 0 for infinite
} LCDCommand_t;

// LED button data struct
typedef enum {LED_RED, LED_GREEN, LED_BOTH, LED_BLINK, LED_OFF} LedState_t;

/// Global functions ///

extern void vSendLCDCommand(String message, unsigned int line, unsigned int duration);

extern void vSendLog(LogLevel_t level, String message);

#endif // GLOBAL_H