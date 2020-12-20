#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "freertos/semphr.h"
#include "string.h"
#include "led.h"

#define LED 2
void init_led()
{
	gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void set_led_state(int state)
{
	gpio_set_level(LED, state);
}