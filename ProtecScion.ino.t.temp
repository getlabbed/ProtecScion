#include <Arduino.h>
#include <DHT_U.h>
#include <DHT.h>
#include <Adafruit_LiquidCrystal.h>
#include <Adafruit_MLX90614.h>
#include <NewPing.h>
#include <PID_v1.h>

#define SOUND_SENSOR_PIN A2
#define VOLTAGE_REFERENCE 3.3

#define PID_INPUT_PIN A1

// Define the pins connected to the H-bridge inputs
const int in1 = 14; // IN1 connected to digital pin 2
const int in2 = 32; // IN2 connected to digital pin 3

unsigned int uiTemp;
unsigned int uiHumid;
double dWoodTemp;
float fSoundSensorVoltage;
float fSoundSensorValue;

// Définition des variables pour le PID
double dWantedSawSpeed, dSawInput, dSawOutput, dfeedRate;
double Kp = 2, Ki = 5, Kd = 1;

//initialisation du PID
PID myPID(&dSawInput, &dSawOutput, &dWantedSawSpeed, Kp, Ki, Kd, 1, DIRECT);


// PWM settings
const int freq = 200; // Desired frequency in Hz
const unsigned long period = 1000000L / freq; // Period in microseconds
const unsigned long highTime = period * dSawOutput / 100; // Time for HIGH state in microseconds
Adafruit_LiquidCrystal lcd(27, 33, 15);
DHT dht(A0, DHT11); // Création d'un objet DHT (capteur de température et humidité)

void setup() {
  Serial.begin(115200);
  // Set the motor control pins as outputs
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
	lcd.begin(20, 4);
	lcd.clear();
  lcd.print("AHAHAH");

  // Configuration du PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 100);
  myPID.SetSampleTime(50);

  dWantedSawSpeed = 50; // Vitesse de la scie désirée

  // Afficher les valeurs sur l'écran LCD
  //vUpdateInfo(fDb, fAmbiantHumid, fAmbiantTemp, (float)dWoodTemp);

}

void loop() {

  // Move motor forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  // Generate the PWM signal manually
  digitalWrite(in1, HIGH); // Set the pin to HIGH
  delayMicroseconds(highTime); // Wait for the high time
  digitalWrite(in1, LOW); // Set the pin to LOW
  delayMicroseconds(period - highTime); // Wait for the remaining time in the period

  dSawInput = analogRead(PID_INPUT_PIN); // Lire la valeur de la vitesse de la scie
  // Serial.println(dSawInput);
  myPID.Compute(); // Exécuter le PID

}

void vPrintData(){

  //Récupérer l'humidité et la température ambiante
  uiTemp = dht.readTemperature();
  uiHumid = dht.readHumidity();
  Serial.print(" Temp: ");
  Serial.print(uiTemp);
  Serial.print(" Humid: ");
  Serial.print(uiHumid);

  //Récupérer le niveau sonore en dB

  fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / 1024.0 * VOLTAGE_REFERENCE;
  fSoundSensorValue = fSoundSensorVoltage * 50.0; // conversion de la tension en décibels
  Serial.println(" Sound: " + String(fSoundSensorValue) + "dB");
}