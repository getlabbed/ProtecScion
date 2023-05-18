 /**
 * @file Task_DHT11.cpp
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Programme permettant d'interfacer un capteur DHT11.
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-08 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

// Code inspiré de https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino

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
			//vSendLog(ERROR, "DHT11: ERREUR           ");
			continue;
		}

		// Mettre à jour les valeurs ddu capteur sur l'écran LCD
		sTextLCD = "Ambi:" + String(fTemperature, 2) + "C " + String(fHumidity, 2) + "%";
		vSendLCDCommand(sTextLCD, 2, 0);
	}
}