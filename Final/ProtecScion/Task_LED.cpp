 /**
 * @file Task_LED.cpp
 * @author Skkeye
 * @brief Code used to manage a LED.
 * @note restrictions: For ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-14 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

#include "Task_LED.h"

/**
 * @brief Task used to manage a LED.
 * @note Depending on the mode of the LED, it can be off, on, blink in green or red.
 * 
 * @param pvParameters 
 */
void vTaskLED(void *pvParameters)
{
	LedState_t led_state = LED_OFF;

	while (true)
	{
		xQueueReceive(xQueueLED, &led_state, 0); // LEDs mode change
		pinMode(PIN_LED, (led_state == LED_OFF) ? INPUT : OUTPUT);    
		digitalWrite(PIN_LED, led_state == LED_RED ? LOW : HIGH);
		vTaskDelay(pdMS_TO_TICKS(led_state == LED_BLINK ? 500 : 10));
		digitalWrite(PIN_LED, led_state == LED_GREEN ? HIGH : LOW);
		vTaskDelay(pdMS_TO_TICKS(led_state == LED_BLINK ? 500 : 10));
	}
}