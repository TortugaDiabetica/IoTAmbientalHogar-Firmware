// INCLUDES
#include <stdio.h>
#include "app_mqtt_client.h"

// MACROS
/**
 * @brief - Configuración del cliente mqtt
 */
esp_mqtt_client_config_t config = {
    .broker.address.uri = "mqtt://192.168.1.20:1883"
};

static esp_mqtt_client_handle_t client = NULL;

// MANEJADOR DE EVENTOS MQTT (actúa como callback):
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // esp_mqtt_event_handle_t event = event_data;

    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED:
        printf("Conectado al broker raspberry!\n");

        esp_mqtt_client_publish(
            client,
            "hidroponia/test",
            "hola desde firmware :D",
            0,
            1,
            0
        );
        break;

    case MQTT_EVENT_DATA:
        printf("Mensaje recibido\n");
        break;
    case MQTT_EVENT_DISCONNECTED:
        printf("Desconectado del broker\n");
    break;
    default:
        break;
    }
}

void mqtt_init() {
    client = esp_mqtt_client_init(&config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}