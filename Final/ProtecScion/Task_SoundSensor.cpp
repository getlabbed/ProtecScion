/**
 * @file Task_SoundSensor.cpp
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Code permettant de gérer le capteur de son.
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-06 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "Task_SoundSensor.h"

void vTaskSoundSensor(void *pvParameters)
{
	int iBuffer;
	float fSoundSensorVoltage;
	float fSoundSensorValue;
	String sTextLCD;
	while (true)
	{
		if (xQueueReceive(xQueueSound, &iBuffer, portMAX_DELAY) == pdFALSE) continue;

		fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / RESOLUTION * VOLTAGE_REFERENCE; // Récupérer le niveau sonore en dB
		fSoundSensorValue = fSoundSensorVoltage * 50.0;							             // conversion de la tension en décibels

		sTextLCD = "Son: " + String(fSoundSensorValue, 3) + "dB";                            // Formater les valeurs obtenues
		vSendLCDCommand(sTextLCD, 1, 0);                                                     // Envoyer les valeurs sur l'écran LCD
	}
}