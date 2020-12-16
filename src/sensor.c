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
extern xSemaphoreHandle conexaoMQTTSemaphore;
void ReadData(){

    DHT11_init(SENSOR);
	xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY);
    while(1){
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        data = DHT11_read();

        if(data.temperature > -1){
            printf("Temp: %d\n",data.temperature);
            printf("Umi: %d\n", data.humidity);
            printf("Estado: %d\n", data.status);
        }else{
			ESP_LOGE("SENSOR","ERRO DE LEITURA");
		}
        
    }
}