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

#include "wifi.h"
#include "http_client.h"
#include "mqtt.h"
#include "button.h"
#include "sensor.h"

xSemaphoreHandle semaphSensor;
xSemaphoreHandle semaphButton;
xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;

void SensorData(void *params)
{
	if (xSemaphoreTake(semaphSensor, portMAX_DELAY))
	{
		ReadData();
	}
}

void Button(void *params)
{
	ButtonConfiguration();
}

void conectadoWifi(void *params)
{
	while (true)
	{
		//vTaskDelay(3000 / portTICK_PERIOD_MS);
		if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
		{
			// Processamento Internet
			mqtt_start();
		}
	}
}

void trataComunicacaoComServidor(void *params)
{
	char mensagem[50];
	if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
	{
		while (true)
		{
			float temperatura = 20.0 + (float)rand() / (float)(RAND_MAX / 10.0);
			sprintf(mensagem, "temperatura1: %f", temperatura);
			mqtt_envia_mensagem("fse2020/160010195/dispositivos/8c:aa:b5:8b:52:e0", mensagem);
			vTaskDelay(3000 / portTICK_PERIOD_MS);
		}
	}
}

void app_main(void)
{
	// Inicializa o NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	conexaoWifiSemaphore = xSemaphoreCreateBinary();
	conexaoMQTTSemaphore = xSemaphoreCreateBinary();
	semaphSensor = xSemaphoreCreateBinary();
	semaphButton = xSemaphoreCreateBinary();
	wifi_start();

	xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 2048, NULL, 1, NULL);
	xTaskCreate(&Button, "Botão", 2048, NULL, 1, NULL);
	xTaskCreate(&SensorData, "Leitura do Sensor", 2048, NULL, 1, NULL);
	xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
