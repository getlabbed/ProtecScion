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

typedef enum {MODE_SEL, WOOD_SEL, EDIT_SAW_SPEED, EDIT_FEED_RATE, ACTIVE} MenuState_t;

void vTaskMenu(void *pvParameters);
void vUpdateScreen();
void xStateModeSel();
void xStateWoodSel();
void xStateEditSawSpeed();
void xStateEditFeedRate();
void xStateActive();