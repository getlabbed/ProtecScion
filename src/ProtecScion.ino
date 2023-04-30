#include "global.h"

vTaskSoundSensor(void *pvParameters) {
    fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / RESOLUTION * VOLTAGE_REFERENCE;   // Récupérer le niveau sonore en dB
    fSoundSensorValue = fSoundSensorVoltage * 50.0;                                        // conversion de la tension en décibels
    Serial.println(" Niveau sonore: " + String(fSoundSensorValue) + "dB");
}

void setup()
{
	// Initialisation de la communication sérielle
	Serial.begin(115200);

	// Priorité 1 - Coeur 0 - Taille de pile 2048 - Lecture de la vitesse de la scie
	xTaskCreatePinnedToCore(vTaskSoundSensor, "Capteur de son", TASK_STACK_SIZE, NULL, 1, &HandleSoundSensor, 0);	
}

void loop()
{
	// Ne rien faire
}