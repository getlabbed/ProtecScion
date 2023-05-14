/**
 * Nom du fichier :
 *  @name Task_IRSensor.cpp
 * Description :
 *  @brief Code permettant de gérer le capteur de distance infrarouge.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-??-?? @author Yanick Labelle - Entrée initiale du code.
 */

#include "Task_IRSensor.h"
#include <Filters.h>

void vTaskIRSensor(void *pvParameters)
{
  unsigned long prevTime = 0;
  unsigned long currTime = 0;
  float deltaTime = 0;

  float prevDistance = 0;
  float currDistance = 0;

  float speed = 0;

  Filter::LPF<float> lowpassFilter(FILTER_GAIN);

  unsigned long lastAvgTime = 0;
  float speedSum = 0;
  int speedCount = 0;

	pinMode(PIN_IR_SENSOR, INPUT);

	while (true)
	{    
		int sensorValue = analogRead(PIN_IR_SENSOR); // Lire la valeur du capteur

    // S'assurer que la valeur du capteur n'est pas égale à 11 pour éviter une division par zéro
		if (sensorValue != 11)
		{
			float distance = convertToDistance(sensorValue);

			// Calculer le temps écoulé depuis la dernière itération
			currTime = millis();
			deltaTime = (currTime - prevTime) / 1000.0;
      
      // Filtrer la distance
			float filteredDistance = lowpassFilter.get(distance, deltaTime);

      // S'assurer que la distance est entre les limites spécifiées
			if (filteredDistance >= LOW_LIMIT && filteredDistance <= HIGH_LIMIT)
			{
				currDistance = filteredDistance;

				speed = abs(currDistance - prevDistance) / deltaTime;

        // Mettre à jour la somme et le compteur de vitesse pour le calcul de la moyenne
				speedSum += speed;
				speedCount++;

				if (currTime - lastAvgTime >= AVG_INTERVAL_TIME)
				{
					float avgSpeed = speedSum / speedCount;
					vSendLog(INFO, "IR: Average Speed: " + String(avgSpeed) + " units/s");

					// Mettre à jour les valeurs de la moyenne
					lastAvgTime = currTime;
					speedSum = 0;
					speedCount = 0;
				}

				// Print the instantaneous speed
				// Serial.print("Instantaneous Speed: ");
				// Serial.print(speed);
				// Serial.print(" - ");
				// Serial.print(currDistance);
				// Serial.println(" units/s");
			}
		}

    // Mettre à jour les valeurs précédentes
		prevDistance = currDistance;
		prevTime = currTime;

    // Délai de 50ms pour assurer la stabilité du capteur
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

float convertToDistance(int sensorValue)
{
  // Formule de conversion de la distance pour le capteur SHARP GP2D2F
	float distance = 2076.0 / (sensorValue - 11.0);
	return distance;
}