#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "app_sensor.h"
#include "app_wifi_init.h"

void app_main(void)
{
    printf("=== Sistema de Monitoreo IoT ===\n");
    wifi_init();

    // Crear tarea para el sensor DHT11
    // printf("Iniciando sensor DHT11...\n");
    // xTaskCreate(readDHT11, "Lectura sensor DHT11", 4096, NULL, 1, NULL);
}
