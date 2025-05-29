# Guía: Leer Sensor DHT11 con Arduino Nano 33 IoT y Enviar Datos por MQTT

Esta guía te mostrará cómo conectar un sensor de temperatura y humedad DHT11 a un Arduino Nano 33 IoT, leer sus valores y publicarlos en un broker MQTT a través de WiFi.

## Prerrequisitos Hardware

1.  **Arduino Nano 33 IoT:** Con su cabezal de pines soldado si es necesario.
2.  **Sensor DHT11:** (El DHT22 es similar pero más preciso, el código sería casi idéntico).
3.  **Resistencia de pull-up de 10kΩ:** Opcional si tu módulo DHT11 ya la incluye (muchos módulos de 3 pines la tienen integrada). Si tienes el sensor DHT11 de 4 pines "suelto", la necesitarás.
4.  **Cables de conexión (jumpers).**
5.  **Cable Micro USB** para programar el Arduino.
6.  **Acceso a una red WiFi.**
7.  **Broker MQTT configurado y accesible** (como el que configuramos en la guía anterior en tu servidor Ubuntu).

## Prerrequisitos Software

1.  **IDE de Arduino:** Instalado y configurado.
2.  **Placa "Arduino SAMD Boards (32-bits ARM Cortex-M0+)" instalada en el Gestor de Placas del IDE de Arduino.**
3.  **Librerías de Arduino necesarias:**
    *   `WiFiNINA`: Para la conectividad WiFi del Nano 33 IoT.
    *   `PubSubClient` de Nick O'Leary: Para la comunicación MQTT.
    *   `DHT sensor library` de Adafruit: Para leer el sensor DHT.
    *   `Adafruit Unified Sensor` de Adafruit: Requerida por la librería DHT.

## Paso 1: Instalar las Librerías de Arduino

1.  Abre el IDE de Arduino.
2.  Ve a `Herramientas` -> `Gestionar Librerías...` (o `Sketch` -> `Incluir Librería` -> `Gestionar Librerías...`).
3.  Busca e instala las siguientes librerías una por una:
    *   **`WiFiNINA`** (probablemente ya instalada si has usado el WiFi del Nano 33 IoT antes).
    *   **`PubSubClient`** (asegúrate de que sea la de Nick O'Leary).
    *   **`DHT sensor library`** (de Adafruit Industries).
    *   **`Adafruit Unified Sensor`** (de Adafruit Industries). Cuando instales la librería DHT, es posible que te pregunte si también quieres instalar esta dependencia; di que sí.

## Paso 2: Conexión del Hardware (Sensor DHT11 al Arduino)

**Si usas un módulo DHT11 de 3 pines (común):**
*   **VCC (o +):** Conectar a `3.3V` en el Arduino Nano 33 IoT. (¡Importante! El DHT11 funciona bien a 3.3V y es más seguro para el Nano 33 IoT).
*   **DATA (o S, o OUT):** Conectar a un pin digital del Arduino. Usaremos el pin `D2` en este ejemplo.
*   **GND (o -):** Conectar a `GND` en el Arduino.

**Si usas un sensor DHT11 de 4 pines "suelto" (sin placa de módulo):**
*   **Pin 1 (VCC):** Conectar a `3.3V` en el Arduino.
*   **Pin 2 (DATA):** Conectar al pin digital `D2` del Arduino. **Y también conectar una resistencia de 10kΩ entre este pin DATA y el pin 3.3V (pull-up).**
*   **Pin 3 (NC - No Conectado):** Dejar sin conectar.
*   **Pin 4 (GND):** Conectar a `GND` en el Arduino.

**Diagrama de conexión simplificado (para módulo de 3 pines):**

```
Arduino Nano 33 IoT        DHT11 Module
--------------------        ------------
      3.3V  ----------------  VCC (+)
      GND   ----------------  GND (-)
      D2    ----------------  DATA (S)
```

## Paso 3: Código del Arduino

Copia y pega el siguiente código en un nuevo sketch del IDE de Arduino.

**Modifica las siguientes constantes en el código según tu configuración:**

*   `ssid`: Nombre de tu red WiFi.
*   `pass`: Contraseña de tu red WiFi.
*   `mqtt_server`: Dirección IP de tu broker MQTT (la IP de tu servidor Ubuntu).
*   `mqtt_user` y `mqtt_password`: Si configuraste autenticación en tu broker Mosquitto. Si permites conexiones anónimas, puedes dejar estas cadenas vacías o usar la versión de `mqttClient.connect()` que solo toma el `clientID`.

```cpp
// Incluir las librerías necesarias
#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>

// --- Configuración WiFi ---
char ssid[] = "NOMBRE_DE_TU_WIFI";        // Tu SSID de red
char pass[] = "CONTRASEÑA_DE_TU_WIFI";    // Tu contraseña de red
int status = WL_IDLE_STATUS;              // Estado de la conexión WiFi

// --- Configuración MQTT ---
const char* mqtt_server = "IP_DE_TU_BROKER_MQTT"; // IP de tu servidor Mosquitto
const int mqtt_port = 1883;                      // Puerto MQTT estándar
// Descomenta y configura si tu broker requiere autenticación
// const char* mqtt_user = "tu_usuario_mqtt";
// const char* mqtt_password = "tu_password_mqtt";
const char* clientID = "arduinoNano33IoT_DHT11"; // ID de cliente MQTT único

// Tópicos MQTT
const char* topicTemp = "tecsup/temperatura";
const char* topicHum = "tecsup/humedad";

// --- Configuración Sensor DHT ---
#define DHTPIN 2        // Pin digital D2 conectado al pin DATA del DHT11
#define DHTTYPE DHT11   // Define el tipo de sensor DHT (DHT11, DHT22, DHT21)
DHT dht(DHTPIN, DHTTYPE);

// --- Variables Globales ---
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastMsgTime = 0;
const long interval = 10000; // Intervalo entre envíos de datos (10 segundos)

// --- FUNCIONES ---

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  // Esta función se llama cuando llega un mensaje a un tópico suscrito
  // No esperamos mensajes entrantes en este ejemplo, pero es bueno tenerla.
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnectMQTT() {
  // Bucle hasta que nos reconectemos
  while (!mqttClient.connected()) {
    Serial.print("Intentando conexión MQTT...");
    // Intenta conectar
    // Si tu broker no requiere usuario/contraseña, usa:
    // if (mqttClient.connect(clientID)) {
    // Si tu broker SÍ requiere usuario/contraseña:
    if (mqttClient.connect(clientID /*, mqtt_user, mqtt_password*/ )) { // Descomenta usuario/pass si es necesario
      Serial.println("conectado!");
      // Puedes suscribirte a tópicos aquí si es necesario
      // mqttClient.subscribe("inTopic");
    } else {
      Serial.print("falló, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      // Espera 5 segundos antes de reintentar
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Espera a que el puerto serial se conecte (necesario para algunos Arduinos)

  pinMode(LED_BUILTIN, OUTPUT); // Para feedback visual
  digitalWrite(LED_BUILTIN, LOW);

  dht.begin(); // Inicializa el sensor DHT

  setup_wifi(); // Conecta a WiFi

  mqttClient.setServer(mqtt_server, mqtt_port); // Configura el servidor MQTT
  mqttClient.setCallback(callbackMQTT);         // Configura la función de callback para mensajes MQTT

  Serial.println("Setup completado.");
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT(); // Si no está conectado a MQTT, intenta reconectar
  }
  mqttClient.loop(); // Esencial para mantener la conexión MQTT y procesar mensajes

  unsigned long currentMillis = millis();
  if (currentMillis - lastMsgTime >= interval) {
    lastMsgTime = currentMillis;

    // Leer Humedad
    float h = dht.readHumidity();
    // Leer Temperatura en Celsius
    float t = dht.readTemperature();
    // Leer Temperatura en Fahrenheit (opcional)
    // float f = dht.readTemperature(true);

    // Verificar si las lecturas fallaron (devuelve NaN en caso de error)
    if (isnan(h) || isnan(t)) {
      Serial.println("Error al leer del sensor DHT!");
      return;
    }

    // Convertir los floats a cadenas de caracteres para publicar
    char tempString[8]; // Suficiente para "xx.xx" más el nulo
    dtostrf(t, 4, 2, tempString); // 4 es el ancho mínimo, 2 son los decimales

    char humString[8];
    dtostrf(h, 4, 2, humString);

    Serial.print("Temperatura: ");
    Serial.print(tempString);
    Serial.print(" °C, Humedad: ");
    Serial.print(humString);
    Serial.println(" %");

    // Publicar en MQTT
    if (mqttClient.publish(topicTemp, tempString)) {
      Serial.print("Publicado en ");
      Serial.print(topicTemp);
      Serial.print(": ");
      Serial.println(tempString);
      digitalWrite(LED_BUILTIN, HIGH); // Encender LED brevemente al publicar
    } else {
      Serial.println("Fallo al publicar temperatura");
    }

    if (mqttClient.publish(topicHum, humString)) {
      Serial.print("Publicado en ");
      Serial.print(topicHum);
      Serial.print(": ");
      Serial.println(humString);
    } else {
      Serial.println("Fallo al publicar humedad");
    }
    
    delay(100); // Pequeña pausa
    digitalWrite(LED_BUILTIN, LOW); // Apagar LED
  }
}
```

**Explicación del Código:**

1.  **Includes:** Carga las librerías necesarias.
2.  **Configuración WiFi y MQTT:** Define tus credenciales de red, la IP del broker MQTT y los tópicos.
3.  **Configuración DHT:** Define el pin al que está conectado el sensor y el tipo de sensor (DHT11).
4.  **Variables Globales:** Clientes WiFi y MQTT, y variables para controlar el intervalo de envío.
5.  **`setup_wifi()`:** Función para conectarse a la red WiFi.
6.  **`callbackMQTT()`:** Función que se ejecutaría si el Arduino recibiera mensajes MQTT (no se usa activamente para enviar, pero es requerida por `PubSubClient`).
7.  **`reconnectMQTT()`:** Intenta conectarse o reconectarse al broker MQTT si la conexión se pierde.
8.  **`setup()`:**
    *   Inicializa la comunicación serial y el sensor DHT.
    *   Llama a `setup_wifi()`.
    *   Configura el servidor MQTT y la función `callbackMQTT`.
9.  **`loop()`:**
    *   Mantiene la conexión MQTT usando `mqttClient.loop()`.
    *   Verifica si es momento de enviar nuevos datos (según `interval`).
    *   Lee los valores de temperatura y humedad del sensor DHT.
    *   Comprueba si las lecturas son válidas (no `NaN`).
    *   Convierte los valores flotantes a cadenas de caracteres usando `dtostrf()`.
    *   Publica los valores de temperatura y humedad en sus respectivos tópicos MQTT.
    *   Proporciona feedback visual con el LED integrado.

## Paso 4: Subir el Código al Arduino

1.  Conecta tu Arduino Nano 33 IoT a tu computadora mediante el cable USB.
2.  En el IDE de Arduino, ve a `Herramientas` -> `Placa` y selecciona "Arduino Nano 33 IoT".
3.  Ve a `Herramientas` -> `Puerto` y selecciona el puerto COM al que está conectado tu Arduino.
4.  Haz clic en el botón "Subir" (la flecha hacia la derecha).

## Paso 5: Probar y Verificar

1.  **Abrir el Monitor Serie:**
    *   En el IDE de Arduino, ve a `Herramientas` -> `Monitor Serie`.
    *   Asegúrate de que la velocidad en baudios esté configurada en `9600`.
    *   Observarás los mensajes de conexión WiFi, MQTT, y las lecturas del sensor siendo publicadas.

2.  **Suscribirse a los Tópicos MQTT en el Servidor:**
    En una terminal de tu servidor Ubuntu (o cualquier cliente MQTT), suscríbete a los tópicos para ver los datos entrantes:
    *   **Comando (para ver ambos tópicos):**
        ```bash
        mosquitto_sub -h localhost -t "tecsup/#" -v
        ```
        (Si estás en una máquina diferente al broker, reemplaza `localhost` con la IP del broker).
        (Si tu broker requiere autenticación, añade `-u "tu_usuario" -P "tu_password"`).

    *   **Comando (para el tópico de temperatura):**
        ```bash
        mosquitto_sub -h localhost -t "tecsup/temperatura" -v
        ```
    *   **Comando (para el tópico de humedad):**
        ```bash
        mosquitto_sub -h localhost -t "tecsup/humedad" -v
        ```

    Deberías ver los valores de temperatura y humedad publicados por tu Arduino Nano 33 IoT cada 10 segundos (o el intervalo que hayas configurado).

**Ejemplo de salida en `mosquitto_sub`:**
```
tecsup/temperatura 25.50
tecsup/humedad 60.20
tecsup/temperatura 25.60
tecsup/humedad 60.10
...
```
