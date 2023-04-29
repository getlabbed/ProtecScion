#include "global"

void setup()
{
	// Initialisation de la communication sérielle
	Serial.begin(115200);

	// Priorité 1 - Coeur 0 - Taille de pile 2048 - Lecture de la vitesse de la scie
	xTaskCreatePinnedToCore(vTaskSoundSensor, "Capteur de son", TASK_STACK_SIZE, NULL, 1, &HandleKeypad, 0);	
}

void loop()
{
	// Ne rien faire
}