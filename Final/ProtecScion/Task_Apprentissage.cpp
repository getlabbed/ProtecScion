/**
 * Nom du fichier :
 *  @name Task_Apprentissage.cpp
 * Description :
 *  @brief Code permettant de gérer le capteur de distance infrarouge.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-??-?? @author Yanick Labelle - Entrée initiale du code.
 */

#include "Task_Apprentissage.h"
#include <Filters.h>

void vTaskApprentissage(void *pvParameters)
{
	Wood_t wood;

	unsigned long prevTime = 0;
	unsigned long currTime = 0;
	float deltaTime = 0;

	float prevDistance = 0;
	float currDistance = 0;

	float speed = 0;
	float avgFeedRate = 0;

	Filter::LPF<float> lowpassFilter(FILTER_GAIN);

	unsigned long lastAvgTime = 0;
	float speedSum = 0;
	int speedCount = 0;

	double sawSpeedSum = 0;
	long int sawSpeedCount = 0;

	// Initialiser le capteur

	pinMode(PIN_IR_SENSOR, INPUT);

	unsigned int controlFlag = 1;

	while (true)
	{
		wood.code = 0;
		xQueueReceive(xQueueApprentissageControl, &(wood.code), portMAX_DELAY);
		// If the control signal is true, start the task

		if (wood.code == 0) continue;
		controlFlag = 1;
		while (controlFlag == 1)
		{

			// MAIN CODE
			// Lire la valeur du capteur
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
						avgFeedRate = speedSum / speedCount;
						vSendLog(INFO, "IR: Average Speed: " + String(avgFeedRate) + " units/s");

						// Mettre à jour les valeurs de la moyenne
						lastAvgTime = currTime;
						speedSum = 0;
						speedCount = 0;
					}
					sawSpeedSum += analogRead(PIN_IR_SENSOR);
					sawSpeedCount++;
				}
			}

			// Mettre à jour les valeurs précédentes
			prevDistance = currDistance;
			prevTime = currTime;

			// Délai de 50ms pour assurer la stabilité du capteur
			vTaskDelay(50 / portTICK_PERIOD_MS);

			if (!xQueueReceive(xQueueApprentissageControl, &controlFlag, 0)) continue;
			float avgSawSpeed = sawSpeedSum / sawSpeedCount;

			wood.code = wood.code;
			wood.feedRate = avgFeedRate;
			wood.sawSpeed = avgSawSpeed;
      vSendLog(INFO, "IR: Saw Speed: " + String(avgSawSpeed) + " units/s");
      vSendLog(INFO, "IR: Feed Rate: " + String(avgFeedRate) + " units/s");

			xQueueSend(xQueueWriteWood, &wood, portMAX_DELAY);
		}
	}
}

float convertToDistance(int sensorValue)
{
	// Formule de conversion de la distance pour le capteur SHARP GP2D2F
	float distance = 2076.0 / (sensorValue - 11.0);
	return distance;
}