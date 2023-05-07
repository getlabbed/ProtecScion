#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "esp_task_wdt.h"

// use a queue to send a uint32_t from one task to another
QueueHandle_t queue;

// task that sends a value to the queue
void task1(void *pvParameters)
{
	uint32_t value = 0;
	while (1)
	{
		// send the value to the queue
		if(xQueueSend(queue, &value, portMAX_DELAY) != pdPASS) {
			Serial.println("Failed to send value to queue!");
		}
		value++;
		vTaskDelay(pdMS_TO_TICKS(100)); // Delay to allow the consumer task to catch up
	}
}

// task that receives a value from the queue
void task2(void *pvParameters)
{
	uint32_t value;
	while (1)
	{
		// wait for a value to arrive
		if(xQueueReceive(queue, &value, portMAX_DELAY) != pdPASS) {
			Serial.println("Failed to receive value from queue!");
		} else {
			Serial.println(value);
		}
	}
}


void setup()
{
	Serial.begin(115200);
	// create the queue
	queue = xQueueCreate(1, sizeof(uint32_t)); // increase queue size to allow for multiple values
	// create the tasks with different priorities
	xTaskCreatePinnedToCore(task1, "task1", 10000, NULL, 2, NULL, 0);
	xTaskCreatePinnedToCore(task2, "task2", 10000, NULL, 1, NULL, 1);

	// remove wdt
	esp_task_wdt_deinit();
	esp_task_wdt_delete(NULL);
}

void loop()
{
}
