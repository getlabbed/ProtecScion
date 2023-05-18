
#include "Task_IOFlash.h"

#include "FS.h"
#include <ArduinoJson.h>

void vTaskIOFlash(void *pvParameters)
{
	Wood_t wood;
	Log_t message;
	unsigned int uiRequestWoodID;

	File file = SPIFFS.open("/wood.json", FILE_READ);
	if (!file || file.size() == 0)
	{
		file.close();
		writeEmptyFile();
	}
	else
	{
		file.close();
	}

	while (1)
	{
		// read from the queue
		if (xQueueReceive(xQueueRequestWood, &uiRequestWoodID, 0) == pdTRUE)
		{
			readWood(wood, uiRequestWoodID);
			xQueueSend(xQueueReadWood, &wood, 0);
		}

		if (xQueueReceive(xQueueWriteWood, &wood, 0) == pdTRUE)
		{

			writeWood(wood.code, wood.sawSpeed, wood.feedRate);
			xSemaphoreGive(xSemaphoreLog);
		}

		if (xQueueReceive(xQueueLog, &message, 0) == pdTRUE)
		{
			// if (message.level >= DEBUG && message.level <= ERROR) {
			// 	String myString = message.message.substring(0, 15);
			// 	vSendLCDCommand(myString, 0, 3000);
			// }

			if (message.level == DUMP)
			{
				dumpLog();
				vDumpWood(message.message == "PURGE");
				xSemaphoreGive(xSemaphoreLog);
				continue;
			}
			logMessage(message);
			xSemaphoreGive(xSemaphoreLog);
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void writeEmptyFile()
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		logMessage(Log_t{ERROR, "WRITE_FILE: Could not take SPIFFS semaphore"});
		return;
	}

	File file = SPIFFS.open("/wood.json", "w");

	if (file.print("{}"))
	{
		file.close();
		xSemaphoreGive(xSemaphoreSPI);
		vTaskDelay(10 / portTICK_PERIOD_MS);
		logMessage(Log_t{INFO, "WRITE_FILE: File written"});
	}
	else
	{
		file.close();
		xSemaphoreGive(xSemaphoreSPI);
		vTaskDelay(10 / portTICK_PERIOD_MS);
		logMessage(Log_t{ERROR, "WRITE_FILE: Write failed"});
	}
}

void readWood(Wood_t &wood, int id)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		logMessage(Log_t{WARNING, "READ_WOOD: Could not take SPIFFS semaphore"});
		return;
	}

	File file = SPIFFS.open("/wood.json", "r");
	// read the file in a variable
	String fileData;
	while (file.available())
	{
		fileData += char(file.read());
	}
	file.close();
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);

	if (fileData.length() > 0) // Si le fichier n'est pas vide
	{
		StaticJsonDocument<2048> doc; // ajuster la taille du fichier selon la quantité de bois
		DeserializationError error = deserializeJson(doc, fileData);

		if (error)
		{
			String error = "READ_WOOD: deserializeJson() failed:\n" + String(error.c_str());
			logMessage(Log_t{ERROR, error});
			return;
		}

		// if the object is not found, return
		if (doc.containsKey(String(id)) == false)
		{
			logMessage(Log_t{WARNING, "READ_WOOD: Object not found"});
			return;
		}

		JsonObject obj = doc[String(id)];
		wood.code = obj["code"].as<int>();
		wood.sawSpeed = obj["sawSpeed"].as<int>();
		wood.feedRate = obj["feedRate"].as<int>();
	}
	else
	{
		logMessage(Log_t{ERROR, "READ_WOOD: Error reading JSON file data"});
	}
}

void writeWood(int id, int sawSpeed, int feedRate)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		logMessage(Log_t{WARNING, "WRITE_WOOD: Could not take SPIFFS semaphore"});
		return;
	}
	// Lire les données du fichier JSON
	File file = SPIFFS.open("/wood.json", "r");
	// read the file in a variable
	String fileData;
	while (file.available())
	{
		fileData += char(file.read());
	}
	file.close();
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);

	if (fileData.length() > 0)
	{
		// Ajuster la taille du fichier selon la quantité de bois
		StaticJsonDocument<2048> doc;
		DeserializationError error = deserializeJson(doc, fileData);

		if (error)
		{
			String error = "WRITE_WOOD: deserializeJson() failed:\n" + String(error.c_str());
			logMessage(Log_t{ERROR, error});
			return;
		}

		doc[String(id)]["code"] = id;
		doc[String(id)]["sawSpeed"] = sawSpeed;
		doc[String(id)]["feedRate"] = feedRate;

		// Serialize the modified JsonDocument back to a string
		String modifiedData;
		serializeJson(doc, modifiedData);

		if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
		{
			logMessage(Log_t{WARNING, "WRITE_WOOD: Could not take SPIFFS semaphore"});
			return;
		}

		File file = SPIFFS.open("/wood.json", "w");
		if (!file)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			xSemaphoreGive(xSemaphoreSPI);
			logMessage(Log_t{ERROR, "WRITE_WOOD: Failed to open file for writing"});
			return;
		}
		if (file.print(modifiedData))
		{
			file.close();
			vTaskDelay(10 / portTICK_PERIOD_MS);
			xSemaphoreGive(xSemaphoreSPI);
			logMessage(Log_t{INFO, "WRITE_WOOD: File written"});
		}
		else
		{
			file.close();
			vTaskDelay(10 / portTICK_PERIOD_MS);
			xSemaphoreGive(xSemaphoreSPI);
			logMessage(Log_t{ERROR, "WRITE_WOOD: Write failed"});
		}
	}
	else
	{
		logMessage(Log_t{ERROR, "WRITE_WOOD: Error reading JSON file data"});
	}
}

void logMessage(Log_t logMsg)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		return;
	}
	File file = SPIFFS.open("/log.txt", "a");
	if (!file)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		xSemaphoreGive(xSemaphoreSPI);
		return;
	}

	// Regarder si le fichier est plus grand que 100kB
	if (file.size() > 100000)
	{
		file.close();
		SPIFFS.remove("/log.txt");
		file = SPIFFS.open("/log.txt", "a");
		if (!file)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			xSemaphoreGive(xSemaphoreSPI);
			return;
		}
	}

	// make a string with the uptime in hh:mm:ss:msmsms format
	String uptime = String(millis() / 3600000) + ":" + String((millis() / 60000) % 60) + ":" + String((millis() / 1000) % 60) + ":" + String(millis() % 1000);
	// format the message like a linux kernel log [uptime] logLevel: message
	String message = "[" + uptime + "] " + logLevelString[logMsg.level] + ": " + logMsg.message;
	file.println(message);
	file.close();
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);
}

void dumpLog()
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		return;
	}

	File file = SPIFFS.open("/log.txt", "r");
	if (!file)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		xSemaphoreGive(xSemaphoreSPI);
		return;
	}

	// read the file line by line
	while (file.available())
	{
		xSemaphoreTake(xSemaphoreSerial, portMAX_DELAY);
		Serial.println(file.readStringUntil('\n'));
		xSemaphoreGive(xSemaphoreSerial);
	}
	file.close();

	// delete the file
	SPIFFS.remove("/log.txt");
	xSemaphoreGive(xSemaphoreSPI);
}

void vDumpWood(bool bPurge)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		return;
	}

	File file = SPIFFS.open("/wood.json", "r");
	if (!file)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		xSemaphoreGive(xSemaphoreSPI);
		return;
	}

	// read the file line by line
	while (file.available())
	{
		xSemaphoreTake(xSemaphoreSerial, portMAX_DELAY);
		Serial.write(file.read());
		xSemaphoreGive(xSemaphoreSerial);
	}
	file.close();

	if (bPurge)
	{
		// delete the file
		SPIFFS.remove("/wood.json");
		ESP.restart();
	}
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);
}