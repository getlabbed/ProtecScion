#include <PID_v2.h>
#include "global.h"


#define PIN_MOTOR 25 // A1
#define PIN_SENSE 26 // A0

bool isFastChange(double input, int threshold);

double Kp = 0.8, Ki = 0.3, Kd = 0.01;
PID_v2 *myPID;

const double target = 2048; // a changer pour la valeur demandée

static double average[10] = {0,0,0,0,0,0,0,0,0,0};
static double lastInputs[4] = {0,0,0,0};
static double lastOutput = 0;
static bool isAntiRecul = false;


void vModeOperation() {
  analogWriteResolution(8);
  analogWriteFrequency(200);
  myPID = new PID_v2(Kp, Ki, Kd, PID::Direct);
  myPID->Start(analogRead(PIN_SENSE), 0, target);

  while (true)
  {
    const double input = analogRead(PIN_SENSE);
    const double output = myPID->Run(input);

    // Attedre que le moteur soit en position avant d'enclencher les mesures de protection
    isAntiRecul = (input > target) ? true : isAntiRecul;


    if (isFastChange(input, 70))
    {
      Serial.println("Anti-recul triggered : " + String(lastInputs[3]) + " : " + String(lastInputs[2]) + " : " + String(lastInputs[1]) + " : " + String(lastInputs[0]) );
      if (isAntiRecul)
      {
        
        // stop the motor
        analogWrite(PIN_MOTOR, 0);
        while (true)
        {
          // attendre un reset
        }
        isAntiRecul = false;
      }
    }

    analogWrite(PIN_MOTOR, output);
  }
}

bool isFastChange(double input, int threshold)
{
  // only check every 100ms
  if (millis() - lastOutput < 20)
  {
    return false;
  }

  // average the last 10 values to debounce
  for (int i = 0; i < 9; i++)
  {
    average[i] = average[i + 1];
  }
  average[9] = input;

  double sum = 0;
  for (int i = 0; i < 10; i++)
  {
    sum += average[i];
  }
  double avg = sum / 10;

  // average the last 4 values to detect fast change
  for (int i = 0; i < 3; i++)
  {
    lastInputs[i] = lastInputs[i + 1];
  }
  lastInputs[3] = avg;

  double sum2 = 0;
  for (int i = 0; i < 4; i++)
  {
    sum2 += lastInputs[i];
  }
  double avg2 = sum2 / 4;

  if (abs(avg2 - avg) > threshold)
  {
    lastOutput = millis();
    return true;
  }
  else
  {
    return false;
  }
}
