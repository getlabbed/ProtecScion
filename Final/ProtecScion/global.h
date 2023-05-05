

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>

/// --------- CONFIGURATION --------- ///
// Tâches
#define TASK_STACK_SIZE 4096
// AsservissementScie
#define TASK_ASSERVISSEMENTSCIE_PRIORITY 1
#define TASK_ASSERVISSEMENTSCIE_CORE 0
// IOFlash
#define TASK_IOFLASH_PRIORITY 1
#define TASK_IOFLASH_CORE 1