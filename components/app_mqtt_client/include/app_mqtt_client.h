#include "mqtt_client.h"
#include "app_sensor.h"
#include "app_events.h"

#ifndef APP_MQTT_CLIENT_H
#define APP_MQTT_CLIENT_H

void mqtt_init();
extern bool mqtt_connected;
void mqtt_publish_sensor_data(const char *json_payload);

#endif
