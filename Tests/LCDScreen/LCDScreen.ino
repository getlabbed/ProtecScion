/**
 * Nom du fichier :
 *  @name LCDScreen.ino
 * Description :
 *  @brief Programme permettant d'afficher un message sur un écran LCD.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 *  pour écran LCD SPI 20x4 (HD44780A00) 
 * Historique :
 *  @date 2023-05-07 @author Yanick Labelle - Entrée initiale du code.
 */

#include "Adafruit_LiquidCrystal.h"

// DAT, CLK, LAT
Adafruit_LiquidCrystal lcd(27, 33, 15);

void setup() {
  lcd.begin(20, 4);
  lcd.clear();
  lcd.print("TEST 1234");
}

void loop() {
  // Ne rien faire
}