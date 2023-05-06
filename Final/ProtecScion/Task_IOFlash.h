
#include "global.h"

const String woodType[] = {"Sapin", "Pin", "Noyer", "Bouleau", "Chêne", "Érable", "Frêne", "Merisier", "Mélèze", "Épinette"};

typedef struct
{
	String name; // nom du bois
	int code;			 // code du bois
	int sawSpeed;	 // vitesse de la lame
	int feedRate;	 // vitesse d'avancement
} Wood_t;

void vTaskIOFlash(void *pvParameters);
void writeEmptyFile();
void readWood(Wood_t &wood, int id);
void writeWood(int id, int sawSpeed, int feedRate);
