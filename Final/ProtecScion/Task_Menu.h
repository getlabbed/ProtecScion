/**
 * Nom du fichier :
 *  @name Task_Menu.h
 * Description :
 *  @brief Fichier d'en-tête du fichier Task_Menu.cpp.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-13 @author Olivier David Laplante - Entrée initiale du code.
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