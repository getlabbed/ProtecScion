/**
 * Nom du fichier :
 *  @name DHT11.ino
 * Description :
 *  @brief Programme permettant d'interfacer un capteur DHT11.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-08 @author Yanick Labelle - Entrée initiale du code.
 */

// Code inspiré de https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino

#include "Task_DHT11.h"
#include "DHT.h"

void vTaskDHT11(void *pvParameters)
{
	int iTrash;
	float fHumidity;
	float fTemperature;
	float fHeatIndex;
	String sTextLCD;
	DHT dht(PIN_DHT11, DHT11);
	dht.begin();
	while (true)
	{
		if (xQueueReceive(xQueueAmbiant, &iTrash, portMAX_DELAY) == pdFALSE) continue;

		fHumidity = dht.readHumidity();
		fTemperature = dht.readTemperature();

		if (isnan(fHumidity) || isnan(fTemperature))
		{
			//vSendLog(ERROR, "DHT11: ERREUR           ");
			continue;
		}

		sTextLCD = "Ambi:" + String(fTemperature, 2) + "C " + String(fHumidity, 2) + "%";
		vSendLCDCommand(sTextLCD, 2, 0);
	}
}