/// @file headers.h
/// @brief Les definitions globales incluant les entetes de fonction
///
/// Ce fichier contient les entetes de fonction et d'autre definitions globales
/// necessare au project

/// Typedefs

/// Olivier
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
Wood_t readWood(int);

/// Journalisation

/// Yanick

/// Programmation du mode modification

/// Programmation du mode apprentissage

/// Afficher le niveau sonore en dB

/// Afficher la temperature et l'humidite ambiante
