/**
 * @file Task_Apprentissage.cpp
 * @author Skkeye's coleague
 * @brief Code used to manage the infrared distance sensor.
 * @note restrictions: ESP32 Feather board type
 * 
 * @version 1.0
 * @date 2023-04-30 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

#include "Task_Apprentissage.h"
#include "Task_AsservissementScie.h"
#include <Filters.h>

/**
 * @brief Task used to manage the infrared distance sensor.
 * 
 * @param pvParameters - Not used
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

	// Init the sensor

	pinMode(PIN_IR_SENSOR, INPUT);

	int iControlFlag = 1;

	unsigned long uiPrevTime2 = 0;

	while (true)
	{
		xWood.code = 0;
		xQueueReceive(xQueueApprentissageControl, &(xWood.code), portMAX_DELAY);
    
    // Start the task if the flag is true
		if (xWood.code == 0) continue;
		iControlFlag = 1;
		while (iControlFlag == 1)
		{
			int iSensorValue = analogRead(PIN_IR_SENSOR); // Read the sensor value

			// Make sure the sensor value is not equal to 11 to avoid a division by zero
			if (iSensorValue != 11)
			{
				float fDistance = convertToDistance(iSensorValue);

				// Calculate the time elapsed since the last iteration
				uiCurrTime = millis();
				fDeltaTime = (uiCurrTime - uiPrevTime) / 1000.0;

				// Filter the distance value
				float fFilteredDistance = lowpassFilter.get(fDistance, fDeltaTime);

				// Make sure the distance is between the specified limits
				if (fFilteredDistance >= LOW_LIMIT && fFilteredDistance <= HIGH_LIMIT)
				{
					fCurrDistance = fFilteredDistance;

					fSpeed = abs(fCurrDistance - fPrevDistance) / fDeltaTime;

					// Update the sum and the speed counter for the averaging
					fSpeedSum += fSpeed;
					iSpeedCount++;

					if (uiCurrTime - lastAvgTime >= AVG_INTERVAL_TIME)
					{
						fAvgFeedRate = fSpeedSum / iSpeedCount;
						vSendLog(INFO, "IR: Average Speed: " + String(fAvgFeedRate) + " units/s");

						// Update the average
						lastAvgTime = uiCurrTime;
						fSpeedSum = 0;
						iSpeedCount = 0;
					}
					dSawSpeedSum += analogRead(PIN_SENSE);
					liSawSpeedCount++;
				}
			}

			// Update the previous values
			fPrevDistance = fCurrDistance;
			uiPrevTime = uiCurrTime;

			// Update the data on the screen
			if (millis() - uiPrevTime2 >= 500)
			{
				uiPrevTime2 = millis();
				xQueueSend(xQueueSound, &(xWood.code), 0);
				xQueueSend(xQueueAmbiant, &(xWood.code), 0);
			}

			// 50ms delay to ensure the stability of the sensor
			vTaskDelay(50 / portTICK_PERIOD_MS);

			if (!xQueueReceive(xQueueApprentissageControl, &iControlFlag, 0)) continue;
			if (iControlFlag == -1) break;
			float avgSawSpeed = dSawSpeedSum / liSawSpeedCount;

			xWood.code = xWood.code;
			xWood.feedRate = fAvgFeedRate;
			xWood.sawSpeed = (avgSawSpeed * 100) / 4095;
			vSendLog(INFO, "IR: Saw Speed: " + String(avgSawSpeed) + " units/s");
			vSendLog(INFO, "IR: Feed Rate: " + String(fAvgFeedRate) + " units/s");

			xQueueSend(xQueueWriteWood, &xWood, portMAX_DELAY);
		}
	}
}

/**
 * @brief Convert the sensor value to distance
 * 
 * @param iSensorValue - Sensor value
 * @return float - Distance in cm
 */
float convertToDistance(int iSensorValue)
{
	// Conversion formula for the SHARP GP2D2F sensor
	float fDistance = 2076.0 / (iSensorValue - 11.0);
	return fDistance;
}