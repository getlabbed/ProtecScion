// THX CHAT GPT FOR CLEAN CODE

#include "Task_Menu.h"
#include "Task_IOFlash.h"

void vTaskMenu(void *pvParameters)
{
	unsigned int uiMode = 0;
	String sChoice;
	char cChoice;
	Wood_t wood;
	MenuState_t state = MODE_SEL;
	const unsigned int zero = 0;

	while (true)
	{
		xQueueReceive(xQueueKeypad, &cChoice, portMAX_DELAY); // Attendre une touche

		if (state == ACTIVE) // Si une touche est appuyée, ARRETER la scie et revenir en mode selection
		{
			xQueueSend(xQueueSawSpeed, &zero, portMAX_DELAY);
			state = MODE_SEL;
		}
		else if (state == MODE_SEL) // Keys 1-4 will select mode, any other key will be ignored
		{
			uiMode = cChoice - '0';
			if (uiMode >= 1 && uiMode <= 4) // valid mode
			{
				// ALWAYS GO TO WOOD SELECT
				state = WOOD_SEL;
				sChoice = "";
			}
		}

		else if (cChoice == '*')
		{

			if (state == WOOD_SEL && uiMode <= 2)
			{
				if (uiMode == 2) xQueueSend(xQueueIRDistance, &(wood.code), portMAX_DELAY);
				xQueueSend(xQueueRequestWood, &(wood.code), portMAX_DELAY);
				xQueueReceive(xQueueReadWood, &wood, portMAX_DELAY);
				xQueueSend(xQueueSawSpeed, &(wood.sawSpeed), portMAX_DELAY);
			}

			else if (state == EDIT_SAW_SPEED)
			{
				// get speed from wood
				// Send speed to saw
				wood.sawSpeed = sChoice.toInt();
			}

			else if (state == EDIT_FEED_RATE)
			{
				// get speed from wood
				// Send speed to saw
				wood.feedRate = sChoice.toInt();
				xQueueSend(xQueueWriteWood, &wood, portMAX_DELAY);
			}

			state = (state == WOOD_SEL && uiMode <= 2) 	? ACTIVE 				: 
							(state == WOOD_SEL)		 							? EDIT_SAW_SPEED 	: 
							(state == EDIT_SAW_SPEED)	 					? EDIT_FEED_RATE 	: 
																										MODE_SEL;
		}

		else
		{
			sChoice.concat(cChoice);
			vSendLCDCommand(sChoice, 2, 0);
			continue;
		}

		if (state == MODE_SEL)
		{
			vSendLCDCommand("1. Mode Operation   ", 0, 0);
			vSendLCDCommand("2. Mode Apprendre   ", 1, 0);
			vSendLCDCommand("3. Mode Manuel      ", 2, 0);
			vSendLCDCommand("4. Mode Modifier    ", 3, 0);
		}

		else if (state == WOOD_SEL)
		{
			vSendLCDCommand("     Input WOOD     ", 0, 0);
			vSendLCDCommand("WOOD ID + WOOD NUM: ", 1, 0);
			vSendLCDCommand("                    ", 2, 0);
			vSendLCDCommand("Press * to confirm  ", 3, 0);
		}

		else if (state == EDIT_SAW_SPEED || state == EDIT_FEED_RATE)
		{
			vSendLCDCommand((uiMode == 3) ? "==  MODE MANUEL   ==" : "== MODE MODIFIER  ==", 0, 0);
			vSendLCDCommand((state == EDIT_SAW_SPEED) ? "Input SAW SPEED     " : "Input FEED RATE     ", 1, 0);
			vSendLCDCommand(sChoice, 2, 0);
			vSendLCDCommand("Press * to confirm  ", 3, 0);
		}

		else if (state == ACTIVE)
		{
			vSendLCDCommand((uiMode == 1) ? "==   OPERATION    ==" : "==   APPRENDRE    ==", 0, 0);
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}