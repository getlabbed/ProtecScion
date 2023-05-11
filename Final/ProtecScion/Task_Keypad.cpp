#include "Task_Keypad.h"
#include <Keypad.h>

void vTaskKeypad(void *pvParameters){
    const byte ROWS = 4;
    const byte COLS = 3;
    char keys[ROWS][COLS] = {
      {'1','2','3'},
      {'4','5','6'},
      {'7','8','9'},
      {'#','0','*'}
    };
    byte rowPins[ROWS] = {36, 4, 5, 18};
    byte colPins[COLS] = {19, 16, 17};
    Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
    while(1){
        // Read keypad
        char key = keypad.getKey();

        if (key != NO_KEY){
            xQueueSend(xQueueKeypad, &key, portMAX_DELAY);
        }
        vTaskDelay(100);
    }
}
