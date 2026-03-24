#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "app_sensor.h"

void app_main(void)
{
    printf("=== Sistema de Monitoreo IoT ===\n");
    
    // Crear tarea para el sensor DHT11
    printf("Iniciando sensor DHT11...\n");
    xTaskCreate(readDHT11, "Lectura sensor DHT11", 4096, NULL, 1, NULL);
}
