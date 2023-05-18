/**
 * @file Task_IOFlash.h
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @author Yanick Labelle (getlabbed@proton.me)
 * @brief Fichier d'en-tête du fichier Task_IOFlash.cpp
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-04-30 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code
 * 
 */

#include "global.h"


/// --------- Wood IO --------- ///

const String woodType[] = {"Sapin", "Pin", "Noyer", "Bouleau", "Chêne", "Érable", "Frêne", "Merisier", "Mélèze"};

void vTaskIOFlash(void *pvParameters);
void writeEmptyFile();
void readWood(Wood_t &xWood, int iCode);
void writeWood(int iCode, int sawSpeed, int feedRate);


/// --------- Log IO --------- ///

const String logLevelString[] = {"INFO", "WARNING", "ERROR"};

void logMessage(Log_t logMessage);
void dumpLog();
void vDumpWood(bool bPurge);
