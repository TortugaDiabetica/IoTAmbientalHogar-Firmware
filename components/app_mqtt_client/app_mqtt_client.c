#include <stdio.h>
#include "app_mqtt_client.h"




/**
 * @brief - Configuración del cliente mqtt
 */
esp_mqtt_client_config_t config = {
    .broker.address.uri = "192.168.1.20:1883"
};



void mqttInit(void)
{
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&config);
    
    esp_mqtt_client_start(client);
}
