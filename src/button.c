#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "button.h"

#define BOTAO_1 0

xQueueHandle filaDeInterrupcao;

extern xSemaphoreHandle semaphButton;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void trataInterrupcaoBotao()
{
  int pino;
  int contador = 0;

  while(true)
  {
    //xSemaphoreTake(semaphButton, portMAX_DELAY);
    if(xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
    {
      // De-bouncing
      int estado = gpio_get_level(pino);
      if(estado == 1)
      {
        gpio_isr_handler_remove(pino);
        while(gpio_get_level(pino) == estado)
        {
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        contador++;
        printf("Os botões foram acionados %d vezes. Botão: %d\n", contador, pino);

        // Habilitar novamente a interrupção
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
      }

    }
    //xSemaphoreGive(semaphButton);
  }
}

void ButtonConfiguration(){

  // Configuração do pino do Botão
  gpio_pad_select_gpio(BOTAO_1);
  // Configura o pino do Botão como Entrada
  gpio_set_direction(BOTAO_1, GPIO_MODE_INPUT);
  // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
  gpio_pulldown_en(BOTAO_1);
  // Desabilita o resistor de Pull-up por segurança.
  gpio_pullup_dis(BOTAO_1);

  // Configura pino para interrupção
  gpio_set_intr_type(BOTAO_1, GPIO_INTR_POSEDGE);

  filaDeInterrupcao = xQueueCreate(10, sizeof(int));
  
  gpio_install_isr_service(0);
  gpio_isr_handler_add(BOTAO_1, gpio_isr_handler, (void *) BOTAO_1);

  trataInterrupcaoBotao();

}

