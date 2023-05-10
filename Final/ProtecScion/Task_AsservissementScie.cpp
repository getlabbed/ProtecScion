
#include "Task_AsservissementScie.h"

#include <PID_v2.h>

// global variables
double average[10] = {0,0,0,0,0,0,0,0,0,0};
double lastInputs[4] = {0,0,0,0};
double lastOutput = 0;

void vTaskAsservissementScie(void *pvParameters) {
  // local variables
  MotorState_t motorState = OFF; // pas pour prod
  unsigned int target = PID_INITIAL_TARGET; // etre capable de setter pour prod
  // PID instance
  PID_v2 myPID(PID_KP, PID_KI, PID_KD, PID::Direct);

  // PWM setup
  analogWriteResolution(PWM_RESOLUTION);
  analogWriteFrequency(PWM_FREQUENCY);

  // PID setup
  myPID.Start(analogRead(PIN_SENSE), 0, target);
  myPID.SetSampleTime(PID_SAMPLE_TIME);

  while (true) {
    if (xQueueReceive(xQueueSawSpeed, &target, 0) == pdTRUE)
    {
      if (target == 0 || target > 4096)
      {
        motorState = OFF;
        vSendLog(INFO, "ASSERV: Received new target, motor set to OFF");
      }
      else
      {
        motorState = INIT;
        vSendLog(INFO, "ASSERV: Received new target, motor set to INIT");
      }
    }

    if (motorState == OFF)
    {
      myPID.Setpoint(0);
      myPID.Run(0);
      analogWrite(PIN_MOTOR, 0);
    }
    else if (motorState == INIT)
    {
      myPID.Setpoint(target);
      motorState = STARTING;
      vSendLog(INFO, "ASSERV: Motor set to STARTING");
    }
    else if (motorState == STARTING)
    {
      const double input = analogRead(PIN_SENSE);
      const double output = myPID.Run(input);
      analogWrite(PIN_MOTOR, output);
      bIsFastChange(input, ANTI_RECUL_THRESHOLD);
      if (abs(input - target) < ANTI_RECUL_ACTIVATION_THRESHOLD)
      {
        motorState = RUNNING;
        vSendLog(INFO, "ASSERV: Motor has reached target, set to RUNNING");
      }
    }
    else if (motorState == RUNNING)
    {
      const double input = analogRead(PIN_SENSE);
      if (bIsFastChange(input, ANTI_RECUL_THRESHOLD))
      {
        // stop the motor
        motorState = OFF;
        vSendLog(INFO, "ASSERV: Fast change detected, motor set to OFF");
      }
      else
      {
        // update the PID
        const double output = myPID.Run(input);
        analogWrite(PIN_MOTOR, output);
      }
    }
    vTaskDelay(PID_SAMPLE_TIME / portTICK_PERIOD_MS);
  }
}

bool bIsFastChange(double input, int threshold)
{
  // only check every 100ms
  if (millis() - lastOutput < 20)
  {
    return false;
  }
  
  lastOutput = millis();

  bool result = false;
  for (int i = 0; i < 9; i++)
  {
    average[i] = average[i+1];
  }
  average[9] = input;

  double sum = 0;
  for (int i = 0; i < 10; i++)
  {
    sum += average[i];
  }
  sum /= 10;

  for (int i = 0; i < 3; i++)
  {
    lastInputs[i] = lastInputs[i+1];
  }
  lastInputs[3] = sum;

  double sumLast = 0;
  for (int i = 0; i < 4; i++)
  {
    sumLast += lastInputs[i];
  }
  sumLast /= 4;

  return abs(sumLast - sum) > threshold;
}