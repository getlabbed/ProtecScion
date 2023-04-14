/**
 * Nom du fichier :
 *  @name ProtecScion.ino
 * Description :
 *  @brief Programme permettant d'améliorer la fonctionnalité d'un banc de scie.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante @author Yanick Labelle - Entrée initiale du code.
 *  @note Utilisation de Doxygen pour la documentation:
 *         https://www.doxygen.nl/manual/docblocks.html
 */

// ======================================== Fichiers d'inclusion =================================================

// Importation du fichier d'inclusion global
#include "src/global.h"

// Importation des bibliothèques
#include <Adafruit_MLX90614.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <Adafruit_NeoPixel.h>

#include <Keypad.h>
#include <PID_v1.h>
#include <ArduinoJson.h>

#include "src/menus.h";

// Importation des bibliothèques pour le système de fichiers
#include <FS.h>
#include <LittleFS.h>

// Importation des bibliothèques pour le debug
#include <TinyConsole.h>
#include <TinyStreaming.h>
#include <TinyString.h>

// Déclaration des Handlers

TaskHandle_t HandleKeypad;
TaskHandle_t HandleSawControl;
TaskHandle_t HandleLearningMode;
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

int sumSawSpeed, readingCount; // Définition des variables pour le calcul de la vitesse moyenne de la scie

// ------------------------------------ Initialisation du Clavier numérique ---------------------------------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
		{'1', '2', '3', 'A'},
		{'4', '5', '6', 'B'},
		{'7', '8', '9', 'C'},
		{'*', '0', '#', 'D'}};

// Types de bois disponibles
const String woodType[] = {"Sapin", "Pin", "Noyer", "Bouleau", "Chêne", "Érable", "Frêne", "Merisier", "Mélèze", "Épinette"};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ------------------------------------ Initialisation du menu ---------------------------------------
Menu_t *menu;

/**
 * Nom de la tâche :
 *  @name vCreateTaskShowAlert
 * Description de la tâche :
 *  @brief Tâche permettant l'initialisation de la tâche vTaskShowAlert
 *
 * Paramètre(s) d'entrée :
 *  @param sAlertText Texte de l'alerte
 *  @param uiAlertDuration Durée de l'alerte
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante - Entrée initiale du code.
 */
void vCreateTaskShowAlert(String sAlertText, uint8_t uiAlertDuration)
{
	AlertParams_t *alertParams = (AlertParams_t *)malloc(sizeof(AlertParams_t));
	alertParams->sText = sAlertText;
	alertParams->u8DurationMs = uiAlertDuration;
	xTaskCreatePinnedToCore(vTaskShowAlert, "vTaskShowAlert", TASK_STACK_SIZE, alertParams, 2, NULL, 1);
}

/**
 * Nom de la tâche :
 *  @name vTaskShowAlert
 * Description de la tâche	 :
 *  @brief Tâche permettant d'afficher une alerte sur l'écran LCD utisant "menus.h"
 *
 * Paramètre(s) d'entrée :
 *  @param alertParams Paramètres de l'alerte
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante - Entrée initiale du code.
 */
void vTaskShowAlert(AlertParams_t *alertParams)
{
	menu->showAlert(alertParams->sText, alertParams->u8DurationMs);
}


/**
 * Nom de la tâche :
 *  @name vTasklightIndicator
 * Description de la tâche :
 *  @brief Tâche permettant de mettre à jour l'indicateur lumineux
 *
 * Paramètre(s) d'entrée :
 *  @param pvParameters Paramètres de la tâche
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante - Entrée initiale du code.
 */

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
		if (*color == Red) // Si la couleur est rouge
		{
			xLightIndicator.setPixelColor(0, xLightIndicator.Color(255, 0, 0));
		}
		else if (*color == Green) // si la couleur est verte
		{
			xLightIndicator.setPixelColor(0, xLightIndicator.Color(0, 255, 0));
		}
		else if (*color == Blue) // si la couleur est bleu
		{
			xLightIndicator.setPixelColor(0, xLightIndicator.Color(0, 0, 255));
		}
	}
}

/**
 * Nom de la tâche :
 *  @name vTasklearningMode
 * Description de la tâche :
 *  @brief Tâche permettant d'activer le mode apprentissage
 *
 * Paramètre(s) d'entrée :
 *  @param pvParameters Paramètres de la tâche
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note Cette tâche est appelée lorsque l'utilisateur appuie sur le bouton «C»
 *
 * Historique :
 *  @date 2021-04-13 @author Yanick Labelle initiale du code.
 */
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

/**
 * Nom de la tâche :
 *  @name vTaskSawControl
 * Description de la tâche :
 *  @brief Tâche permettant de contrôler la vitesse de la scie (Mode opération)
 *
 *  Paramètre(s) d'entrée :
 *  1. @param pvParameters : paramètres de la tâches
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note Code inspiré de :
 *        https://github.com/br3ttb/Arduino-PID-Library/blob/master/examples/PID_Basic/PID_Basic.ino
 *
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante - Entrée initiale du code.
 */
void vTaskSawControl(void *pvParameters)
{
	// Aller chercher la valeur de la vitesse de la scie dans le fichier de configuration

	dWantedSawSpeed = readWood(id).sawSpeed; // Définir la vitesse désirée de la scie (0-4095)
	dfeedRate = readWood(id).feedRate;	     // Définir la vitesse d'avancement du bois

	// Configuration du PID
	myPID.SetMode(AUTOMATIC);
	myPID.SetOutputLimits(0, 4095);
	myPID.SetSampleTime(50);

	while (true)
	{
		dSawInput = analogRead(SAW_INPUT_PIN); // Lire la vitese de la scie (0-4095)

		// Vérification de critères (anti-recul)

		myPID.Compute(); // Exécuter le PID

		setSawSpeed(dSawOutput); // Définir la vitesse de la scie (0-4095)

		// LOG SYSTEM
	}
}

/**
 * Nom de la fonction :
 *  @name readWood
 * Description de la fonction :
 *  @brief Fonction permettant de convertir un id de bois en un objet de type Wood_t
 *
 *  Paramètre(s) d'entrée :
 *  1. @param id Id du bois
 *
 * Valeur de retour :
 *  @return wood_t
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Yanick Labelle - Entrée initiale du code.
 */
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
			else{
				return false; // Retourner faux si le bois n'est pas trouvé
			}
		}
	}
	else
	{
		Serial.println("Error reading JSON file data");
	}

	return wood;
}

/**
 * Nom de la fonction :
 *  @name writeWood
 * Description de la fonction :
 *  @brief Fonction permettant d'écrire les données d'un bois
 *
 *  Paramètre(s) d'entrée :
 *  1. @param id Id du bois
 *  2. @param sawSpeed Vitesse de la scie
 *  3. @param feedRate Vitesse d'avancement du bois
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Yanick Labelle - Entrée initiale du code.
 */
void writeWood(int id, int sawSpeed, int feedRate)
{
	// Lire les données du fichier JSON
	String fileData = readFileData("/wood.json");

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

/**
 * Nom de la fonction :
 *  @name updateWood
 * Description de la fonction :
 *  @brief Fonction permettant de mettre à jour les données d'un bois
 *
 *  Paramètre(s) d'entrée :
 *  1. @param id Id du bois
 *  2. @param sawSpeed Vitesse de la scie
 *  3. @param feedRate Vitesse d'avancement du bois
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Yanick Labelle - Entrée initiale du code.
 */
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

void v

/**
 * Nom de la tâche :
 *  @name vTaskKeypad
 * Description de la tâche :
 *  @brief Tâche permettant de gérer le clavier
 *
 *  Paramètre(s) d'entrée :
 *  1. @param pvParameters : paramètres de la tâches
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante - Entrée initiale du code.
 */
void vTaskKeypad(void *pvParameters)
{
	keypad.setDebounceTime(20);
	
	Wood_t wood;

	keypad.addEventListener([](char key) {
		if (!keypad.isPressed(key))
			return;

		if (isDigit(key)) {
			if (menu->MenuState == PromptWoodId || menu->MenuState == PromptSawSpeed || menu->MenuState == PromptFeedRate) {
				menu->vPromptAppend(key);
			}
		} else if (isAlpha(key)) { // s'assurer que la touche est une lettre (Modes) (A, B, C, D)
			// si la touche est D et que le menu est en mode promptSawSpeed ou promptFeedRate

			// si le menu est en mode prompt
			if (menu->MenuState == PromptWoodId) {
				wood = readWood(menu->sPromptGetInput().toInt());
				// si la touche est A
				if (key == 'A') { // Mode modification
					if(!wood) {
						vCreateTaskShowAlert("Bois introuvable", 1000);
						return;
					}
					menu->vPromptClear();
					menu->MenuState = PromptSawSpeed;
				} else if (key == 'B') { // Mode manuel
					if(wood) {
						vCreateTaskShowAlert("Bois déjà existant", 1000);
						return;
					}
					wood = {menu->sPromptGetInput().toInt(), "CREATE", 0, 0};
					menu->vPromptClear();
					menu->MenuState = PromptSawSpeed;
				} else if (key == 'C') { // Mode apprentissage
				  if(!wood) {
						vCreateTaskShowAlert("Bois introuvable", 1000);
						return;
					}
					
					menu->vPromptClear();

					menu->vShowMenu(wood.name);
					wood = false;

					// créer la tâche d'apprentissage
					xTaskCreatePinnedToCore(vTasklearningMode, "Mode apprentissage", TASK_STACK_SIZE, NULL, 2, &HandleLearningMode, 0);	
					menu->MenuState = Learning;
				} else if (key == 'D') { // Mode opération
					if(!wood) {
						vCreateTaskShowAlert("Bois introuvable", 1000);
						return;
					}
					menu->vPromptClear();
				
					menu->vShowMenu(wood.name);
					wood = false;
					// créer la tâche d'opération
					xTaskCreatePinnedToCore(vTaskSawControl, "Mode opération", TASK_STACK_SIZE, NULL, 2, &HandleSawControl, 0);
					menu->MenuState = Operation;
				}
			} else if(menu->MenuState == PromptSawSpeed) {
				// si la touche est D, passer au promptFeedRate
				if (key == 'D') {
					// verifier si le prompt est valide
					if(menu->sPromptGetInput().toInt() < 0 || menu->sPromptGetInput().toInt() > 100) {
						vCreateTaskShowAlert("Valeur invalide", 1000);
						return;
					}
					
					// modifier le bois
					wood.sawSpeed = menu->sPromptGetInput().toInt();
					menu->vPromptClear();

					menu->MenuState = PromptFeedRate;
				} else if (key == 'A') {
					wood = false;
					menu->vPromptClear();
					menu->MenuState = PromptWoodId;
				}
			} else if(menu->MenuState == PromptFeedRate) {
				// si la touche est D, passer au promptFeedRate
				if (key == 'D') {
					// verifier si le prompt est valide
					if(menu->sPromptGetInput().toInt() < 0 || menu->sPromptGetInput().toInt() > 100) {
						vCreateTaskShowAlert("Valeur invalide", 1000);
						return;
					}

					// modifier le bois
					wood.feedRate = menu->sPromptGetInput().toInt();
					menu->vPromptClear();

					// sauvegarder le bois
					if (wood.name == "CREATE") {
						writeWood(wood.id, wood.sawSpeed, wood.feedRate);
					} else {
						updateWood(wood.id, wood.sawSpeed, wood.feedRate);
					}
					wood = false;
				} else if (key == 'A') {
					wood = false;
					menu->vPromptClear();
					menu->MenuState = PromptSawSpeed;
				}
			}

		} else {
			if(key == '*') { // si la touche clear est appuyée et que le menu est en mode prompt
				if (menu->MenuState == PromptWoodId || menu->MenuState == PromptSawSpeed || menu->MenuState == PromptFeedRate) {
					menu->vPromptBackspace(); // retirer le dernier caractère du prompt
				}
			} else {
				// selon le mode du menu, afficher une alerte avec vCreateTaskShowAlert d'une seconde expliquant le prompt
				if (menu->MenuState == PromptWoodId) {
					vCreateTaskShowAlert("Entrez l'ID du bois", 1000);
				} else if (menu->MenuState == PromptSawSpeed) {
					vCreateTaskShowAlert("Vitesse scie", 1000);
				} else if (menu->MenuState == PromptFeedRate) {
					vCreateTaskShowAlert("Vitesse d'avancement", 1000);
				}
			}
		}
	});

	// Attendre 3 secondes avant de commencer la tâche
	vTaskDelay(pdMS_TO_TICKS(3000));
	menu->vShowPrompt("ID du bois: ", "ID: ");
	menu->xMenuState = PromptWoodId;

	while (true)
	{
		// Mettre a jour la liste des touches
		keypad.updateList();
		// Ajouter un délai pour éviter de lire trop rapidement
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/// ===================== Yanick =====================

/**
 * Nom de la fonction :
 *  @name writeFileData
 * Description de la fonction :
 *  @brief Fontion pour ajouter des données dans un fichier JSON
 *
 *  Paramètre(s) d'entrée :
 *  1. @param filename : le nom du fichier
 *  2. @param data     : les données à ajouter
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Yanick Labelle - Entrée initiale du code.
 */
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

/**
 * Nom de la fonction :
 *  @name readFileData
 * Description de la fonction :
 *  @brief Fontion pour lire les données d'un fichier JSON
 *
 *  Paramètre(s) d'entrée :
 *  1. @param filename : le nom du fichier
 *
 * Valeur de retour :
 *  @return String : les données du fichier
 * Note(s) :
 *  @note aucune(s)
 *
 * Historique :
 *  @date 2021-04-13 @author Yanick Labelle - Entrée initiale du code.
 */
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

/// Mettre à jour l'écran LCD
void vTaskUpdateLCD(void *pvParameters)
{

	Adafruit_MLX90614 xIRWoodTempReader = Adafruit_MLX90614();
	double dWoodTemp = 20;

	float fAmbiantHumid; // Définition locale de l'humidité ambiante
	float fAmbiantTemp;	 // Définition locale de la température ambiante
	float fDb;			 // Définition locale du niveau sonore en dB
	float fSoundSensorVoltage, fSoundSensorValue;
	float uiTemp, uiHumid;
	if (!xIRWoodTempReader.begin())
	{
		// Erreur de communication avec le capteur
	}	

	while (true)
	{
		// Récupérer la température du bois
		dWoodTemp = xIRWoodTempReader.readObjectTempC();

		// Récupérer l'humidité et la température ambiante
		uiTemp = digitalRead(TEMPERATURE_PIN);
		uiHumid = digitalRead(HUMIDITY_PIN);

		// Récupérer le niveau sonore en dB

		fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / 1024.0 * VOLTAGE_REFERENCE;
		fSoundSensorValue = fSoundSensorVoltage * 50.0; // conversion de la tension en décibels

		// Afficher les valeurs sur l'écran LCD
		menu->vUpdateInfo((fDb, fAmbiantHumid, fAmbiantTemp, (float)dWoodTemp));

		// Attendre 1 seconde
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void setup()
{
	// ===================== Olivier =====================
	// Initialisation de la communication sérielle
	Serial.begin(115200);

	menu = new Menu(0x27, 20, 4);

	// Initialisation des broches
	pinMode(TEMPERATURE_PIN, INPUT);
	pinMode(HUMIDITY_PIN, INPUT);
	pinMode(SOUND_SENSOR_PIN, INPUT);

	// Monter le système de fichiers LittleFS
	if (!LittleFS.begin())
	{
		Serial.println("An error occurred while mounting the LittleFS file system");
		return;
	}

	/// ===================== Équipe =====================

	// Priorité 1 - Coeur 0 - Taille de pile 2048 - Lecture de la vitesse de la scie
	xTaskCreatePinnedToCore(vTaskKeypad, "Tâche Keypad", TASK_STACK_SIZE, NULL, 1, &HandleKeypad, 0);	
}

void loop()
{
	// Ne rien faire
}
