#include <Arduino.h>
#include <SimpleDHT.h>
#include <Adafruit_LiquidCrystal.h>
#include <Adafruit_MLX90614.h>
#include <NewPing.h>
#include <PID_v1.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_task_wdt.h>

#define SOUND_SENSOR_PIN A2
#define VOLTAGE_REFERENCE 3.3

#define PID_INPUT_PIN A1

const int in1 = 14;
const int in2 = 32;

unsigned int uiTemp;
unsigned int uiHumid;
double dWoodTemp;
float fSoundSensorVoltage;
float fSoundSensorValue;

double dWantedSawSpeed = 50, dSawInput, dSawOutput = 50, dfeedRate;
double Kp = 2, Ki = 5, Kd = 1;

PID myPID(&dSawInput, &dSawOutput, &dWantedSawSpeed, Kp, Ki, Kd, 1, DIRECT);

const int freq = 200;
const unsigned long period = 1000000L / freq;
unsigned long highTime = period * dSawOutput / 100;
Adafruit_LiquidCrystal lcd(27, 33, 15);
SimpleDHT11 dht(A0);

void vMotorControlTask(void *pvParameters);
void vSensorDataTask(void *pvParameters);

void setup()
{
  Serial.begin(115200);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  lcd.begin(20, 4);
  lcd.clear();

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 100);
  myPID.SetSampleTime(50);

  dWantedSawSpeed = 50;

  xTaskCreate(vMotorControlTask, "Motor Control", 1000, NULL, 2, NULL);
  xTaskCreate(vSensorDataTask, "Sensor Data", 1000, NULL, 1, NULL);

  esp_task_wdt_delete(NULL);
  esp_task_wdt_deinit();

  vTaskStartScheduler();
}

void loop()
{
  // Do nothing, the tasks run independently
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  digitalWrite(in1, HIGH);
  delayMicroseconds(highTime);
  digitalWrite(in1, LOW);
  delayMicroseconds(period - highTime);

  float data = analogRead(PID_INPUT_PIN);
  // myPID.Compute();
  fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / (1024.0);
  fSoundSensorValue = fSoundSensorVoltage * 50.0;
  Serial.println(" Sound: " + String(fSoundSensorValue) + "dB");
  Serial.println("Feedback: " + String(data));
}

void vMotorControlTask(void *pvParameters)
{
  for (;;)
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    digitalWrite(in1, HIGH);
    delayMicroseconds(highTime);
    digitalWrite(in1, LOW);
    delayMicroseconds(period - highTime);

    float data = analogRead(PID_INPUT_PIN);
    myPID.Compute();

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void vSensorDataTask(void *pvParameters)
{
  for (;;)
  {
    uiTemp = dht.readTemperature();
    uiHumid = dht.readHumidity();
    Serial.print(" Temp: ");
    Serial.print(uiTemp);
    Serial.print(" Humid: ");
    Serial.print(uiHumid);

    fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / (1024.0);
    fSoundSensorValue = fSoundSensorVoltage * 50.0;
    lcd.clear();
    lcd.print(" Sound: " + String(fSoundSensorValue) + "dB");

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}