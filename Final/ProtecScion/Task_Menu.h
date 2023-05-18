 /**
 * @file Task_Menu.h
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @brief Fichier d'en-tête du fichier Task_Menu.cpp.
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-05-13 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code 
 * 
 */

#include "global.h"

#define ADMIN_PASSWORD "1337"

void vTaskMenu(void *pvParameters);
void vUpdateScreen();
void xStateModeSel();
void xStateWoodSel();
void xStateEditSawSpeed();
void xStateEditFeedRate();
void xStateActive();
void xStateAdminMode();
void xStateAdminPassword();