/**
 * Nom du fichier :
 *  @name SoundSensor.ino
 * Description :
 *  @brief Code Test pour le de niveau sonore
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-04-29 @author Yanick Labelle- Entrée initiale du code.
 *  @note aucune(s)
 */

#include <Arduino.h>

#define SOUND_SENSOR_PIN 34
#define RESOLUTION 1024
#define VOLTAGE_REFERENCE 3.3

float fSoundSensorVoltage;
float fSoundSensorValue;

void setup() {
  Serial.begin(115200);
}

void loop() {
  fSoundSensorVoltage = analogRead(SOUND_SENSOR_PIN) / RESOLUTION * VOLTAGE_REFERENCE;   // Récupérer le niveau sonore en dB
  fSoundSensorValue = fSoundSensorVoltage * 50.0;                                        // conversion de la tension en décibels
  Serial.println(" Niveau sonore: " + String(fSoundSensorValue) + "dB");
}