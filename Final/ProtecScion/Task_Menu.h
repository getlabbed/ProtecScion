 /**
 * @file Task_Menu.h
 * @author Skkeye
 * @brief Header file for Task_Menu.cpp
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-13 - Initial code entry
 * @date 2023-05-18 - Final code entry
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