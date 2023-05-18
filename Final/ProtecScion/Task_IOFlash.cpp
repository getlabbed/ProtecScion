/**
 * @file Task_IOFlash.cpp
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Fichier avec fonctionnalités de lecture et écriture de la mémoire flash
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-04-30 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code
 * 
 */

#include "Task_IOFlash.h"

#include "FS.h"
#include <ArduinoJson.h>

/**
 * @brief Tache de lecture et écriture de la mémoire flash
 * 
 * @param pvParameters - Non utilisé
 */
void vTaskIOFlash(void *pvParameters)
{
	Wood_t xWood;
	Log_t xMessage;
	unsigned int uiRequestWoodID;

	File xFile = SPIFFS.open("/xWood.json", FILE_READ);
	if (!xFile || xFile.size() == 0)
	{
		xFile.close();
		writeEmptyFile();
	}
	else
	{
		xFile.close();
	}

	while (1)
	{
		// read from the queue
		if (xQueueReceive(xQueueRequestWood, &uiRequestWoodID, 0) == pdTRUE)
		{
			readWood(xWood, uiRequestWoodID);
			xQueueSend(xQueueReadWood, &xWood, 0);
		}

		if (xQueueReceive(xQueueWriteWood, &xWood, 0) == pdTRUE)
		{

			writeWood(xWood.code, xWood.sawSpeed, xWood.feedRate);
			xSemaphoreGive(xSemaphoreLog);
		}

		if (xQueueReceive(xQueueLog, &xMessage, 0) == pdTRUE)
		{
			// if (xMessage.level >= DEBUG && xMessage.level <= ERROR) {
			// 	String myString = xMessage.message.substring(0, 15);
			// 	vSendLCDCommand(myString, 0, 3000);
			// }

			if (xMessage.level == DUMP)
			{
				dumpLog();
				vDumpWood(xMessage.message == "PURGE");
				xSemaphoreGive(xSemaphoreLog);
				continue;
			}
			logMessage(xMessage);
			xSemaphoreGive(xSemaphoreLog);
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

/**
 * @brief Fonction pour écrire un fichier wood.json vierge
 * 
 * @author Olivier David Laplante (skkeye@gmail.com)
 */
void writeEmptyFile()
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		logMessage(Log_t{ERROR, "WRITE_FILE: Could not take SPIFFS semaphore"});
		return;
	}

	File xFile = SPIFFS.open("/xWood.json", "w");

	if (xFile.print("{}"))
	{
		xFile.close();
		xSemaphoreGive(xSemaphoreSPI);
		vTaskDelay(10 / portTICK_PERIOD_MS);
		logMessage(Log_t{INFO, "WRITE_FILE: File written"});
	}
	else
	{
		xFile.close();
		xSemaphoreGive(xSemaphoreSPI);
		vTaskDelay(10 / portTICK_PERIOD_MS);
		logMessage(Log_t{ERROR, "WRITE_FILE: Write failed"});
	}
}

/**
 * @brief Fonction pour lire un objet wood dans le fichier wood.json
 * @author Yanick Labelle (getlabbed@proton.me)
 * 
 * @param xWood - Reference de l'objet wood à lire
 * @param iCode - ID du bois à écrire
 */
void readWood(Wood_t &xWood, int iCode)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		logMessage(Log_t{WARNING, "READ_WOOD: Could not take SPIFFS semaphore"});
		return;
	}

	File xFile = SPIFFS.open("/xWood.json", "r");
	// read the file in a variable
	String xFileData;
	while (xFile.available())
	{
		xFileData += char(xFile.read());
	}
	xFile.close();
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);

	if (xFileData.length() > 0) // Si le fichier n'est pas vide
	{
		StaticJsonDocument<2048> xDoc; // ajuster la taille du fichier selon la quantité de bois
		DeserializationError xError = deserializeJson(xDoc, xFileData);

		if (xError)
		{
			String sError = "READ_WOOD: deserializeJson() failed:\n" + String(xError.c_str());
			logMessage(Log_t{ERROR, sError});
			return;
		}

		// if the object is not found, return
		if (xDoc.containsKey(String(iCode)) == false)
		{
			logMessage(Log_t{WARNING, "READ_WOOD: Object not found"});
			return;
		}

		JsonObject obj = xDoc[String(iCode)];
		xWood.code = obj["code"].as<int>();
		xWood.sawSpeed = obj["sawSpeed"].as<int>();
		xWood.feedRate = obj["feedRate"].as<int>();
	}
	else
	{
		logMessage(Log_t{ERROR, "READ_WOOD: Error reading JSON file data"});
	}
}

/**
 * @brief Fonction pour écrire un objet wood dans le fichier wood.json
 * @author Yanick Labelle
 * 
 * @param iCode - ID du bois à écrire
 * @param sawSpeed - Vitesse de la scie
 * @param feedRate - Vitesse d'avancement
 */
void writeWood(int iCode, int sawSpeed, int feedRate)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		logMessage(Log_t{WARNING, "WRITE_WOOD: Could not take SPIFFS semaphore"});
		return;
	}
	// Lire les données du fichier JSON
	File xFile = SPIFFS.open("/xWood.json", "r");
	// read the file in a variable
	String xFileData;
	while (xFile.available())
	{
		xFileData += char(xFile.read());
	}
	xFile.close();
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);

	if (xFileData.length() > 0)
	{
		// Ajuster la taille du fichier selon la quantité de bois
		StaticJsonDocument<2048> xDoc;
		DeserializationError error = deserializeJson(xDoc, xFileData);

		if (error)
		{
			String error = "WRITE_WOOD: deserializeJson() failed:\n" + String(error.c_str());
			logMessage(Log_t{ERROR, error});
			return;
		}

		xDoc[String(iCode)]["code"] = iCode;
		xDoc[String(iCode)]["sawSpeed"] = sawSpeed;
		xDoc[String(iCode)]["feedRate"] = feedRate;

		// Serialize the modified JsonDocument back to a string
		String modifiedData;
		serializeJson(xDoc, modifiedData);

		if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
		{
			logMessage(Log_t{WARNING, "WRITE_WOOD: Could not take SPIFFS semaphore"});
			return;
		}

		File xFile = SPIFFS.open("/xWood.json", "w");
		if (!xFile)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			xSemaphoreGive(xSemaphoreSPI);
			logMessage(Log_t{ERROR, "WRITE_WOOD: Failed to open file for writing"});
			return;
		}
		if (xFile.print(modifiedData))
		{
			xFile.close();
			vTaskDelay(10 / portTICK_PERIOD_MS);
			xSemaphoreGive(xSemaphoreSPI);
			logMessage(Log_t{INFO, "WRITE_WOOD: File written"});
		}
		else
		{
			xFile.close();
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

/**
 * @brief Fonction pour écrire un objet log dans le fichier log.txt
 * @author Olivier David Laplante
 * 
 * @param xLogMsg - Objet log à écrire
 */
void logMessage(Log_t xLogMsg)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		return;
	}
	File xFile = SPIFFS.open("/log.txt", "a");
	if (!xFile)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		xSemaphoreGive(xSemaphoreSPI);
		return;
	}

	// Regarder si le fichier est plus grand que 100kB
	if (xFile.size() > 100000)
	{
		xFile.close();
		SPIFFS.remove("/log.txt");
		xFile = SPIFFS.open("/log.txt", "a");
		if (!xFile)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			xSemaphoreGive(xSemaphoreSPI);
			return;
		}
	}

	// make a string with the uptime in hh:mm:ss:msmsms format
	String uptime = String(millis() / 3600000) + ":" + String((millis() / 60000) % 60) + ":" + String((millis() / 1000) % 60) + ":" + String(millis() % 1000);
	// format the message like a linux kernel log [uptime] logLevel: message
	String message = "[" + uptime + "] " + logLevelString[xLogMsg.level] + ": " + xLogMsg.message;
	xFile.println(message);
	xFile.close();
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);
}

/**
 * @brief Fonction pour écrire les logs dans le port sériel
 * @author Olivier David Laplante
 * 
 */
void dumpLog()
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		return;
	}

	File xFile = SPIFFS.open("/log.txt", "r");
	if (!xFile)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		xSemaphoreGive(xSemaphoreSPI);
		return;
	}

	// read the file line by line
	while (xFile.available())
	{
		xSemaphoreTake(xSemaphoreSerial, portMAX_DELAY);
		Serial.println(xFile.readStringUntil('\n'));
		xSemaphoreGive(xSemaphoreSerial);
	}
	xFile.close();

	// delete the file
	SPIFFS.remove("/log.txt");
	xSemaphoreGive(xSemaphoreSPI);
}

/**
 * @brief Fonction pour écrire le fichier wood.json dans le port sériel
 * @author Olivier David Laplante
 * 
 * @param bPurge - Booléen pour supprimer le fichier après l'avoir écrit
 */
void vDumpWood(bool bPurge)
{
	if (xSemaphoreTake(xSemaphoreSPI, portMAX_DELAY) == pdFAIL)
	{
		return;
	}

	File xFile = SPIFFS.open("/xWood.json", "r");
	if (!xFile)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		xSemaphoreGive(xSemaphoreSPI);
		return;
	}

	// read the file line by line
	while (xFile.available())
	{
		xSemaphoreTake(xSemaphoreSerial, portMAX_DELAY);
		Serial.write(xFile.read());
		xSemaphoreGive(xSemaphoreSerial);
	}
	xFile.close();

	if (bPurge)
	{
		// delete the file
		SPIFFS.remove("/xWood.json");
		ESP.restart();
	}
	vTaskDelay(10 / portTICK_PERIOD_MS);
	xSemaphoreGive(xSemaphoreSPI);
}