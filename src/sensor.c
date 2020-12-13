#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "dht11.h"

#define SENSOR 4

static struct dht11_reading data;
extern xSemaphoreHandle semaphSensor;

void ReadData(){

    DHT11_init(SENSOR);

    while(1){
        xSemaphoreTake(semaphSensor, portMAX_DELAY);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        data = DHT11_read();

        if(data.temperature > -1){
            printf("Temp: %d\n",data.temperature);
            printf("Umi: %d\n", data.humidity);
            printf("Estado: %d\n", data.status);
        }
        xSemaphoreGive(semaphSensor);
    }
}