/**
 * @file Task_SoundSensor.cpp
 * @author Skkeye's coleague
 * @brief Program used to manage the sound sensor.
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-06 - Initial code entry
 * @date 2023-05-18 - Final code entry 
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

		fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / RESOLUTION * VOLTAGE_REFERENCE; // Get the sound level in dB
		fSoundSensorValue = fSoundSensorVoltage * 50.0;                                      // Voltages conversion to decibels

		sTextLCD = "Son: " + String(fSoundSensorValue, 3) + "dB";                            // Format the values obtained
		vSendLCDCommand(sTextLCD, 1, 0);                                                     // Send the values to the LCD screen
	}
}