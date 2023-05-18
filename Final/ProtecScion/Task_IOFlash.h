
#include "global.h"


/// --------- Wood IO --------- ///

const String woodType[] = {"Sapin", "Pin", "Noyer", "Bouleau", "Chêne", "Érable", "Frêne", "Merisier", "Mélèze"};

void vTaskIOFlash(void *pvParameters);
void writeEmptyFile();
void readWood(Wood_t &wood, int id);
void writeWood(int id, int sawSpeed, int feedRate);


/// --------- Log IO --------- ///

const String logLevelString[] = {"INFO", "WARNING", "ERROR"};

void logMessage(Log_t logMessage);
void dumpLog();
void vDumpWood(bool bPurge);
