# 📚 Guía Completa: Cliente MQTT con ESP32-C5 e ESP-IDF

## Tabla de Contenidos

1. [Conceptos Básicos de MQTT](#conceptos-básicos)
2. [Instalación de Broker (Mosquitto)](#instalación-mosquitto)
3. [Estructura del Código](#estructura-código)
4. [Tipos de Datos Principales](#tipos-datos)
5. [Funciones Esenciales](#funciones)
6. [Ejemplo Práctico](#ejemplo)
7. [Resolución de Problemas](#troubleshooting)

---

## Conceptos Básicos de MQTT {#conceptos-básicos}

### ¿Qué es MQTT?

**MQTT** (Message Queuing Telemetry Transport) es un protocolo de comunicación ligero basado en **Publish/Subscribe**:

```
┌─────────────┐                    ┌──────────┐                    ┌─────────────┐
│  ESP32 #1   │                    │  Broker  │                    │  ESP32 #2   │
│ Publicador  │──publish───→       │ Mosquitto│       ←───subscribe│ Suscriptor  │
│  Tema: temp │                    │          │                    │  Tema: temp │
└─────────────┘                    └──────────┘                    └─────────────┘
```

### Diferencia con Arduino

| Aspecto       | Arduino            | ESP-IDF                             |
| ------------- | ------------------ | ----------------------------------- |
| WiFi          | Librería simple    | Component completo                  |
| MQTT          | Librerías variadas | Componente espressif/mqtt           |
| Configuración | Simples            | Más opciones (SSL, WebSocket, etc.) |
| Event Loop    | No necesario       | Requerido                           |
| Tasks         | Opcional           | Recomendado (FreeRTOS)              |

### Conceptos Clave

#### **Topics** (Temas)

Son como "direcciones" jerárquicas para los mensajes:

```
home/floor1/room1/temperature
home/floor1/room1/humidity
home/floor2/kitchen/temperature
device/esp32/status
```

#### **QoS** (Quality of Service)

| QoS | Nombre        | Garantía         | Velocidad     | Uso                  |
| --- | ------------- | ---------------- | ------------- | -------------------- |
| 0   | At Most Once  | No garantía      | ⚡⚡⚡ Rápida | Sensores no críticos |
| 1   | At Least Once | Mínimo 1 entrega | ⚡⚡ Media    | Datos importantes    |
| 2   | Exactly Once  | Exactamente 1    | ⚡ Lenta      | Datos críticos       |

#### **Retain** (Retención)

Si `retain=true`:

- El broker guarda el último mensaje
- Nuevos suscriptores lo reciben inmediatamente

```
mqtt_publish("device/power/status", "ON", 0, true);
// Los suscriptores **nuevos** verán "ON" al suscribirse
```

---

## Instalación de Mosquitto en Windows {#instalación-mosquitto}

### Descarga e Instalación

1. **Descargar desde**: https://mosquitto.org/download/
2. **Versión Windows**: `mosquitto-2.0.x-install-windows-x64.exe`
3. **Instalar** y dejar valores por defecto

### Verificar Instalación

```cmd
C:\Program Files\mosquitto> mosquitto.exe

# Deberías ver:
# 1708956234: mosquitto version 2.0.15 starting
# 1708956234: Using config file C:\Program Files\mosquitto\mosquitto.conf
# 1708956234: Opening ipv4 listen socket on port 1883.
```

### Configuración Básica

Archivo: `C:\Program Files\mosquitto\mosquitto.conf`

Asegurar que está descomentado:

```
listener 1883
protocol mqtt
```

### Probar Broker con Ejemplos

En una terminal:

```cmd
# Terminal 1: Suscriptor (escucha)
mosquitto_sub -t "test/topic" -v

# Terminal 2: Publicador
mosquitto_pub -t "test/topic" -m "¡Hola MQTT!"

# En Terminal 1 verás: test/topic ¡Hola MQTT!
```

---

## Estructura del Código {#estructura-código}

### Archivo Header: `app_mqtt_client.h`

Define **tipos de datos** y **declaraciones de funciones**:

```c
// Estructura que almacena estado del cliente
typedef struct {
    esp_mqtt_client_handle_t client;  // Handle (puntero) al cliente
    bool is_connected;                 // ¿Está conectado?
    int message_id;                    // ID del último mensaje
} mqtt_context_t;

// Funciones públicas
esp_err_t mqtt_app_start(const char *broker_uri);
int mqtt_publish(const char *topic, const char *payload, int qos, bool retain);
int mqtt_subscribe(const char *topic, int qos);
bool mqtt_is_connected(void);
```

### Archivo Implementación: `app_mqtt_client.c`

Contiene la **lógica del cliente**:

```c
// 1. Event Handler - Se llama para cada evento MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            // Hicimos conexión -> ahora podemos publicar/suscribirnos
            break;
        case MQTT_EVENT_DATA:
            // Recibimos un mensaje en un topic suscrito
            break;
        // ... más eventos
    }
}

// 2. Inicialización
esp_err_t mqtt_app_start(const char *broker_uri)
{
    // Crear configuración
    esp_mqtt_client_config_t cfg = { ... };

    // Crear cliente
    mqtt_ctx.client = esp_mqtt_client_new(&cfg);

    // Registrar evento handler
    esp_mqtt_client_register_event(..., mqtt_event_handler, ...);

    // Iniciar
    esp_mqtt_client_start(mqtt_ctx.client);
}

// 3. Publicar
int mqtt_publish(const char *topic, const char *payload, int qos, bool retain)
{
    return esp_mqtt_client_publish(mqtt_ctx.client, topic, payload,
                                   strlen(payload), qos, retain);
}
```

---

## Tipos de Datos Principales {#tipos-datos}

### `esp_mqtt_client_handle_t`

Es un **opaco handle** (puntero especial) al cliente MQTT:

```c
esp_mqtt_client_handle_t client;  // No puedes acceder sus campos directamente
                                  // Solo usas funciones esp_mqtt_*
```

### `esp_mqtt_event_handle_t`

Parámetro del event handler con los datos del evento:

```c
void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                        int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    // Campos útiles:
    event->event_id;           // Qué evento es (CONNECTED, DATA, etc)
    event->msg_id;             // ID del mensaje
    event->topic;              // Topic en DATA events
    event->topic_len;          // Longitud del topic
    event->data;               // Payload en DATA events
    event->data_len;           // Longitud del payload
    event->error_handle.error_type;  // Código de error
}
```

### `esp_mqtt_client_config_t`

Configuración del cliente:

```c
esp_mqtt_client_config_t config = {
    .broker.address.uri = "mqtt://localhost:1883",

    // MÁS OPCIONES (ver mqtt_client.h):
    // .credentials.username = "user",
    // .credentials.password = "pass",
    // .session.protocol_ver = MQTT_PROTOCOL_V_3_1_1,
    // .network.reconnect_timeout_ms = 10000,
    // .task.priority = 5,
    // ...
};
```

---

## Funciones Esenciales {#funciones}

### 1️⃣ `mqtt_app_start(broker_uri)`

**Inicializa el cliente MQTT**

```c
esp_err_t mqtt_app_start(const char *broker_uri)
```

**Parámetros:**

- `broker_uri`: URL del broker
    - `"mqtt://localhost:1883"` (Mosquitto local)
    - `"mqtt://broker.hivemq.com:1883"` (Broker público)
    - `"mqtt://192.168.1.50:1883"` (Broker en otra PC)

**Retorna:**

- `ESP_OK` - Éxito
- `ESP_FAIL` - Error

**Ejemplo:**

```c
void app_main() {
    esp_event_loop_create_default();  // REQUERIDO
    mqtt_app_start("mqtt://localhost:1883");
}
```

### 2️⃣ `mqtt_publish(topic, payload, qos, retain)`

**Publica un mensaje**

```c
int mqtt_publish(const char *topic, const char *payload, int qos, bool retain)
```

**Parámetros:**

- `topic`: Nombre del topic (ej: `"sensors/temperature"`)
- `payload`: Contenido del mensaje (string)
- `qos`: 0, 1 o 2 (ver tabla QoS arriba)
- `retain`: `true` para guardar en broker, `false` para temporal

**Retorna:**

- Message ID (≥0) - Éxito
- `-1` - Error

**Ejemplos:**

```c
// Publicar temperatura (simple)
mqtt_publish("esp32/temp", "25.5", 0, false);

// Publicar JSON
char payload[100];
snprintf(payload, sizeof(payload), "{\"temp\": 25.5, \"humid\": 60}");
mqtt_publish("esp32/sensor", payload, 1, false);

// Retener el estado (último conocido)
mqtt_publish("device/status", "ONLINE", 1, true);
```

### 3️⃣ `mqtt_subscribe(topic, qos)`

**Suscribirse a un topic**

```c
int mqtt_subscribe(const char *topic, int qos)
```

**Parámetros:**

- `topic`: Topic a escuchar (puedes usar wildcards: `"home/+/temp"`)
- `qos`: Calidad de servicio

**Retorna:**

- Message ID (≥0) - Solicitud enviada
- `-1` - Error

**Ejemplo:**

```c
// En mqtt_event_handler, evento MQTT_EVENT_CONNECTED:
case MQTT_EVENT_CONNECTED:
    mqtt_subscribe("commands/esp32", 0);
    mqtt_subscribe("settings/+/value", 1);
    break;

// En mqtt_event_handler, evento MQTT_EVENT_DATA:
case MQTT_EVENT_DATA:
    printf("Tema: %.*s\n", event->topic_len, event->topic);
    printf("Mensaje: %.*s\n", event->data_len, event->data);
    break;
```

### 4️⃣ `mqtt_is_connected()`

**Verifica si está conectado**

```c
bool mqtt_is_connected(void)
```

**Ejemplo:**

```c
if (mqtt_is_connected()) {
    mqtt_publish("sensors/temp", "25.5", 0, false);
} else {
    printf("Esperando conexión MQTT...\n");
}
```

---

## Ejemplo Práctico {#ejemplo}

### Paso 1: Instalación Previa

1. ✅ Mosquitto corriendo en `localhost:1883`
2. ✅ ESP32-C5 conectado a WiFi
3. ✅ Archivos actualizados (app_mqtt_client.h, app_mqtt_client.c)

### Paso 2: Probar con Mosquitto

Terminal 1 - Suscriptor (escucha):

```cmd
mosquitto_sub -t "esp32/sensors/#" -v
```

Terminal 2 - Compilar y ejecutar ESP32:

```bash
idf.py build flash monitor
```

**Salida esperada en ESP32:**

```
[APP_MAIN] Inicializando WiFi...
[APP_MAIN] Inicializando MQTT...
[MQTT_CLIENT] Cliente MQTT iniciado. Conectando a: mqtt://192.168.1.100:1883
[MQTT_CLIENT] ✓ Conectado al broker MQTT
[MQTT_CLIENT] Mensaje publicado: topic=esp32/sensors/dht11, msg_id=1
```

**Salida en Terminal 1:**

```
esp32/sensors/dht11 {"temperatura": 25.5, "humedad": 60.0, "timestamp": 1708956234}
esp32/sensors/dht11 {"temperatura": 25.5, "humedad": 60.0, "timestamp": 1708956240}
```

### Paso 3: Enviar Comandos al ESP32

Terminal 3 - Enviar comando:

```cmd
mosquitto_pub -t "commands/esp32" -m "LED_ON"
```

En el código (mqtt_event_handler):

```c
case MQTT_EVENT_DATA:
    if (strncmp(event->topic, "commands/esp32", event->topic_len) == 0) {
        if (strncmp(event->data, "LED_ON", event->data_len) == 0) {
            // Encender LED
            gpio_set_level(LED_PIN, 1);
        }
    }
    break;
```

---

## Resolución de Problemas {#troubleshooting}

### ❌ "No estoy conectado. No puedo publicar"

**Causa**: La conexión MQTT aún no está lista

**Solución**:

```c
// MALO - No espera conexión
mqtt_app_start("mqtt://localhost:1883");
mqtt_publish("test", "data", 0, false);  // Falla ❌

// CORRECTO - Espera conexión
mqtt_app_start("mqtt://localhost:1883");
vTaskDelay(5000 / portTICK_PERIOD_MS);  // Esperar 5s
while (!mqtt_is_connected()) vTaskDelay(100 / portTICK_PERIOD_MS);
mqtt_publish("test", "data", 0, false);  // OK ✓
```

### ❌ "Connection refused: 111"

**Causa**: Broker no está disponible en esa dirección

**Soluciones**:

- Verificar IP correcta: `ipconfig` (Windows)
- Verificar puerto: `netstat -an | findstr :1883`
- Iniciar Mosquitto: `mosquitto.exe`
- Cambiar a `"mqtt://localhost:1883"` si es local

### ❌ No aparecen mensajes en mosquitto_sub

**Causa**:

- Topic no coincide (case-sensitive)
- No está suscrito a tiempo
- Payload vacío

**Solución**:

```c
// Usar wildcard para recibir todo
mosquitto_sub -t "#" -v

// Verificar que topic coincida exactamente
mqtt_publish("esp32/sensors/dht11", "...", 0, false);
mosquitto_sub -t "esp32/sensors/dht11" -v  // ✓ Correcto
mosquitto_sub -t "esp32/sensors/DHT11" -v  // ✗ No funciona (mayúscula)
```

### ❌ Memory leak / Crash en publicación

**Causa**: String payload no NULL-terminado

**Solución**:

```c
// MALO
const char *msg = "test";  // ¿Cuántos bytes?
mqtt_publish("topic", msg, 0, false);  // Puede leer basura

// CORRECTO
char msg[] = "test";
mqtt_publish("topic", msg, 0, false);  // strlen() = 4

// O con snprintf
char payload[100];
snprintf(payload, sizeof(payload), "temp=%.1f", 25.5);
mqtt_publish("topic", payload, 0, false);
```

---

## Referencias Oficiales

- ESP-MQTT Docs: https://docs.espressif.com/projects/esp-mqtt
- MQTT Spec: https://mqtt.org/
- Mosquitto: https://mosquitto.org/
- Tutoriales: https://github.com/espressif/esp-mqtt/tree/master/examples

---

**¡Éxito en tu proyecto IoT!** 🚀
