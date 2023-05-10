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
	DHT dht(PIN_DHT11, DHT11);
	dht.begin();
	while (true)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		float fHumidity = dht.readHumidity();
		float fTemperature = dht.readTemperature();
		
		if (isnan(fHumidity) || isnan(fTemperature))
		{
			vSendLog(ERROR, "DHT11: Impossible de lire les données du capteur DHT !");
		}
		else{
			float fHeatIndex = dht.computeHeatIndex(fTemperature, fHumidity, false); // false = °Celsius

			xQueueSend(xQueueAmbiantHumidity, &fHumidity, 0);
			xQueueSend(xQueueAmbiantTemperature, &fTemperature, 0);
			xQueueSend(xQueueHeatIndex, &fHeatIndex, 0);
		}
	}
}