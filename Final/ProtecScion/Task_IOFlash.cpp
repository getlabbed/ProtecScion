
#include "Task_IOFlash.h"

#include "FS.h"
#include <ArduinoJson.h>

void vTaskIOFlash(void *pvParameters)
{
	// make the task read from the queue every 100ms to write to the file
	Wood_t wood;
	Log_t message;

	while (1)
	{
		// when you receive a notification read the wood from the id from the notification value and write it to the queue
		if (ulTaskNotifyTake(pdTRUE, 0) > 0)
		{
			unsigned int id = ulTaskNotifyValueClear(pdFALSE); // TODO: check if this is the right way to do it
			readWood(wood, (int)pvParameters);
			xQueueSend(xQueueWriteWood, &wood, 0);
		}

		if (xQueueReceive(xQueueWriteWood, &wood, 0) == pdTRUE)
		{
			writeWood(wood.code, wood.sawSpeed, wood.feedRate);
		}
		
		if (xQueueReceive(xQueueLog, &message, 0) == pdTRUE)
		{
			logMessage(message);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void writeEmptyFile()
{
	File file = SPIFFS.open("/wood.json", "rw");
	if (!file)
	{
		Serial.println("Failed to create file");
		return;
	}

	// if the file already contains data, return
	if (file.size() > 0)
	{
		Serial.println("File already contains data");
		file.close();
		return;
	}

	if (file.print("{}"))
	{
		Serial.println("File written");
	}
	else
	{
		Serial.println("Write failed");
	}
	file.close();
}

void readWood(Wood_t &wood, int id)
{
	File file = SPIFFS.open("/wood.json", "r");
	// read the file in a variable
	String fileData;
	while (file.available())
	{
		fileData += char(file.read());
	}
	file.close();

	if (fileData.length() > 0) // Si le fichier n'est pas vide
	{
		StaticJsonDocument<2048> doc; // ajuster la taille du fichier selon la quantité de bois
		DeserializationError error = deserializeJson(doc, fileData);

		if (error)
		{
			Serial.print("deserializeJson() failed: ");
			Serial.println(error.c_str());
			return;
		}

		// if the object is not found, return
		if (doc.containsKey(String(id)) == false)
		{
			Serial.println("Object not found");
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
		Serial.println("Error reading JSON file data");
	}
}

void writeWood(int id, int sawSpeed, int feedRate)
{
	// Lire les données du fichier JSON
	File file = SPIFFS.open("/wood.json", "r");
	// read the file in a variable
	String fileData;
	while (file.available())
	{
		fileData += char(file.read());
	}
	file.close();

	if (fileData.length() > 0)
	{
		// Ajuster la taille du fichier selon la quantité de bois
		StaticJsonDocument<2048> doc;
		DeserializationError error = deserializeJson(doc, fileData);

		if (error)
		{
			Serial.print("deserializeJson() failed: ");
			Serial.println(error.c_str());
			return;
		}

		doc[String(id)]["name"] = woodType[id];
		doc[String(id)]["code"] = id;
		doc[String(id)]["sawSpeed"] = sawSpeed;
		doc[String(id)]["feedRate"] = feedRate;

		// Serialize the modified JsonDocument back to a string
		String modifiedData;
		serializeJson(doc, modifiedData);

		File file = SPIFFS.open("/wood.json", "w");
		if (!file)
		{
			Serial.println("Failed to open file for writing");
			return;
		}
		if (file.print(modifiedData))
		{
			Serial.println("File written");
		}
		else
		{
			Serial.println("Write failed");
		}
		file.close();
	}
	else
	{
		Serial.println("Error reading JSON file data");
	}
}

void logMessage(Log_t logMessage)
{
	File file = SPIFFS.open("/log.txt", "a");
	if (!file)
	{
		Serial.println("Failed to open file for writing");
		return;
	}

	// check if the file is bigger than 100kB
	if (file.size() > 100000)
	{
		Serial.println("File is bigger than 100kB, deleting");
		file.close();
		SPIFFS.remove("/log.txt");
		file = SPIFFS.open("/log.txt", "a");
	}

	// make a string with the uptime in hh:mm:ss:msmsms format
	String uptime = String(millis() / 3600000) + ":" + String((millis() / 60000) % 60) + ":" + String((millis() / 1000) % 60) + ":" + String(millis() % 1000);
	// format the message like a linux kernel log [uptime] logLevel: message
	String message = "[" + uptime + "] " + logLevelString[logMessage.level] + ": " + logMessage.message;

	if (file.println(message))
	{
		Serial.println("File written");
	}
	else
	{
		Serial.println("Write failed");
	}
	file.close();
}

void dumpLog()
{
	File file = SPIFFS.open("/log.txt", "r");
	if (!file)
	{
		Serial.println("Failed to open file for reading");
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
}
