 /**
 * @file Task_DHT11.cpp
 * @author Skkeye's coleague
 * @brief Program used to interface a DHT11 sensor.
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-08 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

// Code inspired by https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino

#include "Task_DHT11.h"
#include "DHT.h"

void vTaskDHT11(void *pvParameters)
{
	int iBuffer;
	float fHumidity;
	float fTemperature;
	String sTextLCD;
	DHT dht(PIN_DHT11, DHT11);
	dht.begin();
	while (true)
	{
		if (xQueueReceive(xQueueAmbiant, &iBuffer, portMAX_DELAY) == pdFALSE) continue;

		fHumidity = dht.readHumidity();
		fTemperature = dht.readTemperature();

		if (isnan(fHumidity) || isnan(fTemperature))
		{
			//vSendLog(ERROR, "DHT11: ERROR            ");
			continue;
		}

		// Update the sensor values on the LCD screen
		sTextLCD = "Ambi:" + String(fTemperature, 2) + "C " + String(fHumidity, 2) + "%";
		vSendLCDCommand(sTextLCD, 2, 0);
	}
}