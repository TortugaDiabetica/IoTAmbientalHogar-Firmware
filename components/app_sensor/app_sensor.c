// INCLUDES
#include <stdio.h>
#include "app_sensor.h"

// MACROS Y CONSTANTES
#define DHT_PIN GPIO_NUM_2

// FUNCIONES
void readDHT11(void *pvParameters)
{
    int16_t humedad;
    int16_t temperatura;

    // Inicializar el GPIO
    esp_rom_gpio_pad_select_gpio(DHT_PIN);
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT_OUTPUT);
    
    // Esperar a que el sistema se estabilice (igual que en el código anterior)
    vTaskDelay(pdMS_TO_TICKS(4000));

    while (1)
    {
        esp_err_t res = dht_read_data(DHT_TYPE_DHT11, DHT_PIN, &humedad, &temperatura);
        if (res == ESP_OK) {
            printf("\n-----------------------\n");
            printf("Temp: %.1f | Hum: %.1f", temperatura / 10.0, humedad / 10.0);
            printf("\n-----------------------\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
        } else {
            ESP_LOGE("ERROR", "El sensor no responde...");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
    vTaskDelete(NULL);
}
