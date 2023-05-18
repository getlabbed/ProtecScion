 /**
 * @file Task_SoundSensor.cpp
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Code permettant de gérer le capteur de son.
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-12 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "Task_Keypad.h"
#include <Keypad.h>

byte rowPins[KEYPAD_ROWS] = {PIN_KEYPAD_1, PIN_KEYPAD_2, PIN_KEYPAD_3, PIN_KEYPAD_4};
byte colPins[KEYPAD_COLS] = {PIN_KEYPAD_5, PIN_KEYPAD_6, PIN_KEYPAD_7};

/**
 * @brief Tâche permettant de gérer le clavier matriciel.
 * 
 * @param pvParameters - Non utilisé
 */
void vTaskKeypad(void *pvParameters)
{
	char key = NO_KEY; // Initialiser la variable de la touche à aucune touche
	Keypad kp = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

	while (1)
	{
		while((key = kp.getKey()) == NO_KEY) vTaskDelay(pdMS_TO_TICKS(10)); // Attendre pour une touche
		vTaskDelay(1);																                      // Très important
		xQueueSend(xQueueKeypad, &key, portMAX_DELAY);                      // Envoyer la touche dans la file
	}
}
