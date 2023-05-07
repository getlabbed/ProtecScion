
#include "Task_IOFlash.h"

#include "FS.h"
#include <ArduinoJson.h>

void vTaskIOFlash(void *pvParameters)
{
	Wood_t wood;
	Log_t message;
	unsigned int uiRequestWoodID;

	writeEmptyFile();

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
		}
		
		if (xQueueReceive(xQueueLog, &message, 0) == pdTRUE)
		{
			if (message.level >= DEBUG) {
				// Send to LCD
				continue;
			}

			if (message.level == DUMP) {
				dumpLog();
				continue;
			}
			logMessage(message);
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void writeEmptyFile()
{
	if (xSemaphoreTake(xSemaphoreSPIFFS, portMAX_DELAY) == pdFAIL) 
	{
		logMessage(Log_t{ERROR, "WRITE_FILE: Could not take SPIFFS semaphore"});
		return;
	}

	File file = SPIFFS.open("/wood.json", "w");
	if (!file)
	{
		xSemaphoreGive(xSemaphoreSPIFFS);
		logMessage(Log_t{ERROR, "WRITE_FILE: Failed to create file"});
		return;
	}

	// if the file already contains data, return
	if (file.size() > 0)
	{
		file.close();
		xSemaphoreGive(xSemaphoreSPIFFS);
		logMessage(Log_t{INFO, "WRITE_FILE: File already contains data"});
		return;
	}

	if (file.print("{}"))
	{
		file.close();
		xSemaphoreGive(xSemaphoreSPIFFS);
		logMessage(Log_t{INFO, "WRITE_FILE: File written"});
	}
	else
	{
		file.close();
		xSemaphoreGive(xSemaphoreSPIFFS);
		logMessage(Log_t{ERROR, "WRITE_FILE: Write failed"});
	}
}

void readWood(Wood_t &wood, int id)
{
	if (xSemaphoreTake(xSemaphoreSPIFFS, portMAX_DELAY) == pdFAIL) 
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
	xSemaphoreGive(xSemaphoreSPIFFS);

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
		wood.name = obj["name"].as<String>();
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
	if (xSemaphoreTake(xSemaphoreSPIFFS, portMAX_DELAY) == pdFAIL) 
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
	xSemaphoreGive(xSemaphoreSPIFFS);

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

		doc[String(id)]["name"] = woodType[id];
		doc[String(id)]["code"] = id;
		doc[String(id)]["sawSpeed"] = sawSpeed;
		doc[String(id)]["feedRate"] = feedRate;

		// Serialize the modified JsonDocument back to a string
		String modifiedData;
		serializeJson(doc, modifiedData);

		if (xSemaphoreTake(xSemaphoreSPIFFS, portMAX_DELAY) == pdFAIL) 
		{
			logMessage(Log_t{WARNING, "WRITE_WOOD: Could not take SPIFFS semaphore"});
			return;
		}

		File file = SPIFFS.open("/wood.json", "w");
		if (!file)
		{
			xSemaphoreGive(xSemaphoreSPIFFS);
			logMessage(Log_t{ERROR, "WRITE_WOOD: Failed to open file for writing"});
			return;
		}
		if (file.print(modifiedData))
		{
			file.close();
			xSemaphoreGive(xSemaphoreSPIFFS);
			logMessage(Log_t{INFO, "WRITE_WOOD: File written"});
		}
		else
		{
			file.close();
			xSemaphoreGive(xSemaphoreSPIFFS);
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
	if (xSemaphoreTake(xSemaphoreSPIFFS, portMAX_DELAY) == pdFAIL) { return; }
	File file = SPIFFS.open("/log.txt", "a");
	if (!file)
	{
		xSemaphoreGive(xSemaphoreSPIFFS);
		return;
	}

	// check if the file is bigger than 100kB
	if (file.size() > 100000)
	{
		file.close();
		SPIFFS.remove("/log.txt");
		file = SPIFFS.open("/log.txt", "a");
		if (!file)
		{
			xSemaphoreGive(xSemaphoreSPIFFS);
			return;
		}
	}

	// make a string with the uptime in hh:mm:ss:msmsms format
	String uptime = String(millis() / 3600000) + ":" + String((millis() / 60000) % 60) + ":" + String((millis() / 1000) % 60) + ":" + String(millis() % 1000);
	// format the message like a linux kernel log [uptime] logLevel: message
	String message = "[" + uptime + "] " + logLevelString[logMsg.level] + ": " + logMsg.message;
	file.println(message);
		file.close();
		xSemaphoreGive(xSemaphoreSPIFFS);
}

void dumpLog()
{
	if (xSemaphoreTake(xSemaphoreSPIFFS, portMAX_DELAY) == pdFAIL) { return; }

	File file = SPIFFS.open("/log.txt", "r");
	if (!file)
	{
		xSemaphoreGive(xSemaphoreSPIFFS);
		return;
	}

	// read the file line by line
	while (file.available())
	{
		Serial.write(file.read());
	}
	file.close();
	
	// delete the file
	SPIFFS.remove("/log.txt");
	xSemaphoreGive(xSemaphoreSPIFFS);
}
