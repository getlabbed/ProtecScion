
#include "global.h"

/// --------- CONFIGURATION --------- ///
// Définition des pins
#define PIN_MOTOR 25 // A1
#define PIN_SENSE 26 // A0

// Définition des constantes
// PID
#define PID_KP 0.8
#define PID_KI 0.3
#define PID_KD 0.01
#define PID_INITIAL_TARGET 2048 // mettre a 0 pour prod
#define PID_SAMPLE_TIME 100

// PWM
#define PWM_FREQUENCY 200
#define PWM_RESOLUTION 8

// ANTI-RECUL
#define ANTI_RECUL_THRESHOLD 70
#define ANTI_RECUL_ACTIVATION_THRESHOLD 100

/// --------- DEFINITIONS --------- ///
// Definition des types
typedef enum {OFF, INIT, STARTING, RUNNING} MotorState_t;

// Declaration des fonctions
void vTaskAsservissementScie(void *pvParameters);
bool bIsFastChange(double input, int threshold);