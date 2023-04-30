/**
 * Nom du fichier :
 *  @name global.h
 * Description :
 *  @brief Fichier d'entête pour le code ProtecScion.ino
 * restrictions:
 *  Pour type de carte ESP32 Feather
 *  Pour utilisation avec ProtecScion.ino
 * Historique :
 *  @date 2021-04-13 @author Olivier David Laplante @author Yanick Labelle - Entrée initiale du code.
 *  @note aucune(s)
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

#include "SoundSensor.ino"
#include "SPIFFS.ino"
#include "LCDscreen.ino"
#include "ModeOperation.ino"
#include "ModeManuel.ino"
#include "ModeModification.ino"

#define TASK_STACK_SIZE 4096

/// Typedef

/// ===================== Olivier =====================

// -- Pas utilisé pour le moment --
// typedef struct MenuOption_t MenuOption_t;
// struct MenuOption_t {
//   char name[16];                           // nom de l'option
//   char value[8];                           // valeur de l'option
//   void *params;                            // données optionnelles
//   int (*hookFunction)(MenuOption_t *self); // fonction exécutée lors d'un click
// };

// typedef struct {
//   char *name[16];           // nom du menu
//   MenuOption_t options[10]; // liste d'options
// } Menu_t;

// typedef Menu_t *MenuStack_t; // définition de la pile pour la navigation

typedef struct {
  char name[16]; // nom du bois
  int code;      // code du bois
  int sawSpeed;  // vitesse de la lame
  int feedRate;  // vitesse d'avancement
} Wood_t;

/// ===================== Olivier =====================

/// Affichage de la température du bois
float readTemperatureWood();

/// Programmation des indicateurs lumineux
typedef enum LightColors_t { Red = 1, Green = 2, Blue = 3 };

void vTasksetLightIndicator(void *pvParameters);

/// Programmation du mode manuel
int writeWood(Wood_t);

/// Programmation du mode opération (asservissement, anti-recul)
void setSawSpeed(int speed);
int readSawSpeed();
int readWoodSpeed();
Wood_t readWood(int);

/// Journalisation
void serialSemaphoreTakeHook();
void serialSemaphoreGiveHook();
void logfileSemaphoreTakeHook();
void logfileSemaphoreGiveHook();
void logfilePrintHook(char *);
void serialPrintHook(char *);
void panicKillHook();

/// ===================== Yanick =====================

/// Mettre à jour l'écran LCD
void vTaskUpdateLCD(void *pvParameters);

/// Mettre à jour la température et l'humidité ambiante
void vTaskUpdateAmbiantHumidTemp(void *pvParameters);

/// Mettre à jour le niveau sonore en dB
void vTaskUpdateDb(void *pvParameters);

/// Programmation du mode apprentissage
void vTaskLearningMode(int id);

/// Programmation du mode modification
void vModificationMode(void *pvParameters);

// Écriture d'un fichier JSON
String readFileData(const char *filename);

// LEcture d'un fichier JSON
void writeFileData(const char *filename, const char *data);

// Affichage LCD
void vLcdSetLine(const char *message, int iLine);

// Modifier les paramètres existants d'un bois
void updateWood(int id, int sawSpeed, int feedRate);

// Création d'un nouveau bois
void writeWood(int id, int sawSpeed, int feedRate);

// Lecture des paramètres d'un bois
Wood_t readWood(int id);

#endif