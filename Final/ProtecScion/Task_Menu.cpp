/**
 * Nom du fichier :
 *  @name Task_Menu.cpp
 * Description :
 *  @brief Code permettant de gérer le menu de l'application.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-13 @author Olivier David Laplante - Entrée initiale du code.
 */

#include "Task_Menu.h"
#include "Task_IOFlash.h"
#include "yasm.h"

unsigned int uiMode = 0; // Mode sélectionné | 0: Rien, 1: Opération, 2: Apprendre, 3: Manuel, 4: Modifier
String sChoice;          // Choix de l'utilisateur
char cChoice;            // choix de l'utilisateur
Wood_t wood;             // Bois sélectionné
YASM xStateMachine;      // Machine à état

// Tache principale
void vTaskMenu(void *pvParameters)
{
	xStateMachine.next(xStateModeSel); // Initialiser la machine à état
	vTaskDelay(pdMS_TO_TICKS(1000));
	vUpdateScreen();                   // Mettre à jour l'écran
	while (true)
	{
		xQueueReceive(xQueueKeypad, &cChoice, portMAX_DELAY); // Attendre une touche
		xStateMachine.run();                                  // Exécuter la machine à état
		vUpdateScreen();                                      // Mettre à jour l'écran
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void xStateAdminMode()
{
	if (cChoice == '1') // Dump Log
	{
		vSendLog(DUMP,"");
	}
	else if (cChoice == '2')
	{
		
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

// sélection du mode
void xStateModeSel()
{
	vSendLog(INFO, "Menu: Executed xStateModeSel");
	if (cChoice >= '1' && cChoice <= '4') // Si le mode choisit est valide
	{
		uiMode = cChoice - '0';             // Convertir le choix en entier
		// ALWAYS GO TO WOOD SELECT
		xStateMachine.next(xStateWoodSel);  // Aller à la sélection du bois
		sChoice = "";                       // Réinitialiser le choix
	}
	else if (cChoice == '*') 
	{
		xStateMachine.next(xStateAdminMode);
	}
}

// sélection du bois
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
		if (uiMode <= 2)
		{
			if (uiMode == 2)
			{
				xQueueSend(xQueueApprentissageControl, &(wood.code), portMAX_DELAY); // TBD
			}
			xQueueSend(xQueueRequestWood, &(wood.code), portMAX_DELAY);
			xQueueReceive(xQueueReadWood, &wood, portMAX_DELAY);
			sChoice = "";
			xStateMachine.next(xStateActive);
		}
		else
		{
			if (uiMode == 4)
			{
				xQueueSend(xQueueRequestWood, &(wood.code), portMAX_DELAY);
				xQueueReceive(xQueueReadWood, &wood, portMAX_DELAY);
			}
			wood.code = sChoice.toInt();
			sChoice = (uiMode == 4) ? String(wood.sawSpeed) : "";
			xStateMachine.next(xStateEditSawSpeed);
		}
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

// Édition de la vitesse de la scie
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
		wood.sawSpeed = sChoice.toInt();
		sChoice = (uiMode == 4) ? String(wood.feedRate) : "";
		xStateMachine.next(xStateEditFeedRate);
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

// Édition de la vitesse d'avancement
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
		wood.feedRate = sChoice.toInt();
		xQueueSend(xQueueWriteWood, &wood, portMAX_DELAY);
		sChoice = "";
		xStateMachine.next(xStateModeSel);
	}
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

// Mode actif
void xStateActive()
{
	vSendLog(INFO, "Menu: Executed xStateActive");
	unsigned int zero = 0;
	xQueueSend(xQueueApprentissageControl, &zero, portMAX_DELAY); // Arrêter la lecture de la distance
	xQueueSend(xQueueSawSpeed, &zero, portMAX_DELAY); // Arrêter la scie
	xStateMachine.next(xStateModeSel);
}

// Fonction qui permet de mettre à jour l'écran
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
		vSendLCDCommand((sChoice == "") ? "                    " : sChoice, 2, 0);
		vSendLCDCommand("Press * to confirm ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateActive))
	{
		vSendLCDCommand((uiMode == 1) ? "==   OPERATION   ==" : "==   APPRENDRE   ==", 0, 0);
		vSendLCDCommand("                   ", 1, 0);
		vSendLCDCommand("                   ", 2, 0);
		vSendLCDCommand("                   ", 3, 0);
	}
	else if (xStateMachine.isInState(xStateAdminMode))
	{
		vSendLCDCommand("==   ADMIN MODE   ==", 0, 0);
		vSendLCDCommand("1. Dump logs      ", 1, 0);
		vSendLCDCommand("2.                ", 2, 0);
		vSendLCDCommand("3.                ", 3, 0);
	}
}