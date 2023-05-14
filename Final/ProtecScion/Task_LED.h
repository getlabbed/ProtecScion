/**
 * Nom du fichier :
 *  @name Task_Menu.cpp
 * Description :
 *  @brief Code permettant de gérer une DEL.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-14 @author Olivier David Laplante - Entrée initiale du code.
 */

#include "global.h"

#define PIN_LED 27

void vTaskLED(void *pvParameters);