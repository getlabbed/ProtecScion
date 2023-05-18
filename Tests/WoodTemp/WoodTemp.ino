/**
 * @file WoodTemp.ino
 * @author Olivier David Laplante
 * @brief Code test pour la lecture de la température du bois
 * @version 0.1
 * @date 2023-05-17
 * 
 */

#include <Adafruit_MLX90614.h>

#include <Arduino.h>
#include <Wire.h>

TwoWire WireBus = TwoWire(0);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup()
{
	Serial.begin(115200);
  WireBus.begin(23, 22);
  mlx.begin(0x5A, &WireBus);
}

void loop()
{
	Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF());
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

  Serial.println();
  delay(500);
}
 