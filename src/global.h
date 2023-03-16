/// @file headers.h
/// @brief Les definitions globales incluant les entetes de fonction
///
/// Ce fichier contient les entetes de fonction et d'autre definitions globales
/// necessare au project

/// Inclusion
#include "../libs/debugger/debug.h"

/// Typedefs

/// ===================== Olivier =====================
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

typedef Menu_t *MenuStack_t; // définition de la pile pour la navigation

typedef struct {
  char name[16]; // nom du bois
  int code;      // code du bois
  int sawSpeed;  // vitesse de la lame
  int feedRate;  // vitesse d'avancement
} Wood_t;

/// ===================== Yanick =====================

/// Structure qui contient les données a mettre à jour sur l'écran LCD

typedef struct {
    int iDb;
    float fAmbiantHumidTemp;
} LCDParams_t;

/// ===================== Olivier =====================

/// Affichage de la température du bois
float readTemperatureWood();

/// Programmation des indicateurs lumineux
enum lightColors { Red = 1, Green = 2, Blue = 4 };
void setLightIndicator(lightColors);

/// Programmation du mode manuel
int writeWood(Wood_t);

/// Programmation du mode opération (asservissement, anti-recul)
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

/// ===================== Yanick =====================

/// Programmation du mode modification

void vModificationMode(Wood_t name);

/// Programmation du mode apprentissage

void vTaskLearningMode(); // * On doit ajouter le storage* en ce moment on a juste wood_t

/// Mettre à jour le niveau sonore en dB

void vTaskUpdateDb(void);

/// Mettre à jour la température et l'humidité ambiante

void vTaskUpdateAmbiantHumidTemp(void);

/// Mettre à jour l'écran LCD

void vTaskUpdateLCD(void *pvParameters)