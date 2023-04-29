
#include <PID_v2.h>

#define PIN_MOTOR 25 // A1
#define PIN_SENSE 26 // A0

double Kp = 0.8, Ki = 0.3, Kd = 0.01;
PID_v2 myPID(Kp, Ki, Kd, PID::Direct);

const double target = 2048;

bool isFastChange(double input, int threshold);

static double average[10] = {0,0,0,0,0,0,0,0,0,0};
static double lastInputs[4] = {0,0,0,0};
static double lastOutput = 0;
static bool isAntiRecul = false;

void setup()
{
  Serial.begin(115200);
  analogWriteResolution(8);
  analogWriteFrequency(200);
	myPID.Start(analogRead(PIN_SENSE), 0, target);
}

void loop()
{
	const double input = analogRead(PIN_SENSE);
  const double output = myPID.Run(input);

  //Serial.println("                     " + String(input));

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

      }
      isAntiRecul = false;
    }
  }

  analogWrite(PIN_MOTOR, output);
}

// write a function to detect fast change 
// average the last 10 values to debounce
// average the last 4 values to detect fast change
bool isFastChange(double input, int threshold)
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

  if (abs(sumLast - sum) > threshold)
  {
    result = true;
  }

  return result;
}