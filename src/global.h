/// @file headers.h
/// @brief Les definitions globales incluant les entetes de fonction
///
/// Ce fichier contient les entetes de fonction et d'autre definitions globales
/// necessare au project

/// Inclusion
#include "../libs/debugger/debug.h"

/// Typedefs

/// Olivier
typedef struct MenuOption_t MenuOption_t;
struct MenuOption_t {
  char name[16];                           // nom de l'option
  char value[8];                           // valeur de l'option
  void *params;                            // donnees optionnelles
  int (*hookFunction)(MenuOption_t *self); // fonction executee lors d'un click
};

typedef struct {
  char *name[16];           // nom du menu
  MenuOption_t options[10]; // liste d'options
} Menu_t;

typedef Menu_t *MenuStack_t; // definition de la pile pour la navigation

typedef struct {
  char name[16]; // nom du bois
  int code;      // code du bois
  int sawSpeed;  // vitesse de la lame
  int feedRate;  // vitesse d'avancement
} Wood_t;

/// Yanick

/// Entetes de fonctions

/// Olivier

/// Affichage de la temperature du bois
float readTemperatureWood();

/// Programmation des indicateurs lumineux
enum lightColors { Red = 1, Green = 2, Blue = 4 };
void setLightIndicator(lightColors);

/// Programmation du mode manuel
int writeWood(Wood_t);

/// Programmation du mode operation (asservissement, anti-recul)
void setSawSpeed(int);
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

/// Yanick

/// Programmation du mode modification

/// Programmation du mode apprentissage

/// Afficher le niveau sonore en dB

/// Afficher la temperature et l'humidite ambiante
