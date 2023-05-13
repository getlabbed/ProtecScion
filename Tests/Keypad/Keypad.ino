/**
 * Nom du fichier :
 *  @name Keypad.ino
 * Description :
 *  @brief Code Test pour le clavier numérique
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-05-13 @author Yanick Labelle - Entrée initiale du code.
 *  @note aucune(s)
 */

#include <Keypad.h>

#define KEYPAD1 21
#define KEYPAD2 17
#define KEYPAD3 16
#define KEYPAD4 19
#define KEYPAD5 18
#define KEYPAD6 5
#define KEYPAD7 4

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '#', '0', '*' }
};
byte rowPins[ROWS] = { KEYPAD1, KEYPAD2, KEYPAD3, KEYPAD4 };
byte colPins[COLS] = { KEYPAD5, KEYPAD6, KEYPAD7 };
Keypad kp = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char key = NO_KEY;

void setup() {
  Serial.begin(115200);
}
void loop() {
  key = kp.waitForKey();
  delay(1); // Très important
  Serial.println(key);
}