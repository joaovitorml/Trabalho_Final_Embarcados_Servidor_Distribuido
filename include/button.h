#ifndef BUTTON_H
#define BUTTON_H

void ButtonConfiguration();

static void IRAM_ATTR gpio_isr_handler(void *args);

void ButtonInterruption(void *params);

#endif