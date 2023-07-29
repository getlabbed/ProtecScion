/**
 * @file Task_AsservissementScie.h
 * @author Skkeye
 * @brief Header file of the Saw Servo Task 
 * @note restrictions: ESP32 Feather board type
 * @version 1.0
 * @date 2023-04-30 - Initial code entry
 * @date 2023-05-18 - Final code entry
 * 
 */

#include "global.h"

/// --------- CONFIGURATION --------- ///
// Pin definitions
#define PIN_MOTOR 25 // A1
#define PIN_SENSE 26 // A0

// Constant definitions
// PID
#define PID_KP 0.8
#define PID_KI 0.3
#define PID_KD 0.01
#define PID_INITIAL_TARGET 0 // Set to 0 for prod
#define PID_SAMPLE_TIME 100

// PWM
#define PWM_FREQUENCY 200
#define PWM_RESOLUTION 8

// KickBack detection
#define ANTI_RECUL_THRESHOLD 150
#define ANTI_RECUL_ACTIVATION_THRESHOLD 100

/// --------- DEFINITIONS --------- ///
// Type definitions
typedef enum {OFF, INIT, STARTING, RUNNING} MotorState_t;

// Function prototypes
void vTaskAsservissementScie(void *pvParameters);
bool bIsFastChange(double input, int iThreshold);