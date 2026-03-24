idf_component_register(SRCS "app_mqtt_client.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "app_sensor" "mqtt" "esp_log" "wifi")
