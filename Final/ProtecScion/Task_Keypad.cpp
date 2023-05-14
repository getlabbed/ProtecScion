/**
 * Nom du fichier :
 *  @name Task_SoundSensor.cpp
 * Description :
 *  @brief Code permettant de gérer le capteur de son.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-12 @author Yanick Labelle - Entrée initiale du code.
 *  @date 2023-05-13 @author Olivier David Laplante @author Yanick Labelle - Résolution du problème de décalage des touches.
 */

#include "Task_Keypad.h"
#include <Keypad.h>

byte rowPins[KEYPAD_ROWS] = {PIN_KEYPAD_1, PIN_KEYPAD_2, PIN_KEYPAD_3, PIN_KEYPAD_4};
byte colPins[KEYPAD_COLS] = {PIN_KEYPAD_5, PIN_KEYPAD_6, PIN_KEYPAD_7};

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
