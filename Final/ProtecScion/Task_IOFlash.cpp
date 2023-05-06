
#include "Task_IOFlash.h"

#include "FS.h"
#include <ArduinoJson.h>

void vTaskIOFlash(void *pvParameters)
{

  if (false)  //!SPIFFS.totalBytes() == 0)
  {
    // Error mounting SPIFFS
  } else {
    // SPIFFS mounted successfully

    // test
    writeEmptyFile();
		Wood_t wood;

		readWood(wood, 1);
		Serial.println(wood.name);
		Serial.println(wood.code);
		Serial.println(wood.sawSpeed);
		Serial.println(wood.feedRate);
		readWood(wood, 2);
		Serial.println(wood.name);
		Serial.println(wood.code);
		Serial.println(wood.sawSpeed);
		Serial.println(wood.feedRate);

		writeWood(1, 100, 100);

		//updateWood(wood, 1, 200, 200);
		writeWood(2, 300, 300);

		readWood(wood, 1);
		Serial.println(wood.name);
		Serial.println(wood.code);
		Serial.println(wood.sawSpeed);
		Serial.println(wood.feedRate);

		readWood(wood, 2);
		// print the return of the function (struct)
		Serial.println(wood.name);
		Serial.println(wood.code);
		Serial.println(wood.sawSpeed);
		Serial.println(wood.feedRate);
  }

  //setup();
  while (true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
		Serial.println("Hello World");
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
