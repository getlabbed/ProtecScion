
#include <PID_v2.h>

#define PIN_MOTOR 26
#define PIN_SENSE 25

double Kp = 2, Ki = 5, Kd = 1;
PID_v2 myPID(Kp, Ki, Kd, PID::Direct);

bool isFastChange(double input, int threshold);

static double lastInput = 0;

void setup()
{
  Serial.begin(115200);
  analogWriteResolution(8);
  analogWriteFrequency(200);
	myPID.Start(analogRead(PIN_SENSE), 0, 2048);
}

void loop()
{
	const double input = analogRead(PIN_SENSE);
  const double output = myPID.Run(input);

  //Serial.println(input);

  if (isFastChange(input, 100))
  {
    Serial.println("Anti-recul triggered");
  }

  analogWrite(PIN_MOTOR, output);
}

/*
  * @function isFastChange
  * @param input: the input value
  * @param threshold: the threshold to trigger the anti-recul
  * @return bool: true if the input is changing faster than the threshold
  * 
  * @brief This function is used to detect if the input is changing faster than the threshold
  * 
  * @update 2021-03-10 by Olivier David Laplante 
  */
bool isFastChange(double input, int threshold)
{
  const double diff = abs(input - lastInput);
  lastInput = input;
  return diff > threshold;
}
