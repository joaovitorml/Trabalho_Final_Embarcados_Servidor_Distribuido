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
#include "cJSON.h"
#include "mqtt.h"

#include "dht11.h"

#define MATRICULA CONFIG_MATRICULA 
#define SENSOR 4

static struct dht11_reading data;
extern xSemaphoreHandle semaphSensor;
extern xSemaphoreHandle conexaoMQTTSemaphore;
extern char room[100];
char humid_topic[200], temp_topic[200];

void ReadData(){
    DHT11_init(SENSOR);
	xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY);
	ESP_LOGI("SENSOR","\n\n\n antes do sprintf \n\n\n");

	sprintf(humid_topic,"fse2020/%s/%s/umidade",MATRICULA,room);
	sprintf(temp_topic,"fse2020/%s/%s/temperatura",MATRICULA,room);
	ESP_LOGI("SENSOR","\n\n\n depois do sprintf \n\n\n");
	cJSON *temp_json = NULL,
		*humid_json = NULL,
		*temp_value= NULL,
		*humid_value = NULL,
		*json_room = NULL;
	ESP_LOGI("SENSOR","\n\n\n depois da declaração \n\n\n");
	json_room = cJSON_CreateString(room);
	ESP_LOGI("SENSOR","\n\n\n depois do primeiro json \n\n\n");
    while(1){
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
		ESP_LOGI("SENSOR","\n\n\n depois do vtask \n\n\n");

        data = DHT11_read();
	ESP_LOGI("SENSOR","\n\n\n depois do DHT_read \n\n\n");
		
        if(data.temperature > -1){
            printf("Temp: %d\n",data.temperature);
            printf("Umi: %d\n", data.humidity);
            printf("Estado: %d\n", data.status);

			temp_value = cJSON_CreateNumber(data.temperature);
			humid_value = cJSON_CreateNumber(data.humidity);
			
			temp_json = cJSON_CreateObject();
			humid_json = cJSON_CreateObject();

			cJSON_AddItemToObject(temp_json, "room",json_room);
			cJSON_AddItemToObject(humid_json, "room",json_room);
			cJSON_AddItemToObject(temp_json, "temp",temp_value);
			cJSON_AddItemToObject(humid_json, "humidi",humid_value);
			
			mqtt_envia_mensagem(temp_topic, cJSON_Print(temp_json));
			mqtt_envia_mensagem(humid_topic,cJSON_Print(humid_json));
        }else{
			ESP_LOGE("SENSOR","ERRO DE LEITURA");
		}
    }
}