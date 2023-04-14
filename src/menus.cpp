/**

 * Nom du fichier :
 *  @name menus.cpp
 * Description :
 *  @brief Code qui gère le menu de l'application ProtecScion
 * restrictions:
 *  Pour type de carte ESP32 Feather
 *  Pour utilisation avec ProtecScion.ino
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante - Entrée initiale du code.
 *  @note aucune(s)
 */

#include "menus.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

Menu_t::Menu_t(uint8_t u8LCDAddress, uint8_t u8LCDColumns, uint8_t u8LCDRows)
{
  this->bAlertActive = false;
  this->xMenuState = Init;
  this->cpAlertBuffer = (char *)malloc(20 * sizeof(char));

  this->xSemaphoreLCD = xSemaphoreCreateBinary();
  xSemaphoreGive(this->xSemaphoreLCD);

  this->lcd = new Adafruit_LiquidCrystal(u8LCDAddress);
  this->lcd->begin(u8LCDColumns, u8LCDRows);
  this->vShowBootScreen();
}

void Menu_t::vShowBootScreen()
{
  this->vLCDSetLine("    ProtecScion    ", 0); // centre le texte sur la ligne
  this->vLCDSetLine("    Version 1.0    ", 1);
  this->vLCDSetLine("Olivier D. Laplante", 2);
  this->vLCDSetLine("    Yanick Labelle ", 3);
}

void Menu_t::vLCDSetLine(char *pcString, uint8_t u8Line)
{
  if (u8Line == 1) u8Line = 2;
  else if (u8Line == 2) u8Line = 1;

  xSemaphoreTake(this->xSemaphoreLCD, portMAX_DELAY);
  if (u8Line == 1)
  {
    strcpy(this->cpAlertBuffer, pcString);
    if (this->bAlertActive) {
      xSemaphoreGive(this->xSemaphoreLCD);
      return;
    }
  }
  this->lcd->setCursor(0, u8Line);
  this->lcd->print("                    ");
  this->lcd->setCursor(0, u8Line);
  this->lcd->print(pcString);
  xSemaphoreGive(this->xSemaphoreLCD);
}

void Menu_t::vShowPrompt(char *pcPromptTitle, char *pcPromptText)
{
  this->vLCDSetLine(pcPromptTitle, 0);
  this->vLCDSetLine(pcPromptText, 1);
  this->vLCDSetLine("                    ", 2);
  this->vLCDSetLine("                    ", 3);
}

void Menu_t::vShowMenu(char *pcTitle)
{
  this->vLCDSetLine(pcTitle, 0);
  this->vLCDSetLine("                    ", 1);
  this->vLCDSetLine("                    ", 2);
  this->vLCDSetLine("                    ", 3);
}

void Menu_t::vUpdateOperation(float fSoundLevel, float fTempAmbi, float fHumidityAmbi, float fWoodTemp)
{
  // Formater et afficher le niveau de son avec 3 décimales
  char cSoundLevel[20];
  sprintf(cSoundLevel, "Sound: %.4f", fSoundLevel);
  this->vLCDSetLine(cSoundLevel, 1);

  // Formater et afficher la température et l'humidité avec 2 decimale sur la même ligne
  char cTempHum[20];
  sprintf(cTempHum, "Ambi: %2.2C %2.2f%", fTempAmbi, fHumidityAmbi);
  this->vLCDSetLine(cTempHum, 2);

  // Formater et afficher la température du bois avec 1 décimale
  char cWoodTemp[20];
  sprintf(cWoodTemp, "Wood: %.1fC", fWoodTemp);
  this->vLCDSetLine(cWoodTemp, 3);
}

void Menu_t::vShowAlert(char *pcAlertText, uint8_t u8DurationMs)
{
  if (this->bAlertActive) return;

  this->bAlertActive = true;
  
  char cAlertTextBuffer[20];
  strncpy(cAlertTextBuffer, pcAlertText, 20);

  this->vLCDSetLine(cAlertTextBuffer, 1);
  vTaskDelay(pdMS_TO_TICKS(u8DurationMs));
  this->vLCDSetLine(this->cpAlertBuffer, 1);

  this->bAlertActive = false;
}