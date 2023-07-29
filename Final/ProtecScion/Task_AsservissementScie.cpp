/**
 * @file Task_AsservissementScie.cpp
 * @author Skkeye
 * @brief Implementations file of the task of the saw (PID) servo loop 
 *        and KickBacks detection
 * @note restriction: ESP32 Feather board type
 * @version 1.0
 * @date 2023-04-30 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

#include "Task_AsservissementScie.h"

#include <PID_v2.h>

// global variables
double dAverage[10] = {0,0,0,0,0,0,0,0,0,0};
double dLastInputs[4] = {0,0,0,0};
double dLastOutput = 0;

/**
 * @brief Saw Servo Task (PID) and KickBacks detection
 * @author Skkeye
 * 
 * @param pvParameters - Not used
 */
void vTaskAsservissementScie(void *pvParameters) {
  // local variables
  MotorState_t xMotorState = OFF; // not for prod
  unsigned int uiTarget = PID_INITIAL_TARGET; // Need to be able to set for prod
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
      // Send the BOTH state to LED
      const LedState_t cxBoth = LED_BOTH;
      xQueueSend(xQueueLED, &cxBoth, 0);
      myPID.Setpoint(target);
      motorState = STARTING;
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
        // send the GREEN state to LED
        const LedState_t cxGreen = LED_GREEN;
        xQueueSend(xQueueLED, &cxGreen, 0);
        motorState = RUNNING;
        vSendLog(INFO, "ASSERV: Motor has reached target, set to RUNNING");
      }
    }
    else if (xMotorState == RUNNING)
    {
      const double cdInput = analogRead(PIN_SENSE);
      if (bIsFastChange(cdInput, ANTI_RECUL_THRESHOLD))
      {
        // send the RED state to LED
        const LedState_t cxRed = LED_RED;
        xQueueSend(xQueueLED, &cxRed, 0);
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
 * @brief Detection function of sudden changes in the speed of the saw
 * @author Skkeye
 * @note This function is used to detect KickBacks
 * 
 * @param cdInput - Saw speed value
 * @param iThreshold - Detecting threshold
 * @return true - If a fast change is detected
 * @return false - If no fast change is detected
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