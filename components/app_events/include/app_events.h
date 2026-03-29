#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

// Event Group global
extern EventGroupHandle_t system_event_group;

// Bits
#define WIFI_CONNECTED_BIT BIT0 // 1 << 0
#define MQTT_CONNECTED_BIT BIT1 // 1 << 1
