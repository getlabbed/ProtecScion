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
  this->xMenuState = Init;

  this->xSemaphoreLCD = xSemaphoreCreateBinary();
  this->xSemaphoreAlert = xSemaphoreCreateBinary();
  xSemaphoreGive(this->xSemaphoreLCD);
  xSemaphoreGive(this->xSemaphoreAlert);

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

void Menu_t::vLCDSetLine(String sText, uint8_t u8Line)
{
  if(xSemaphoreTake(this->xSemaphoreLCD, portMAX_DELAY)) return;

  if (u8Line == 1){
    this->sAlertBuffer = sText;
    if (!xSemaphoreTake(this->xSemaphoreAlert, 0)) {
      xSemaphoreGive(this->xSemaphoreLCD);
      return;
    } else {
      xSemaphoreGive(this->xSemaphoreAlert);
    }
  }

  u8Line = (u8Line == 1) ? 2 : (u8Line == 2) ? 1 : u8Line;

  this->lcd->setCursor(0, u8Line);
  this->lcd->print("                    ");
  this->lcd->setCursor(0, u8Line);
  this->lcd->print(sText);
  xSemaphoreGive(this->xSemaphoreLCD);
}

void Menu_t::vShowPrompt(String sPromptTitle, String sPromptText)
{
  this->vLCDSetLine(sPromptTitle, 0);
  this->vLCDSetLine(sPromptText, 1);
  this->vLCDSetLine("                    ", 2);
  this->vLCDSetLine("                    ", 3);

  // si le texte est plus long que 10 caractères, on le coupe
  if(sPromptText.length() > 10) {
    sPromptText = sPromptText.substring(0, 10);
  }

  sPromptTextBuffer = sPromptText;
}

void Menu_t::vPromptAppend(String sText)
{
  // ajouter le texte au buffer seulement si la longueur est inférieure à 20
  if(sPromptInputBuffer.length() + sText.length() < 20) {
    this->sPromptInputBuffer += sText;
  }

  this->vLCDSetLine(this->sPromptInputBuffer, 1);
}

void Menu_t::vPromptBackspace()
{
  // supprimer le dernier caractère du buffer
  if(this->sPromptInputBuffer.length() > 0) {
    this->sPromptInputBuffer.remove(this->sPromptInputBuffer.length() - 1);
  }

  this->vLCDSetLine(this->sPromptInputBuffer, 1);
}

String Menu_t::sPromptGetInput()
{
  return this->sPromptInputBuffer;
}

void Menu_t::vPromptClearInput()
{
  this->sPromptInputBuffer = "";
}

void Menu_t::vShowMenu(String sTitle)
{
  this->vLCDSetLine(sTitle, 0);
  this->vLCDSetLine("                    ", 1);
  this->vLCDSetLine("                    ", 2);
  this->vLCDSetLine("                    ", 3);
}

void Menu_t::vUpdateInfo(float fSoundLevel, float fTempAmbi, float fHumidityAmbi, float fWoodTemp)
{
  // si le mode est différent de l'opération, on ne fait rien
  if(this->xMenuState != Operation) return;

  // afficher le niveau de son avec 4 décimales utilisant String sur la ligne 1
  this->vLCDSetLine("Son: " + String(fSoundLevel, 4) + "dB", 1);

  // afficher la température et humidité de l'air avec 2 décimales sur la ligne 2
  this->vLCDSetLine("Ambi: " + String(fTempAmbi, 2) + "C " + String(fHumidityAmbi, 2) + "%", 2);

  // afficher la température du bois avec 2 décimale sur la ligne 3
  this->vLCDSetLine("Temp bois: " + String(fWoodTemp, 2) + "C", 3);
}

void Menu_t::vShowAlert(String sAlertText, uint16_t u16DurationMs)
{
  if(!xSemaphoreTake(this->xSemaphoreAlert, portMAX_DELAY)) return;

  this->vLCDSetLine(sAlertText, 1);
  vTaskDelay(pdMS_TO_TICKS(u16DurationMs));
  this->vLCDSetLine(this->sAlertBuffer, 1);

  xSemaphoreGive(this->xSemaphoreAlert);
}