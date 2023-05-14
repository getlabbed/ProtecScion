/**
 * Nom du fichier :
 *  @name Task_Menu.cpp
 * Description :
 *  @brief Code permettant de gérer une DEL.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-14 @author Olivier David Laplante - Entrée initiale du code.
 *  @date 2023-05-14 @author Yanick Labelle - Ajout du mode BLINK
 */

#include "Task_LED.h"

void vTaskLED(void *pvParameters) {
	LedState_t led_state = LED_OFF;

  while(true)
	{
    xQueueReceive(xQueueLED, &led_state, 0);
    pinMode(PIN_LED, (led_state == LED_OFF) ? INPUT : OUTPUT);
    digitalWrite(PIN_LED, led_state == LED_RED ? LOW : HIGH);
    vTaskDelay(pdMS_TO_TICKS(led_state == LED_BLINK ? 500 : 10));
    digitalWrite(PIN_LED, led_state == LED_GREEN ? HIGH : LOW);
    vTaskDelay(pdMS_TO_TICKS(led_state == LED_BLINK ? 500 : 10));
  }
}