 /**
 * @file Task_Menu.cpp
 * @author Skkeye
 * @brief Program used to manage the application menu.
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-05-13 - Initial code entry
 * @date 2023-05-18 - Final code entry 
 * 
 */

#include "Task_Menu.h"
#include "Task_IOFlash.h"
#include "yasm.h"

unsigned int uiMode = 0; // Mode selected | 0: Nothing, 1: Operation, 2: Learning, 3: Manual, 4: Modify
String sChoice;          // User choice
char cChoice;            // User choice
Wood_t xWood;            // Selected wood
YASM xStateMachine;      // State machine

// Main task
void vTaskMenu(void *pvParameters)
{
	xStateMachine.next(xStateModeSel); // Initialize the state machine
	vTaskDelay(pdMS_TO_TICKS(1000));
	vUpdateScreen();                   // Update the screen
	while (true)
	{
		xQueueReceive(xQueueKeypad, &cChoice, portMAX_DELAY); // Wait for a key to be pressed
		xStateMachine.run();                                  // Execute the state machine
		vUpdateScreen();                                      // Update the screen
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void xStateAdminPassword()
{
	if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 4)
	{
		sChoice.concat(cChoice);
	}
	else if (cChoice == '#')
	{
		if (sChoice.length() > 0)
		{
			sChoice = "";
		}
		else
		{
			xStateMachine.next(xStateModeSel);
		}
	}
	else if (cChoice == '*')
	{
		if (sChoice == ADMIN_PASSWORD)
		{
			xStateMachine.next(xStateAdminMode);
		}
		else
		{
			xStateMachine.next(xStateModeSel);
		}
	}
}

void xStateAdminMode()
{
	if (cChoice == '1') // Dump the log
	{
		vSendLog(DUMP,"");
	}
	else if (cChoice == '2') // Dump the log and delete the files (xWood and log)
	{
		vSendLog(DUMP,"PURGE");
	}
	else if (cChoice == '3')
	{

	}
	else if (cChoice == '4')
	{
		LedState_t led_state = LED_OFF;
		xQueueSend(xQueueLED, &led_state, 0);
	}
	else if (cChoice == '5')
	{
		LedState_t led_state = LED_RED;
		xQueueSend(xQueueLED, &led_state, 0);
	}
	else if (cChoice == '6')
	{
		LedState_t led_state = LED_GREEN;
		xQueueSend(xQueueLED, &led_state, 0);
	}
	else if (cChoice == '7')
	{
		LedState_t led_state = LED_BLINK;
		xQueueSend(xQueueLED, &led_state, 0);
	}
	else if (cChoice == '8')
	{
		LedState_t led_state = LED_BOTH;
		xQueueSend(xQueueLED, &led_state, 0);
	}
	else if (cChoice == '9')
	{

	}
	else if (cChoice == '0')
	{

	}
	else if (cChoice == '*')
	{
		xStateMachine.next(xStateModeSel);
	}
	else if (cChoice == '#')
	{
		xStateMachine.next(xStateModeSel);
	}
	else if (cChoice == '*')
	{

	}
}

// Mode selection
void xStateModeSel()
{
	vSendLog(INFO, "Menu: Executed xStateModeSel");
	if (cChoice >= '1' && cChoice <= '4') // If the selected mode is valid
	{
		uiMode = cChoice - '0';             // Convert the choice to an integer
		// ALWAYS GO TO WOOD SELECT
		xStateMachine.next(xStateWoodSel);  // GOTO WOOD SELECT
		sChoice = "";                       // Reset the choice
	}
	else if (cChoice == '*') 
	{
		sChoice = "";
		xStateMachine.next(xStateAdminPassword);
	}
	// send the off state to led
	const LedState_t led_state = LED_OFF;
	xQueueSend(xQueueLED, &led_state, 0);
}

// Wood selection
void xStateWoodSel()
{
	vSendLog(INFO, "Menu: Executed xStateWoodSel");
	if (cChoice == '#')
	{
		if (sChoice.length() > 0)
		{
			sChoice = "";
		}
		else
		{
			xStateMachine.next(xStateModeSel);
		}
	}
	else if (cChoice == '*')
	{
		xWood.code = sChoice.toInt();
		if (uiMode <= 2)
		{
			xQueueSend(xQueueLearningControl, &(xWood.code), portMAX_DELAY); // TBD
			xQueueSend(xQueueRequestWood, &(xWood.code), portMAX_DELAY);
			xQueueReceive(xQueueReadWood, &xWood, portMAX_DELAY);
			unsigned int uiSawSpeed = (xWood.sawSpeed * 4096) / 100;
			xQueueSend(xQueueSawSpeed, &uiSawSpeed, portMAX_DELAY);
			sChoice = "";
			xStateMachine.next(xStateActive);
		}
		else
		{
			if (uiMode == 4)
			{
				xQueueSend(xQueueRequestWood, &(xWood.code), portMAX_DELAY);
				xQueueReceive(xQueueReadWood, &xWood, portMAX_DELAY);
			}
			sChoice = (uiMode == 4) ? String(xWood.sawSpeed) : "";
			xStateMachine.next(xStateEditSawSpeed);
		}
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

// Saw speed editing
void xStateEditSawSpeed()
{
	vSendLog(INFO, "Menu: Executed xStateEditSawSpeed");
	if (cChoice == '#')
	{
		if (sChoice.length() > 0)
		{
			sChoice = "";
		}
		else
		{
			xStateMachine.next(xStateModeSel);
		}
	}
	else if (cChoice == '*' && sChoice.length() > 1)
	{
		if (sChoice.toInt() > 100) sChoice = "100";
		xWood.sawSpeed = sChoice.toInt();
		sChoice = (uiMode == 4) ? String(xWood.feedRate, 0) : "";
		xStateMachine.next(xStateEditFeedRate);
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

// Feed rate editing
void xStateEditFeedRate()
{
	vSendLog(INFO, "Menu: Executed xStateEditFeedRate");
	if (cChoice == '#')
	{
		if (sChoice.length() > 0)
		{
			sChoice = "";
		}
		else
		{
			xStateMachine.next(xStateModeSel);
		}
	}
	else if (cChoice == '*' && sChoice.length() > 1)
	{
		if (sChoice.toInt() > 300) sChoice = "300";
		xWood.feedRate = sChoice.toInt();
		xQueueSend(xQueueWriteWood, &xWood, portMAX_DELAY);
		xSemaphoreTake(xSemaphoreLog, portMAX_DELAY);
		sChoice = "";
		xStateMachine.next(xStateModeSel);
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

// Active state
void xStateActive()
{
	if (cChoice != 0)
	{
		// send the off state to led
		const LedState_t led_state = LED_OFF;
		xQueueSend(xQueueLED, &led_state, 0);
	}
	vSendLog(INFO, "Menu: Executed xStateActive");
	int flag = uiMode - 2; // -1 = Operation mode, 0 = Learning mode
	unsigned int uiZero = 0;
	xQueueSend(xQueueLearningControl, &flag, portMAX_DELAY); // Stop reading the distance
	xQueueSend(xQueueSawSpeed, &uiZero, portMAX_DELAY); // Stop the saw
	xStateMachine.next(xStateModeSel);
}

// Function that updates the screen
void vUpdateScreen()
{
	if (xStateMachine.isInState(xStateModeSel))
	{
		vSendLCDCommand("1. Operation Mode  ", 0, 0);
		vSendLCDCommand("2. Learning Mode   ", 1, 0);
		vSendLCDCommand("3. Manual Mode     ", 2, 0);
		vSendLCDCommand("4. Modify Mode     ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateWoodSel))
	{
		vSendLCDCommand("==  Wood Choice  ==", 0, 0);
		vSendLCDCommand("ID then number:    ", 1, 0);
		vSendLCDCommand((sChoice == "") ? "                   " : sChoice, 2, 0);
		vSendLCDCommand("\"*\" To Confirm     ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateEditSawSpeed))
	{
		vSendLCDCommand((uiMode == 3) ? "==  MANUAL MODE  ==" : "==  MODIFY MODE  ==", 0, 0);
		vSendLCDCommand("Saw Speed:         ", 1, 0);
		vSendLCDCommand((sChoice == "") ? "                   " : sChoice, 2, 0);
		vSendLCDCommand("\"*\" To Confirm     ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateEditFeedRate))
	{
		vSendLCDCommand((uiMode == 3) ? "==  MANUAL MODE  ==" : "==  MODIFY MODE  ==", 0, 0);
		vSendLCDCommand("Wood speed:        ", 1, 0);
		vSendLCDCommand((sChoice == "") ? "                   " : sChoice, 2, 0);
		vSendLCDCommand("\"*\" To Confirm     ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateActive))
	{
		vSendLCDCommand((uiMode == 1) ? "==   OPERATION   ==" : "==   LEARNING    ==", 0, 0);
		vSendLCDCommand("                   ", 1, 0);
		vSendLCDCommand("                   ", 2, 0);
		vSendLCDCommand("                   ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateAdminMode))
	{
		vSendLCDCommand("==   ADMIN MODE  ==", 0, 0);
		vSendLCDCommand("1. Logging         ", 1, 0);
		vSendLCDCommand("2. Factory Reset   ", 2, 0);
		vSendLCDCommand("3.                 ", 3, 0);
	}
}