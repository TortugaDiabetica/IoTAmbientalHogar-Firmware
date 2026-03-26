#include "driver/gpio.h"
#include "dht.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#ifndef APP_SENSOR_H
#define APP_SENSOR_H

void readDHT11(void *pvParameters);
void dht11_data_to_json(int16_t temperatura, int16_t humedad, char *json_buffer, size_t buffer_size);

#endif