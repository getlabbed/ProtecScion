// Mode manuel: ajouter un bois si un bois n
//
//


#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>

typedef struct
{
	char name[16]; // nom du bois
	int code;	   // code du bois
	int sawSpeed;  // vitesse de la lame
	int feedRate;  // vitesse d'avancement
} Wood_t;

// Types de bois disponibles
const String woodType[] = {"Sapin", "Pin", "Noyer", "Bouleau", "Chêne", "Érable", "Frêne", "Merisier", "Mélèze", "Épinette"};

void writeEmptyFile();
void readWood(Wood_t wood, int id);
void writeWood(Wood_t wood, int id, int sawSpeed, int feedRate);
void updateWood(Wood_t wood, int id, int sawSpeed, int feedRate);

void setup()
{
	Serial.begin(115200);
	if (!SPIFFS.begin(true))
	{
		Serial.println("An error has occurred while mounting SPIFFS");
	}
	else
	{
		writeEmptyFile();
		Wood_t wood;
		writeWood(wood, 1, 100, 100);
		readWood(wood, 1);
		Serial.println(wood.name);
		Serial.println(wood.code);
		Serial.println(wood.sawSpeed);
		Serial.println(wood.feedRate);

		updateWood(wood, 1, 200, 200);

		wood = readWood(wood, 1);
		// print the return of the function (struct)
		Serial.println(wood.name);
		Serial.println(wood.code);
		Serial.println(wood.sawSpeed);
		Serial.println(wood.feedRate);
		}
}

void loop()
{
	// Ne rien faire
}

void writeEmptyFile()
{
	File file = SPIFFS.open("/wood.json", "w");
	if (!file)
	{
		Serial.println("Failed to create file");
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

		JsonObject root = doc.as<JsonObject>();
		for (JsonPair keyValue : root)
		{
			JsonObject woodObj = keyValue.value().as<JsonObject>();
			if (woodObj["code"].as<int>() == id)
			{
				strncpy(wood.name, woodObj["name"].as<const char *>(), sizeof(wood.name) - 1);
				wood.code = woodObj["code"].as<int>();
				wood.sawSpeed = woodObj["sawSpeed"].as<int>();
				wood.feedRate = woodObj["feedRate"].as<int>();
				break;
			}
			else
			{
				strncpy(wood.name, "NULL", sizeof(wood.name) - 1); // Si le bois n'est pas trouvé, retourner avec un nom NULL
			}
		}
	}
	else
	{
		Serial.println("Error reading JSON file data");
	}
}

void writeWood(Wood_t &wood, int id, int sawSpeed, int feedRate)
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

		// Créer un nouveau object JSON avec les données du bois
		JsonObject newWood;
		newWood["name"] = woodType[id];
		newWood["code"] = id;
		newWood["sawSpeed"] = sawSpeed;
		newWood["feedRate"] = feedRate;

		// Add the new JsonObject to the JsonDocument
		JsonObject root = doc.as<JsonObject>();
		root[String(id)] = newWood;
		// Serialize the modified JsonDocument back to a string

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

void updateWood(Wood_t &wood, int id, int sawSpeed, int feedRate)
{
	// Lire le fichier de configuration
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
		// Définir la taille du fichier selon la quantité de bois
		StaticJsonDocument<2048> doc;
		DeserializationError error = deserializeJson(doc, fileData);

		if (error)
		{
			Serial.print("deserializeJson() failed: ");
			Serial.println(error.c_str());
			return;
		}

		// Accédé à l'objet json du bois à l'aide de son id
		JsonObject root = doc.as<JsonObject>();
		JsonObject woodObj = root[String(id)];

		// Mettre à jour les propriétés du JsonObject
		woodObj["name"] = woodType[id];
		woodObj["sawSpeed"] = sawSpeed;
		woodObj["feedRate"] = feedRate;

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
