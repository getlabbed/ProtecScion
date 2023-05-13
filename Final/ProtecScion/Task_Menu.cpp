#include "Task_Menu.h"
#include "Task_IOFlash.h"
#include "yasm.h"

unsigned int uiMode = 0;
String sChoice;
char cChoice;
Wood_t wood;
YASM xStateMachine;

void vTaskMenu(void *pvParameters)
{
	xStateMachine.next(xStateModeSel);
	vTaskDelay(pdMS_TO_TICKS(1000));
	vUpdateScreen();
	while (true)
	{
		xQueueReceive(xQueueKeypad, &cChoice, portMAX_DELAY); // Attendre une touche
		xStateMachine.run();
		vUpdateScreen();
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void xStateModeSel()
{
	Serial.println("xStateModeSel");
	if (cChoice >= '1' && cChoice <= '4') // valid mode
	{
		uiMode = cChoice - '0';
		// ALWAYS GO TO WOOD SELECT
		xStateMachine.next(xStateWoodSel);
		sChoice = "";
	}
}

void xStateWoodSel()
{
	Serial.println("xStateWoodSel");
	if (cChoice == '#')
	{
		if (sChoice.length() > 0)
		{
			sChoice.remove(sChoice.length() - 1);
		}
		else
		{
			xStateMachine.next(xStateWoodSel);
		}
	}
	else if (cChoice == '*')
	{
		if (uiMode <= 2)
		{
			if (uiMode == 2) xQueueSend(xQueueIRDistance, &(wood.code), portMAX_DELAY);
			xQueueSend(xQueueRequestWood, &(wood.code), portMAX_DELAY);
			sChoice = "";
			xStateMachine.next(xStateActive);
		}
		else
		{
			sChoice = "";
			xStateMachine.next(xStateEditSawSpeed);
		}
	}
	else if (cChoice >= '0' && cChoice <= '9')
	{
		sChoice.concat(cChoice);
	}
}

void xStateEditSawSpeed()
{
	Serial.println("xStateEditSawSpeed");
	if (cChoice == '#')
	{
		if (sChoice.length() > 0)
		{
			sChoice.remove(sChoice.length() - 1);
		}
		else
		{
			xStateMachine.next(xStateWoodSel);
		}
	}
	else if (cChoice == '*' && sChoice.length() > 1)
	{
		wood.sawSpeed = sChoice.toInt();
		sChoice = "";
		xStateMachine.next(xStateEditFeedRate);
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

void xStateEditFeedRate()
{
	Serial.println("xStateEditFeedRate");
	if (cChoice == '#')
	{
		if (sChoice.length() > 0)
		{
			sChoice.remove(sChoice.length() - 1);
		}
		else
		{
			xStateMachine.next(xStateWoodSel);
		}
	}
	else if (cChoice == '*' && sChoice.length() > 1)
	{
		wood.feedRate = sChoice.toInt();
		sChoice = "";
		xStateMachine.next(xStateModeSel);
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

void xStateActive()
{
	Serial.println("xStateActive");
	unsigned int zero = 0;
	xQueueSend(xQueueSawSpeed, &(zero), portMAX_DELAY);
	xStateMachine.next(xStateModeSel);
}

void vUpdateScreen()
{
	if (xStateMachine.isInState(xStateModeSel))
	{
		vSendLCDCommand("1. Mode Operation  ", 0, 0);
		vSendLCDCommand("2. Mode Apprendre  ", 1, 0);
		vSendLCDCommand("3. Mode Manuel     ", 2, 0);
		vSendLCDCommand("4. Mode Modifier   ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateWoodSel))
	{
		vSendLCDCommand("==   Input WOOD   ==", 0, 0);
		vSendLCDCommand("WOOD ID + WOOD NUM:", 1, 0);
		vSendLCDCommand((sChoice == "") ? "                    " : sChoice, 2, 0);
		vSendLCDCommand("Press * to confirm ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateEditSawSpeed))
	{
		vSendLCDCommand((uiMode == 3) ? "==  MODE MANUEL   ==" : "== MODE MODIFIER  ==", 0, 0);
		vSendLCDCommand("Input SAW SPEED    ", 1, 0);
		vSendLCDCommand((sChoice == "") ? "                    " : sChoice, 2, 0);
		vSendLCDCommand("Press * to confirm ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateEditFeedRate))
	{
		vSendLCDCommand((uiMode == 3) ? "==  MODE MANUEL  ==" : "== MODE MODIFIER ==", 0, 0);
		vSendLCDCommand("Input FEED RATE    ", 1, 0);
		vSendLCDCommand(sChoice, 2, 0);
		vSendLCDCommand("Press * to confirm ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateActive))
	{
		vSendLCDCommand((uiMode == 1) ? "==   OPERATION   ==" : "==   APPRENDRE   ==", 0, 0);
		vSendLCDCommand("                   ", 1, 0);
		vSendLCDCommand("                   ", 2, 0);
		vSendLCDCommand("                   ", 3, 0);
	}
}