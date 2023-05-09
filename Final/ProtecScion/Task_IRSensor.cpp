#include "Task_IRSensor.h"
#include <Filters.h>

// Timer
unsigned long prevTime = 0;
unsigned long currTime = 0;
float deltaTime = 0;

// Distance
float prevDistance = 0;
float currDistance = 0;

// Speed
float speed = 0;

// Filter object
Filter::LPF<float> lowpassFilter(FILTER_GAIN);

// Average speed calculation
unsigned long lastAvgTime = 0;
float speedSum = 0;
int speedCount = 0;

void vTaskIRSensor(void *pvParameters)
{
	pinMode(PIN_IR_SENSOR, INPUT);

	// wait 2 sec then send a message per line to the lcd queue
	vTaskDelay(2000 / portTICK_PERIOD_MS);
	LCDCommand_t cmdBuffer4 = {"IR: 3333", 3, 0};
	xQueueSend(xQueueLCD, &cmdBuffer4, 0);
	LCDCommand_t cmdBuffer1 = {"IR: 0", 0, 0};
	xQueueSend(xQueueLCD, &cmdBuffer1, 0);
	LCDCommand_t cmdBuffer2 = {"IR: 11", 1, 0};
	xQueueSend(xQueueLCD, &cmdBuffer2, 0);
	LCDCommand_t cmdBuffer3 = {"IR: 222", 2, 0};
	xQueueSend(xQueueLCD, &cmdBuffer3, 0);
	LCDCommand_t cmdBuffer5 = {"IR: 44444", 3, 2000};
	xQueueSend(xQueueLCD, &cmdBuffer5, 0);
	

	while (true)
	{
		// Read the sensor value
		int sensorValue = analogRead(PIN_IR_SENSOR);

		// Check if the sensor value is not equal to 11 to avoid division by zero
		if (sensorValue != 11)
		{
			float distance = convertToDistance(sensorValue);

			// Calculate deltaTime
			currTime = millis();
			deltaTime = (currTime - prevTime) / 1000.0;

			// Filter the distance value
			float filteredDistance = lowpassFilter.get(distance, deltaTime);

			// Check if the distance is within the specified limits
			if (filteredDistance >= LOW_LIMIT && filteredDistance <= HIGH_LIMIT)
			{
				currDistance = filteredDistance;

				speed = abs(currDistance - prevDistance) / deltaTime;

				// Update the speed sum and count for the average calculation
				speedSum += speed;
				speedCount++;

				// Calculate and print the average speed over the time interval
				if (currTime - lastAvgTime >= AVG_INTERVAL_TIME)
				{
					float avgSpeed = speedSum / speedCount;
					Log_t log = {INFO, "IR: Average Speed: " + String(avgSpeed) + " units/s"};
					xQueueSend(xQueueLog, &log, portMAX_DELAY);

					// Reset values for the next interval
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

		// Update previous values
		prevDistance = currDistance;
		prevTime = currTime;

		// Delay for stability
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

float convertToDistance(int sensorValue)
{
	// Conversion formula based on SHARP GP2D2F sensor characteristics
	float distance = 2076.0 / (sensorValue - 11.0);
	return distance;
}