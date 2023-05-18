/**
 * @file Task_AsservissementScie.cpp
 * @author Olivier David Laplante (skkeye@gmail.com)
 * @brief Fichier d'implémentation de la tâche d'asservissement de la scie (PID)
 *        et de la detection des KickBacks
 * @note restrictions: Pour type de carte ESP32 Feather
 * @version 1.0
 * @date 2023-04-30 - Entrée initiale du code
 * @date 2023-05-18 - Entrée finale du code
 * 
 */

#include "Task_AsservissementScie.h"

#include <PID_v2.h>

// global variables
double dAverage[10] = {0,0,0,0,0,0,0,0,0,0};
double dLastInputs[4] = {0,0,0,0};
double dLastOutput = 0;

/**
 * @brief Tache d'asservissement de la scie (PID) et de la detection des KickBacks
 * @author Olivier David Laplante
 * 
 * @param pvParameters - Non utilisé
 */
void vTaskAsservissementScie(void *pvParameters) {
  // local variables
  MotorState_t xMotorState = OFF; // pas pour prod
  unsigned int uiTarget = PID_INITIAL_TARGET; // etre capable de setter pour prod
  // PID instance
  PID_v2 xPID(PID_KP, PID_KI, PID_KD, PID::Direct);

  // PWM setup
  analogWriteResolution(PWM_RESOLUTION);
  analogWriteFrequency(PWM_FREQUENCY);

  // PID setup
  xPID.Start(analogRead(PIN_SENSE), 0, uiTarget);
  xPID.SetSampleTime(PID_SAMPLE_TIME);

  while (true) {
    if (xQueueReceive(xQueueSawSpeed, &uiTarget, 0) == pdTRUE)
    {
      if (uiTarget == 0 || uiTarget > 4096)
      {
        xMotorState = OFF;
        vSendLog(INFO, "ASSERV: Received new uiTarget, motor set to OFF");
      }
      else
      {
        xMotorState = INIT;
        vSendLog(INFO, "ASSERV: Received new uiTarget, motor set to INIT");
      }
    }

    if (xMotorState == OFF)
    {
      xPID.Setpoint(0);
      xPID.Run(0);
      analogWrite(PIN_MOTOR, 0);
    }
    else if (xMotorState == INIT)
    {
      xPID.Setpoint(uiTarget);
      xMotorState = STARTING;
      vSendLog(INFO, "ASSERV: Motor set to STARTING");
    }
    else if (xMotorState == STARTING)
    {
      const double cdInput = analogRead(PIN_SENSE);
      const double cdOutput = xPID.Run(cdInput);
      analogWrite(PIN_MOTOR, cdOutput);
      bIsFastChange(cdInput, ANTI_RECUL_THRESHOLD);
      if (abs(cdInput - uiTarget) < ANTI_RECUL_ACTIVATION_THRESHOLD)
      {
        xMotorState = RUNNING;
        vSendLog(INFO, "ASSERV: Motor has reached uiTarget, set to RUNNING");
      }
    }
    else if (xMotorState == RUNNING)
    {
      const double cdInput = analogRead(PIN_SENSE);
      if (bIsFastChange(cdInput, ANTI_RECUL_THRESHOLD))
      {
        // stop the motor
        xMotorState = OFF;
        unsigned int uiZero = 0;
        xQueueSend(xQueueKeypad, &uiZero, 0);
        vSendLog(INFO, "ASSERV: Fast change detected, motor set to OFF");
      }
      else
      {
        // update the PID
        const double cdOutput = xPID.Run(cdInput);
        analogWrite(PIN_MOTOR, cdOutput);
      }
    }
    vTaskDelay(PID_SAMPLE_TIME / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Fonction qui detecte les changements rapides de la vitesse de la scie
 * @author Olivier David Laplante
 * @note Cette fonction est utilisee pour detecter les KickBacks
 * 
 * @param cdInput - Valeur de la vitesse de la scie
 * @param iThreshold - Seuil de detection
 * @return true - Si un changement rapide est detecte
 * @return false - Si aucun changement rapide n'est detecte
 */
bool bIsFastChange(double cdInput, int iThreshold)
{
  for (int i = 0; i < 9; i++)
  {
    dAverage[i] = dAverage[i+1];
  }
  dAverage[9] = cdInput;

  double dSum = 0;
  for (int i = 0; i < 10; i++)
  {
    dSum += dAverage[i];
  }
  dSum /= 10;

  for (int i = 0; i < 3; i++)
  {
    dLastInputs[i] = dLastInputs[i+1];
  }
  dLastInputs[3] = dSum;

  double dSumLast = 0;
  for (int i = 0; i < 4; i++)
  {
    dSumLast += dLastInputs[i];
  }
  dSumLast /= 4;

  // Serial.print("dSum: ");
  // Serial.print(dSum);
  // Serial.print(" dSumLast: ");
  // Serial.println(dSumLast);

  return abs(dSumLast - dSum) > iThreshold;
}