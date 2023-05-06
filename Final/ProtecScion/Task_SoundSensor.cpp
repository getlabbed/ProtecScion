#include "Task_SoundSensor.h"

void vTaskSoundSensor(void *pvParameters)
{
	float fSoundSensorVoltage;
	float fSoundSensorValue;
	while (true)
	{

		fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / RESOLUTION * VOLTAGE_REFERENCE; // Récupérer le niveau sonore en dB
		fSoundSensorValue = fSoundSensorVoltage * 50.0;																			 // conversion de la tension en décibels
		Serial.println(" Niveau sonore: " + String(fSoundSensorValue) + "dB");
	}
}