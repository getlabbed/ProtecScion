/**
 * Nom du fichier :
 *  @name Task_SoundSensor.cpp
 * Description :
 *  @brief Code permettant de gérer le capteur de son.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-??-?? @author Yanick Labelle - Entrée initiale du code.
 */

#include "Task_SoundSensor.h"

void vTaskSoundSensor(void *pvParameters)
{
	float fSoundSensorVoltage;
	float fSoundSensorValue;
	while (true)
	{

		fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / RESOLUTION * VOLTAGE_REFERENCE; // Récupérer le niveau sonore en dB
		fSoundSensorValue = fSoundSensorVoltage * 50.0;																			 // conversion de la tension en décibels
		//Serial.println(" Niveau sonore: " + String(fSoundSensorValue) + "dB");

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}