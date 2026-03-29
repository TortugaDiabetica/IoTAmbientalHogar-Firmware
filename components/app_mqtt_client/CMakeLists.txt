idf_component_register(SRCS "app_mqtt_client.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "mqtt" "esp_system" "app_sensor" "dht" "app_events")
