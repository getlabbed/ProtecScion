 /**
 * @file Task_LED.cpp
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @brief Code permettant de gérer une DEL.
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-14 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "Task_LED.h"

/**
 * @brief Tâche permettant de gérer une DEL.
 * @note Selon le mode de la DEL, elle peut être éteinte, allumée, clignoter en vert ou en rouge.
 * 
 * @param pvParameters 
 */
void vTaskLED(void *pvParameters)
{
	LedState_t led_state = LED_OFF;

	while (true)
	{
		xQueueReceive(xQueueLED, &led_state, 0); // Changement de mode des DELs
		pinMode(PIN_LED, (led_state == LED_OFF) ? INPUT : OUTPUT);    
		digitalWrite(PIN_LED, led_state == LED_RED ? LOW : HIGH);
		vTaskDelay(pdMS_TO_TICKS(led_state == LED_BLINK ? 500 : 10));
		digitalWrite(PIN_LED, led_state == LED_GREEN ? HIGH : LOW);
		vTaskDelay(pdMS_TO_TICKS(led_state == LED_BLINK ? 500 : 10));
	}
}