#include <stdio.h>
#include "app_wifi_init.h"

// DEFINES
#define SSID        "WiFi_Mesh-560103"
#define PASSWORD    "HA97NzS7"

static const char *TAG = "WifiHogar";

static void wifi_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *data_event) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Conectandose...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(system_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGW(TAG, "Reconectando...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        printf("WiFi conectado! Iniciando MQTT...\n");
        xEventGroupSetBits(system_event_group, WIFI_CONNECTED_BIT);
        mqtt_init();
    }
}

void wifi_init(){
    // Iniciando memoria NVS por defecto:
    nvs_flash_init();
    
    // Inicializando RED:
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);

    // Registrar eventos
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    // Evento Wifi
    esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        &instance_any_id
    );

    // Evento IP
    esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL,
        &instance_got_ip
    );

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PASSWORD,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    esp_wifi_start();
}