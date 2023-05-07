/**
 * Nom du fichier :
 *  @name DHT11.ino
 * Description :
 *  @brief Programme permettant d'interfacer un capteur DHT11.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-07 @author Yanick Labelle - Entrée initiale du code.
 */

// Code inspiré de https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino

#include "DHT.h"

#define PIN_DHT11 33

DHT dht(PIN_DHT11, DHT11);

void setup()
{
	Serial.begin(9600);
	dht.begin();
}

void loop()
{
	delay(1000);

	float fHumidity = dht.readHumidity();
	float fTemperature = dht.readTemperature();

	if (isnan(fHumidity) || isnan(fTemperature))
	{
		Serial.println("ERROR: Impossible de lire les données du capteur DHT !");
	}

	float fHeatIndex = dht.computeHeatIndex(fTemperature, fHumidity, false);

	Serial.print("Humidity: ");
	Serial.print(fHumidity);
	Serial.print("% ");

	Serial.print("Temperature: ");
	Serial.print(fTemperature);
	Serial.print("C ");

	Serial.print("Heat index: ");
	Serial.print(fHeatIndex);
	Serial.println("C ");
}