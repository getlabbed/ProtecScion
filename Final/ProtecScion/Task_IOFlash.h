/**
 * @file Task_IOFlash.h
 * @author Skkeye
 * @author Skkeye's coleague
 * @brief Header file of Task_IOFlash.cpp
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-04-30 - Initial code entry
 * @date 2023-05-18 - Final code entry
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
