idf_component_register(SRCS "app_wifi_init.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_wifi" "esp_system" "nvs_flash" "app_mqtt_client" "app_events")
