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

unsigned int uiMode = 0; // Mode sélectionné | 0: Opération, 1: Apprendre, 2: Manuel, 3: Modifier
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

// sélection du mode
void xStateModeSel()
{
	Serial.println("xStateModeSel");
	if (cChoice >= '1' && cChoice <= '4') // Si le mode choisit est valide
	{
		uiMode = cChoice - '0';             // Convertir le choix en entier
		// ALWAYS GO TO WOOD SELECT
		xStateMachine.next(xStateWoodSel);  // Aller à la sélection du bois
		sChoice = "";                       // Réinitialiser le choix
	}
}

// sélection du bois
void xStateWoodSel()
{
	Serial.println("xStateWoodSel");
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
	else if (cChoice >= '0' && cChoice <= '9' && sChoice.length() < 3)
	{
		sChoice.concat(cChoice);
	}
}

// Édition de la vitesse de la scie
void xStateEditSawSpeed()
{
	Serial.println("xStateEditSawSpeed");
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
		sChoice = "";
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
	Serial.println("xStateEditFeedRate");
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
	Serial.println("xStateActive");
	unsigned int zero = 0;
	xQueueSend(xQueueSawSpeed, &(zero), portMAX_DELAY); // Arrêter la scie
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
}