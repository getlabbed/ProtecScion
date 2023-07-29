 /**
 * @file Task_SoundSensor.cpp
 * @author Skkeye's coleague
 * @brief Code used to manage the sound sensor.
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-12 - Initial code entry
 * @date 2023-05-18 - Final code entry 
 * 
 */

#include "Task_Keypad.h"
#include <Keypad.h>

byte rowPins[KEYPAD_ROWS] = {PIN_KEYPAD_1, PIN_KEYPAD_2, PIN_KEYPAD_3, PIN_KEYPAD_4};
byte colPins[KEYPAD_COLS] = {PIN_KEYPAD_5, PIN_KEYPAD_6, PIN_KEYPAD_7};

/**
 * @brief Task used to manage the keypad.
 * 
 * @param pvParameters - Not used
 */
void vTaskKeypad(void *pvParameters)
{
	char key = NO_KEY; // Init the key variable to no key
	Keypad kp = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

	while (1)
	{
		while((key = kp.getKey()) == NO_KEY) vTaskDelay(pdMS_TO_TICKS(10)); // Wait for a key to be pressed
		vTaskDelay(1);																                      // Very important 
		xQueueSend(xQueueKeypad, &key, portMAX_DELAY);                      // Send the key to the queue
	}
}
