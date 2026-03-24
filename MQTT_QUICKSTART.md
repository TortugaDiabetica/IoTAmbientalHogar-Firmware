# ⚡ Quick Start MQTT - Checklist y Snippets

## ✅ Checklist Antes de Compilar

### 1. Dependencias en CMakeLists.txt

- [ ] `components/app_mqtt_client/CMakeLists.txt` incluye `"mqtt"`
- [ ] `main/CMakeLists.txt` incluye `"app_mqtt_client"`

### 2. Configuración Requerida en ESP-IDF

```bash
idf.py menuconfig
# Ir a: Component config → MQTT Configuration
# Verificar que está habilitado
```

### 3. WiFi Configurada en main.c

- [ ] SSID correcto
- [ ] Password correcto
- [ ] Función `wifi_init_sta()` llamada

### 4. Broker Disponible

- [ ] Mosquitto corriendo en Windows
    - [ ] Puerto 1883 abierto
    - [ ] Direccionble: `mosquitto.exe` iniciado
- [ ] O cambiar URL a broker público (HiveMQ)

---

## 🔧 Snippets Rápidos de Código

### Snippet 1: Inicialización Completa

```c
#include "app_mqtt_client.h"

void app_main(void) {
    esp_event_loop_create_default();
    wifi_init_sta();
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    mqtt_app_start("mqtt://192.168.1.100:1883");

    while (1) {
        if (mqtt_is_connected()) {
            mqtt_publish("test/topic", "hello", 0, false);
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
```

### Snippet 2: Event Handler Custom (agregar en app_mqtt_client.c)

```c
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "✓ MQTT Conectado");
            mqtt_ctx.is_connected = true;
            mqtt_subscribe("my/commands", 0);
            break;

        case MQTT_EVENT_DATA:
            // Procesar datos recibidos
            printf("Topic: %.*s\n", event->topic_len, event->topic);
            printf("Data: %.*s\n", event->data_len, event->data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "Error: %d", event->error_handle.error_type);
            break;

        default:
            break;
    }
}
```

### Snippet 3: Publicar Cada 5 Segundos

```c
static void publish_task(void *pvParameters) {
    while (!mqtt_is_connected()) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    while (1) {
        char msg[64];
        snprintf(msg, sizeof(msg), "{\"value\":%d}", rand() % 100);
        mqtt_publish("esp32/data", msg, 1, false);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

// En app_main():
xTaskCreate(publish_task, "Publish", 4096, NULL, 2, NULL);
```

### Snippet 4: Manejar Comandos

```c
// En mqtt_event_handler, caso DATA:
case MQTT_EVENT_DATA:
    if (strncmp(event->topic, "commands", 8) == 0) {
        if (strncmp(event->data, "ON", 2) == 0) {
            gpio_set_level(GPIO_NUM_2, 1);
        } else if (strncmp(event->data, "OFF", 3) == 0) {
            gpio_set_level(GPIO_NUM_2, 0);
        }
    }
    break;
```

### Snippet 5: JSON Helper

```c
#include <cJSON.h>  // Si está disponible

char* create_sensor_msg(float temp, float humid) {
    static char json_str[256];
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temperature", temp);
    cJSON_AddNumberToObject(root, "humidity", humid);
    cJSON_AddNumberToObject(root, "timestamp", time(NULL));

    char *str = cJSON_Print(root);
    snprintf(json_str, sizeof(json_str), "%s", str);
    cJSON_Delete(root);
    free(str);

    return json_str;
}
```

---

## 🐛 Troubleshooting Rápido

| Problema              | Causa                        | Solución                                           |
| --------------------- | ---------------------------- | -------------------------------------------------- |
| "No estoy conectado"  | MQTT no conectó              | Esperar con `while (!mqtt_is_connected())`         |
| "URI vacío"           | broker_uri = ""              | Cambiar en main.c a URL válida                     |
| msg_id = -1           | No conectado o payload vacío | Revisar `mqtt_is_connected()` y payload            |
| No recibe datos       | No suscrito                  | Agregar `mqtt_subscribe()` en MQTT_EVENT_CONNECTED |
| Mosquitto no responde | No corriendo                 | `mosquitto.exe` en cmd.exe                         |
| Connection refused    | IP/puerto incorrectos        | `ipconfig` en cmd.exe, verificar puerto 1883       |

---

## 📱 Probar sin Compilar (Usar Broker Público)

Cambiar URL en main.c:

```c
mqtt_app_start("mqtt://broker.hivemq.com:1883");
```

Luego en cmd.exe:

```cmd
mosquitto_sub -h broker.hivemq.com -t "mi/topic" -v
```

En otra terminal:

```cmd
mosquitto_pub -h broker.hivemq.com -t "mi/topic" -m "test"
```

---

## 🎯 Casos de Uso Comunes

### Caso 1: Publicar temperatura cada minuto

```c
mqtt_publish("home/temperature", "22.5", 1, false);
vTaskDelay(60000 / portTICK_PERIOD_MS);
```

### Caso 2: Alertas críticas

```c
mqtt_publish("alerts/fire", "DETECTED", 2, false);  // QoS 2
```

### Caso 3: Retenido = Estado actual

```c
mqtt_publish("device/mode", "AUTO", 1, true);  // Retener
```

### Caso 4: Sincronización de múltiples ESP32

```c
mqtt_subscribe("sync/+/request", 1);
// En handler, responder a "sync/esp32-1/request"
```

---

## 📚 Archivos del Proyecto

```
monitoreo_IoT/
├── components/app_mqtt_client/
│   ├── include/app_mqtt_client.h    ← Tipos y declaraciones
│   ├── app_mqtt_client.c             ← Implementación
│   └── CMakeLists.txt
├── main/main.c                       ← Punto de entrada
├── GUIA_MQTT.md                      ← Documentación completa
└── examples_mqtt.c                   ← Ejemplos de uso
```

---

## 🚀 Próximos Pasos

1. **Configurar Mosquitto** → Ejecutar `mosquitto.exe`
2. **Actualizar WiFi** → Cambiar SSID y PASSWORD en main.c
3. **Actualizar Broker IP** → Cambiar URL en main.c
4. **Compilar** → `idf.py build flash monitor`
5. **Probar** → `mosquitto_sub` en otra terminal

---

Más info: https://docs.espressif.com/projects/esp-mqtt/
