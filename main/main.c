#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "app_wifi_init.h"
#include "app_sensor.h"
#include "app_mqtt_client.h"
#include "app_events.h"


void app_main(void)
{
    printf("\n=====MONITOREO IOT HUMEDAD/TEMPERATURA====\n");
    system_event_group = xEventGroupCreate();

    wifi_init();
    
    xTaskCreate(readDHT11, "Lectura de sensor DHT11", 2048, NULL, 1, NULL);
}
