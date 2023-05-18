/**
 * @file Task_Apprentissage.cpp
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Code permettant de gérer le capteur de Distance infrarouge.
 * @note restrictions: Pour type de carte ESP32 Feather
 * 
 * @version 1.0
 * @date 2023-04-30 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "Task_Apprentissage.h"
#include "Task_AsservissementScie.h"
#include <Filters.h>

/**
 * @brief Tâche permettant de gérer le capteur de Distance infrarouge.
 * 
 * @param pvParameters - Non utilisé
 */
void vTaskApprentissage(void *pvParameters)
{
	Wood_t xWood;

	unsigned long uiPrevTime = 0;
	unsigned long uiCurrTime = 0;
	float fDeltaTime = 0;

	float fPrevDistance = 0;
	float fCurrDistance = 0;

	float fSpeed = 0;
	float fAvgFeedRate = 0;

	Filter::LPF<float> lowpassFilter(FILTER_GAIN);

	unsigned long lastAvgTime = 0;
	float fSpeedSum = 0;
	int iSpeedCount = 0;

	double dSawSpeedSum = 0;
	long int liSawSpeedCount = 0;

	// Initialiser le capteur

	pinMode(PIN_IR_SENSOR, INPUT);

	int iControlFlag = 1;

	unsigned long uiPrevTime2 = 0;

	while (true)
	{
		xWood.code = 0;
		xQueueReceive(xQueueApprentissageControl, &(xWood.code), portMAX_DELAY);
    
    // Démarrer la tâche si le flag est vrai
		if (xWood.code == 0) continue;
		iControlFlag = 1;
		while (iControlFlag == 1)
		{
			int iSensorValue = analogRead(PIN_IR_SENSOR); // Lire la valeur du capteur

			// S'assurer que la valeur du capteur n'est pas égale à 11 pour éviter une division par zéro
			if (iSensorValue != 11)
			{
				float fDistance = convertToDistance(iSensorValue);

				// Calculer le temps écoulé depuis la dernière itération
				uiCurrTime = millis();
				fDeltaTime = (uiCurrTime - uiPrevTime) / 1000.0;

				// Filtrer la distance
				float fFilteredDistance = lowpassFilter.get(fDistance, fDeltaTime);

				// S'assurer que la distance est entre les limites spécifiées
				if (fFilteredDistance >= LOW_LIMIT && fFilteredDistance <= HIGH_LIMIT)
				{
					fCurrDistance = fFilteredDistance;

					fSpeed = abs(fCurrDistance - fPrevDistance) / fDeltaTime;

					// Mettre à jour la somme et le compteur de vitesse pour le calcul de la moyenne
					fSpeedSum += fSpeed;
					iSpeedCount++;

					if (uiCurrTime - lastAvgTime >= AVG_INTERVAL_TIME)
					{
						fAvgFeedRate = fSpeedSum / iSpeedCount;
						vSendLog(INFO, "IR: Average Speed: " + String(fAvgFeedRate) + " unitees/s");

						// Mettre à jour les valeurs de la moyenne
						lastAvgTime = uiCurrTime;
						fSpeedSum = 0;
						iSpeedCount = 0;
					}
					dSawSpeedSum += analogRead(PIN_SENSE);
					liSawSpeedCount++;
				}
			}

			// Mettre à jour les valeurs précédentes
			fPrevDistance = fCurrDistance;
			uiPrevTime = uiCurrTime;

			// Mettre à jour les données sur l'écran
			if (millis() - uiPrevTime2 >= 500)
			{
				uiPrevTime2 = millis();
				xQueueSend(xQueueSound, &(xWood.code), 0);
				xQueueSend(xQueueAmbiant, &(xWood.code), 0);
			}

			// Délai de 50ms pour assurer la stabilité du capteur
			vTaskDelay(50 / portTICK_PERIOD_MS);

			if (!xQueueReceive(xQueueApprentissageControl, &iControlFlag, 0)) continue;
			if (iControlFlag == -1) break;
			float avgSawSpeed = dSawSpeedSum / liSawSpeedCount;

			xWood.code = xWood.code;
			xWood.feedRate = fAvgFeedRate;
			xWood.sawSpeed = (avgSawSpeed * 100) / 4095;
			vSendLog(INFO, "IR: Saw Speed: " + String(avgSawSpeed) + " unites/s");
			vSendLog(INFO, "IR: Feed Rate: " + String(fAvgFeedRate) + " unites/s");

			xQueueSend(xQueueWriteWood, &xWood, portMAX_DELAY);
		}
	}
}

/**
 * @brief Convertir la valeur du capteur en fDistance
 * 
 * @param iSensorValue - Valeur du capteur
 * @return float - Distance en cm
 */
float convertToDistance(int iSensorValue)
{
	// Formule de conversion de la fDistance pour le capteur SHARP GP2D2F
	float fDistance = 2076.0 / (iSensorValue - 11.0);
	return fDistance;
}