
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