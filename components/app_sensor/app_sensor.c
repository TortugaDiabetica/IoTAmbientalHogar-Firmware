// INCLUDES
#include <stdio.h>
#include "app_sensor.h"
#include "app_mqtt_client.h"

// MACROS Y CONSTANTES
#define DHT_PIN GPIO_NUM_2

// PROTOTIPOS
void dht11_data_to_json(int16_t temperatura, int16_t humedad, char *json_buffer, size_t buffer_size);

// FUNCIONES
void readDHT11(void *pvParameters)
{
    int16_t humedad;
    int16_t temperatura;

    // Inicializar el GPIO
    esp_rom_gpio_pad_select_gpio(DHT_PIN);
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT_OUTPUT);
    vTaskDelay(pdMS_TO_TICKS(4000));

    while (1)
    {
        esp_err_t res = dht_read_data(DHT_TYPE_DHT11, DHT_PIN, &humedad, &temperatura);
        if (res == ESP_OK) {
            char json_str[64]; // Buffer creado para el payload que será enviado a mqtt
            dht11_data_to_json(temperatura, humedad, json_str, sizeof(json_str));
            printf("\n-----------------------\n");
            printf("Temp: %.1f | Hum: %.1f\n", temperatura / 10.0, humedad / 10.0);
            printf("JSON: %s\n", json_str);
            printf("-----------------------\n");
            mqtt_publish_sensor_data(json_str);
            vTaskDelay(pdMS_TO_TICKS(3000));
        } else {
            ESP_LOGE("ERROR", "El sensor no responde...");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
    vTaskDelete(NULL);
}

void dht11_data_to_json(int16_t temperatura, int16_t humedad, char *json_buffer, size_t buffer_size) {
    snprintf(json_buffer, buffer_size, "{\"temperatura\":%.1f,\"humedad\":%.1f}", temperatura / 10.0, humedad / 10.0);
}

