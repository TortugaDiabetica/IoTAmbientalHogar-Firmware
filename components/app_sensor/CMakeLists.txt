idf_component_register(SRCS "app_sensor.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "driver" "freertos" "esp_system" "app_mqtt_client" "app_events")
