/*
 * Nom du fichier :
 * ProtecScion.ino
 * Description :
 * Programme permettant d'améliorer la fonctionnalité d'un banc de scie.
 * restrictions:
 * Pour type de carte ESP32 Feather
 * Historique :
 * 2023-04-13 Olivier David Laplante, Yanick Labelle - Entrée initiale du code.
 * Notes: Aucune(s)
 */
// ======================================== Fichiers d'inclusion =================================================

// Importation du fichier d'inclusion global
#include "src/global.h"

// Importation des bibliothèques
#include <Adafruit_MLX90614.h>
#include <Adafruit_LiquidCrystal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <Adafruit_NeoPixel.h>

#include "menus.h";

// Importation des bibliothèques pour le système de fichiers
#include <FS.h>
#include <LittleFS.h>

// Importation des bibliothèques pour le debug
#include <TinyConsole.h>
#include <TinyStreaming.h>
#include <TinyString.h>

#include <Keypad.h>

#include <PID_v1.h>

#include <ArduinoJson.h>

// Déclaration des Handlers
TaskHandle_t HandleUpdateLCD;
TaskHandle_t HandleUpdateDb;
TaskHandle_t HandleUpdateAmbiantHumidTemp;
TaskHandle_t HandlereadSawSpeed;

// Déclaration des files
QueueHandle_t xQueueWoodTemp;
QueueHandle_t xQueueAmbiantHumid;
QueueHandle_t xQueueAmbiantTemp;
QueueHandle_t xQueueDb;
QueueHandle_t xQueueLightIndicator;

LCDParams_t LCDParams;

// ===================================== Définition des constantes ===============================================

#define TASK_STACK_SIZE 2048	// Taille allouée de la pile pour les tâches RTOS ()
#define VOLTAGE_REFERENCE 3.3 // Tension de rérérence pour le ESP32 Feather

#define TEMPERATURE_PIN 2			// Broche de la température ambiante
#define HUMIDITY_PIN 3				// Broche de l'humidité ambiante
#define SOUND_SENSOR_PIN 4		// Broche du niveau sonore
#define LIGHT_INDICATOR_PIN 5 // Broche de l'indicateur lumineux
#define SAW_INPUT_PIN 6				// Broche d'entrée de la vitesse de la scie
#define SAW_OUTPUT_PIN 7			// Broche de sortie PWM pour la vitesse de la scie

// ================================== Définition des variables globales ==========================================

// Définition des variables pour le PID
double dWantedSawSpeed, dSawInput, dSawOutput;

double Kp = 2, Ki = 5, Kd = 1;

int sumSawSpeed, readingCount;

// ------------------------------------ Initialisation du Clavier numérique ---------------------------------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
		{'1', '2', '3', 'A'},
		{'4', '5', '6', 'B'},
		{'7', '8', '9', 'C'},
		{'*', '0', '#', 'D'}};

// Types de bois disponibles
const String woodType[] = {"Sapin","Pin","Noyer","Bouleau","Chêne","Érable","Frêne","Merisier","Mélèze","Épinette"};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/// Affichage de la température du bois
void vTaskReadTemueWood()
{
	Adafruit_MLX90614 xIRWoodTempReader = Adafruit_MLX90614();
	double dWoodTemp = 20;

	if (!xIRWoodTempReader.begin())
	{
		// Erreur de communication avec le capteur
	}

	while (1)
	{
		dWoodTemp = xIRWoodTempReader.readObjectTempC();

		xQueueSend(xQueueWoodTemp, &dWoodTemp, pdMS_TO_TICKS(1000));
	}
}

/// Programmation des indicateurs lumineux

/// @brief Tâche permettant de mettre à jour l'indicateur lumineux
/// @param pvParameters Paramètres de la tâche

void vTaskSetLightIndicator(void *pvParameters)
{
	Adafruit_NeoPixel xLightIndicator(1, LIGHT_INDICATOR_PIN);
	xLightIndicator.begin();
	xLightIndicator.clear();
	LightColors_t *color;

	while (1)
	{
		if (!xQueueReceive(xQueueLightIndicator, color, portMAX_DELAY))
			continue;
		if (*color == Red)
		{
			xLightIndicator.setPixelColor(0, xLightIndicator.Color(255, 0, 0));
		}
		else if (*color == Green)
		{
			xLightIndicator.setPixelColor(0, xLightIndicator.Color(0, 255, 0));
		}
		else if (*color == Blue)
		{
			xLightIndicator.setPixelColor(0, xLightIndicator.Color(0, 0, 255));
		}
	}
}

/// @brief Tâche permettant d'activer le mode apprentissage
/// @param pvParameters Paramètres de la tâche
/// @note Cette tâche est appelée lorsque l'utilisateur appuie sur le bouton C

void vTasklearningMode(void *pvParameters)
{
	// Aller chercher la valeur de la vitesse de la scie dans le fichier de configuration
	sumSawSpeed = 0;
	readingCount = 0;
	setSawSpeed(4095); // Définir la vitesse maximale de la scie

	dSawInput = analogRead(SAW_INPUT_PIN);
	while (dSawInput < 4000)
	{
		dSawInput = analogRead(SAW_INPUT_PIN);
	}
	while (true)
	{
		if (dSawInput >= 4000)
			{
				// Faire la moyenne des valeurs de la vitesse de la scie
				sumSawSpeed += dSawInput;
				readingCount++;
			}
		else
		{
			if (readingCount != 0) // Ne pas diviser par 0
			{
				// Envoyer la vitesse moyenne de la scie
				double truc = sumSawSpeed / readingCount;
			}
		}
	}
}

/// https://github.com/br3ttb/Arduino-PID-Library/blob/master/examples/PID_Basic/PID_Basic.ino

/// @brief Tâche permettant de contrôler la vitesse de la scie
/// @param pvParameters Paramètres de la tâche
/// @return void

void vTaskSawControl(void *pvParameters)
{
	// Aller chercher la valeur de la vitesse de la scie dans le fichier de configuration

	dWantedSawSpeed = readWood(id).sawSpeed; // Définir la vitesse désirée de la scie (0-4095)
	dfeedRate = readWood(id).feedRate;			 // Définir la vitesse d'avancement du bois

	// Configuration du PID
	myPID.SetMode(AUTOMATIC);
	myPID.SetOutputLimits(0, 4095); // Set the output limits to match the saw speed range
	myPID.SetSampleTime(50);				// Set the PID sample time in milliseconds

	while (true)
	{
		dSawInput = analogRead(SAW_INPUT_PIN); // Lire la vitese de la scie (0-4095)

		// Vérification de critères (anti-recul)

		myPID.Compute(); // Exécuter le PID

		setSawSpeed(dSawOutput); // Définir la vitesse de la scie (0-4095)

		// LOG SYSTEM
	}
}

/// @brief Fonction permettant de convertir un id de bois en un objet de type Wood_
/// @param id Id du bois
/// @return Wood_t

Wood_t readWood(int id)
{
	Wood_t wood;
	String fileData = readFileData("/wood.json");

	if (fileData.length() > 0) // Si le fichier n'est pas vide
	{
		StaticJsonDocument<2048> doc; // ajuster la taille du fichier selon la quantité de bois
		DeserializationError error = deserializeJson(doc, fileData);

		if (error)
		{
			Serial.print("deserializeJson() failed: ");
			Serial.println(error.c_str());
			return wood;
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
		}
	}
	else
	{
		Serial.println("Error reading JSON file data");
	}

	return wood;
}

void writeWood(int id, int sawSpeed, int feedRate)
{
	// Read the existing JSON data from the file
	String fileData = readFileData("/wood.json");

	if (fileData.length() > 0)
	{
		// Deserialize the JSON data into a JsonDocument
		StaticJsonDocument<2048> doc;
		DeserializationError error = deserializeJson(doc, fileData);

		if (error)
		{
			Serial.print("deserializeJson() failed: ");
			Serial.println(error.c_str());
			return;
		}

		// Create a new JsonObject with the wood data
		JsonObject newWood;
		newWood["name"] = woodType[id];
		newWood["code"] = id;
		newWood["sawSpeed"] = sawSpeed;
		newWood["feedRate"] = feedRate;

		// Add the new JsonObject to the JsonDocument
		JsonObject root = doc.as<JsonObject>();
		root[String(id)] = newWood;

		// Serialize the modified JsonDocument back to a string
		String modifiedData;
		serializeJson(doc, modifiedData);

		// Sauvegarder les données modifiées dans le fichier
		saveFileData("/wood.json", modifiedData);
	}
	else
	{
		Serial.println("Error reading JSON file data");
	}
}

void updateWood(int id, int sawSpeed, int feedRate)
{
		// Lire le fichier de configuration
    String fileData = readFileData("/wood.json");

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

        // Save the modified JSON data back to the file
        saveFileData("/wood.json", modifiedData);
    }
    else
    {
        Serial.println("Error reading JSON file data");
    }
}

void vTaskKeypad(void *pvParameters)
{
	keypad.setDebounceTime(20);
	// id variable to store the wood id
	int id = 0;
	// wood variable to store the wood object
	Wood_t wood;

	keypad.addEventListener([](char key) {
		if(!keypad.isPressed(key)) return;
		if(isDigit(key)){
			if(menuState == Prompting) {
				// append the key to the id variable
				id = id * 10 + (key - '0');

			}
		} else if(isAlpha(key)) { // s'assurer que la touche est une lettre (Modes)
			if (menuState == Prompting) {
				// check if the id is valid and get the wood object
				if(id >= 0 && id <= 9) {
					wood = readWood(id);
					menuState = Selecting;
				}
				if (key == 'D') {       // === Mode opération ===
					if(wood.code == -1) {
						id = 0;
						menuState = Prompting;
						return;
					}
					// Commencer le mode opération
					xTaskCreatePinnedToCore(vTaskSawControl, "SawControl", TASK_STACK_SIZE, NULL, 1, &xSawControlHandle, 0);
				} else if(key == 'C') { // === Mode apprentissage ===
						id = 0;
						menuState = Prompting;
						return;
					}
					// Commencer le mode apprentissage
					xTaskCreatePinnedToCore(vTaskLearningMode, "LearningMode", TASK_STACK_SIZE, NULL, 1, &xLearningModeHandle, 0);
				} else if(key == 'B') { // === Mode manuel ===
					if(wood.code == -1) {
						// Commencer le mode manuel
					}
				} else if(key == 'A') { // === Mode modification ===
					if(wood.code == -1) {
						id = 0;
						menuState = Prompting;
						return;
					}
				}
			}
			
		} else {

		}
	});

	while (true)
	{
		// Mettre a jour la liste des touches
		keypad.updateList();
		// Ajouter un délai pour éviter de lire trop rapidement
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/// ===================== Yanick =====================

void writeFileData(const char *filename, const char *data)
{
	// Ouvrir le fichier
	File file = LittleFS.open(filename, "w");

	if (!file)
	{
		Serial.println("une erreur est survenue lors de l'ouverture du fichier");
		return;
	}

	// Écrire les données dans le fichier
	if (file.print(data))
	{
		Serial.println("Données enregistrées dans le fichier");
	}
	else
	{
		Serial.println("Une erreur est survenue lors de l'enregistrement des données dans le fichier");
	}

	// Fermer le fichier
	file.close();
}

/// @brief Lire les données du fichier
/// @param filename Le nom du fichier
/// @return La chaine de caractère contenant les données du fichier
String readFileData(const char *filename)
{
	String fileData = "";

	// Ouvrir le fichier
	File file = LittleFS.open(filename, "r");

	if (!file)
	{
		Serial.println("Une erreur est survenue lors de l'ouverture du fichier");
		return fileData;
	}

	// Lire les données du fichier
	while (file.available())
	{
		fileData += (char)file.read(); // Concaténation de la chaine de caractère
	}

	// Fermer le fichier
	file.close();

	return fileData;
}

/// Programmation du mode modification

void vModificationMode(void *pvParameters)
{
	// Recevoir les paramètres par le clavier numérique

	updateWood(1, 100, 200);
}

/// Mettre à jour le niveau sonore en dB
// https://wiki.dfrobot.com/Gravity__Analog_Sound_Level_Meter_SKU_SEN0232
void vTaskUpdateDb(void *pvParameters)
{
	while (true)
	{
		float fSoundSensorVoltage, fSoundSensorValue;

		fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / 1024.0 * VOLTAGE_REFERENCE;
		fSoundSensorValue = fSoundSensorVoltage * 50.0; // conversion de la tension en décibels

		xQueueSend(xQueueDb, &fSoundSensorValue, portMAX_DELAY);
	}
}

/// Mettre à jour la température et l'humidité ambiante
void vTaskUpdateAmbiantHumidTemp(void *pvParameters)
{
	while (true)
	{
		float uiTemp, uiHumid;

		uiTemp = digitalRead(TEMPERATURE_PIN);
		uiHumid = digitalRead(HUMIDITY_PIN);

		xQueueSend(xQueueAmbiantTemp, &uiTemp, portMAX_DELAY);
		xQueueSend(xQueueAmbiantHumid, &uiHumid, portMAX_DELAY);
	}
}

/// Mettre à jour l'écran LCD
void vTaskUpdateLCD(void *pvParameters)
{
	while (true)
	{
		double dWoodTemp;		 // Définition locale de la température du bois
		float fAmbiantHumid; // Définition locale de l'humidité ambiante
		float fAmbiantTemp;	 // Définition locale de la température ambiante
		float fDb;					 // Définition locale du niveau sonore en dB

		// Récupérer les valeurs des files

		xQueueReceive(xQueueWoodTemp, &dWoodTemp, portMAX_DELAY);
		xQueueReceive(xQueueAmbiantHumid, &fAmbiantHumid, portMAX_DELAY);
		xQueueReceive(xQueueAmbiantTemp, &fAmbiantTemp, portMAX_DELAY);
		xQueueReceive(xQueueDb, &fDb, portMAX_DELAY);

		// Afficher les données sur l'écran LCD
		lcd.print("dB: ");
		lcd.print(&fDb);
		lcd.print("Temp: ");
		lcd.print(&fAmbiantTemp);
		lcd.setCursor(0, 1);
		lcd.print("Humidite: ");
		lcd.print(&fAmbiantHumid);
		lcd.print("C");
	}
}

void setup()
{
	/// ===================== Olivier =====================

	/// ===================== Yanick =====================

	// initialisation des broches
	pinMode(TEMPERATURE_PIN, INPUT);
	pinMode(HUMIDITY_PIN, INPUT);
	pinMode(SOUND_SENSOR_PIN, INPUT);

	// initialisation de l'écran LCD
	Adafruit_LiquidCrystal lcd(0x27, 16, 2);

	// Initialiser l'écran LCD
	lcd.begin(16, 2); // ÉCran d'une résolution de 16x2
	lcd.clear();
	lcd.home();

	// Initialize serial communication
	Serial.begin(115200);

	// Mount the LittleFS file system
	if (!LittleFS.begin())
	{
		Serial.println("An error occurred while mounting the LittleFS file system");
		return;
	}

	/// ===================== Équipe =====================

	// Priorité 1 - Coeur 0 - Taille de pile 2048 - Lecture de la vitesse de la scie
	xTaskCreatePinnedToCore(vTaskreadSawSpeed, "vTaskreadSawSpeed", TASK_STACK_SIZE, NULL, 1, &HandlereadSawSpeed, 0);

	// Priorité 2 - Coeur 1 - Taille de pile 2048 - Lecture de la température et de l'humidité ambiante
	xTaskCreatePinnedToCore(vTaskUpdateAmbiantHumidTemp, "vTaskUpdateAmbiantHumidTemp", TASK_STACK_SIZE, NULL, 2, &HandleUpdateAmbiantHumidTemp, 1);

	// Priorité 2 - Coeur 1 - Taille de pile 2048 - Lecture du niveau sonore en dB
	xTaskCreatePinnedToCore(vTaskUpdateDb, "vTaskUpdateDb", TASK_STACK_SIZE, NULL, 2, &HandleUpdateDb, 1);

	// Priorité 1 - Coeur 1 - Taille de pile 2048 - Affichage des données sur l'écran LCD
	xTaskCreatePinnedToCore(vTaskUpdateLCD, "vTaskUpdateLCD", TASK_STACK_SIZE, NULL, 1, &HandleUpdateLCD, 1);
}

void loop()
{
	// Ne rien faire
}
