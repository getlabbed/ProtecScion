/**
 * Nom du fichier :
 *  @name ProtecScion.ino
 * Description :
 *  @brief Programme permettant d'améliorer la fonctionnalité d'un banc de scie.
 * restrictions:
 *  Pour type de carte ESP32 Feather
 * Historique :
 *  @date 2023-04-30 @author Olivier David Laplante @author Yanick Labelle - Entrée initiale du code.
 *  @note Utilisation de Doxygen pour la documentation:
 *         https://www.doxygen.nl/manual/docblocks.html
 */

/// --------- INCLUDES --------- ///
#include "global.h"

/// --------- TASKS --------- ///
// include tasks
#include "Task_AsservissementScie.h"
#include "Task_IOFlash.h"

// define handles
TaskHandle_t xTaskAsservissementScie;
TaskHandle_t xTaskIOFlash;

/** 
 * @fn vCreateAllTasks
 * @brief Create all tasks
 * 
 * @author Olivier David Laplante @date 30-04-2023
 */
void vCreateAllTasks() {
  // create tasks
  xTaskCreatePinnedToCore(vTaskAsservissementScie, "AsservissementScie", TASK_STACK_SIZE, NULL, TASK_ASSERVISSEMENTSCIE_PRIORITY, &xTaskAsservissementScie, TASK_ASSERVISSEMENTSCIE_CORE);
  //xTaskCreatePinnedToCore(vTaskIOFlash, "IOFlash", TASK_STACK_SIZE, NULL, TASK_IOFLASH_PRIORITY, &xTaskIOFlash, TASK_IOFLASH_CORE);
}

/// --------- SETUP & LOOP --------- ///
/**
 * Nom de la fonction :
 *  @name setup
 * Description de la fonction :
 *  @brief Fonction d'initialisation du programme.
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note Aucune
 *
 * Historique :
 *  @date 2023-04-30 @author Olivier David Laplante - Entrée initiale du code.
 */
void setup() {
  // setup tasks
  vCreateAllTasks();

  // disable watchdog
  esp_task_wdt_delete(NULL);
  esp_task_wdt_deinit();
}

/**
 * Nom de la fonction :
 *  @name loop
 * Description de la fonction :
 *  @brief Fonction de boucle principale du programme.
 *
 * Valeur de retour :
 *  @return void
 * Note(s) :
 *  @note Aucune
 *
 * Historique :
 *  @date 2023-04-30 Olivier David Laplante - Entrée initiale du code.
 */
void loop() {
}