#include "Task_Keypad.h"
#include <Keypad.h>

byte rowPins[KEYPAD_ROWS] = {PIN_KEYPAD_1, PIN_KEYPAD_2, PIN_KEYPAD_3, PIN_KEYPAD_4};
byte colPins[KEYPAD_COLS] = {PIN_KEYPAD_5, PIN_KEYPAD_6, PIN_KEYPAD_7};

void vTaskKeypad(void *pvParameters)
{
	char key = NO_KEY;
	Keypad kp = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

	while (1)
	{
		while((key = kp.getKey()) == NO_KEY) vTaskDelay(pdMS_TO_TICKS(10));												 // Attendre pour une touche
		vTaskDelay(1);																 // Très important
		xQueueSend(xQueueKeypad, &key, portMAX_DELAY); // Envoyer la touche dans la file
	}
}
