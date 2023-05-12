
#include "global.h"

typedef enum {MODE_SEL, WOOD_SEL, EDIT_SAW_SPEED, EDIT_FEED_RATE, ACTIVE} MenuState_t;

void vTaskMenu(void *pvParameters);