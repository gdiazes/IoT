### **Guía de Laboratorio: Control Directo de un LED con Arduino Nano 33 IoT y Alexa**

#### **1. Introducción y Objetivos**

Esta práctica de laboratorio demuestra el poder de los microcontroladores modernos con conectividad integrada. Se utilizará un Arduino Nano 33 IoT para conectarse directamente a servicios en la nube, permitiendo el control de un LED mediante comandos de voz a través de Amazon Alexa. Esta arquitectura elimina la necesidad de un gateway intermedio (como un ordenador), representando un sistema IoT más robusto y autónomo.

Al finalizar esta guía, el estudiante será capaz de:

*   **Implementar** una arquitectura IoT directa Nube-Dispositivo.
*   **Configurar** un Arduino Nano 33 IoT para conectarse a una red WiFi.
*   **Utilizar** librerías de cliente MQTT para suscribirse a un feed de Adafruit IO.
*   **Programar** el microcontrolador para reaccionar en tiempo real a los mensajes recibidos desde la nube.
*   **Integrar** múltiples servicios en la nube (Alexa, IFTTT, Adafruit IO) para crear una solución de domótica funcional.

#### **2. Prerrequisitos**

*   **Hardware:**
    *   **Arduino Nano 33 IoT**.
    *   1 LED (cualquier color).
    *   1 Resistencia de 220Ω o 330Ω.
    *   Protoboard y cables de conexión.
    *   Cable Micro USB.
*   **Software:**
    *   [Arduino IDE](https://www.arduino.cc/en/software) instalado.
    *   **Soporte para placas SAMD:** En el Arduino IDE, ir a `Herramientas > Gestor de Placas`, buscar "SAMD" e instalar "Arduino SAMD Boards (32-bits ARM Cortex-M0+)".
    *   **Librerías necesarias:** Ir a `Herramientas > Gestionar Librerías...` y buscar e instalar las siguientes:
        1.  `WiFiNINA` (para la conectividad WiFi).
        2.  `Adafruit MQTT Library` (para la comunicación con Adafruit IO).
        3.  `Arduino_JSON` (puede ser necesaria como dependencia).
*   **Cuentas (gratuitas):**
    *   Cuenta de Amazon con un dispositivo Alexa configurado.
    *   Cuenta en [IFTTT.com](https://ifttt.com/).
    *   Cuenta en [Adafruit.io](https://io.adafruit.com/).

#### **3. Procedimiento Detallado**

**Parte I y II: Configuración de la Nube (Adafruit IO e IFTTT)**

Estas partes son **exactamente iguales** a la guía anterior, ya que la configuración de los servicios en la nube no cambia.

1.  **En Adafruit IO:**
    *   Crear un **Feed** llamado `luz-laboratorio`.
    *   (Opcional) Crear un **Dashboard** con un bloque de **Toggle** (interruptor) vinculado a ese feed para pruebas.
    *   Copiar y guardar tu **Username** y **Active Key** de Adafruit IO.

2.  **En IFTTT:**
    *   Crear un **Applet para encender**:
        *   **If This:** Amazon Alexa > "Say a specific phrase" > `enciende la luz del laboratorio`.
        *   **Then That:** Adafruit > "Send data to Adafruit IO" > Feed `luz-laboratorio` > Data to save: `ON`.
    *   Crear un **Applet para apagar**:
        *   **If This:** Amazon Alexa > "Say a specific phrase" > `apaga la luz del laboratorio`.
        *   **Then That:** Adafruit > "Send data to Adafruit IO" > Feed `luz-laboratorio` > Data to save: `OFF`.

**Parte III: Montaje y Programación del Arduino Nano 33 IoT**

Aquí es donde todo cambia. El montaje físico es similar, pero el código es mucho más avanzado, ya que manejará la conexión WiFi y la comunicación con la nube directamente.

1.  **Montaje del Circuito:**
    *   Conecta el pin largo (ánodo) del LED a una resistencia.
    *   Conecta el otro extremo de la resistencia al pin digital **D4**.
    *   Conecta el pin corto (cátodo) del LED a un pin **GND** del Arduino Nano 33 IoT.

2.  **Preparar las Credenciales en el Código:**
    *   El método más seguro para manejar credenciales en Arduino es usando un archivo de cabecera separado.
    *   En tu sketch de Arduino, haz clic en la flecha de la esquina superior derecha y selecciona **"Nueva Pestaña"**.
    *   Nombra el nuevo archivo `arduino_secrets.h`.
    *   En este nuevo archivo, pega el siguiente código y rellena tus datos:

    ```cpp
    // Archivo: arduino_secrets.h
    #define SECRET_SSID "TU_NOMBRE_DE_WIFI"
    #define SECRET_PASS "TU_CONTRASENA_DE_WIFI"
    #define IO_USERNAME "TU_USERNAME_DE_ADAFRUIT"
    #define IO_KEY "TU_KEY_DE_ADAFRUIT"
    ```

3.  **Código Principal del Arduino:**
    *   Vuelve a la pestaña principal de tu sketch.
    *   Pega el siguiente código. Este se conectará al WiFi, se suscribirá al feed de Adafruit IO y escuchará los mensajes "ON" y "OFF".

```cpp
#include <SPI.h>
#include <WiFiNINA.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "arduino_secrets.h"

// --- Configuración de Red y Adafruit IO ---
const char* WLAN_SSID = SECRET_SSID;
const char* WLAN_PASS = SECRET_PASS;
const char* AIO_SERVER = "io.adafruit.com";
const int   AIO_SERVERPORT = 1883; // Puerto para MQTT
const char* AIO_USERNAME = IO_USERNAME;
const char* AIO_KEY = IO_KEY;

// --- Configuración del LED ---
const int LED_PIN = 4;

// --- Cliente WiFi ---
WiFiClient client;

// --- Cliente MQTT de Adafruit IO ---
// Se crea una instancia del cliente MQTT que se conecta al servidor de Adafruit
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// --- Feed de Adafruit IO ---
// Se configura el feed al que nos vamos a suscribir
// El formato es: username/feeds/feed_name
Adafruit_MQTT_Subscribe luzLaboratorio = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/luz-laboratorio");

void setup() {
  Serial.begin(115200);
  while (!Serial); // Esperar a que el puerto serie se conecte

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Empezar con el LED apagado

  Serial.println("Conectando al WiFi...");
  connectWiFi();

  // Suscribirse a la función de manejo de mensajes del feed
  mqtt.subscribe(&luzLaboratorio);
}

void loop() {
  // Mantener la conexión con Adafruit IO
  MQTT_connect();

  // Escuchar por nuevos mensajes. El cliente MQTT maneja esto de forma no bloqueante.
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &luzLaboratorio) {
      // Convertir el mensaje recibido (que es un array de bytes) a un String
      String message = (char *)luzLaboratorio.lastread;
      Serial.print("Mensaje recibido del feed: ");
      Serial.println(message);

      // Actuar según el mensaje
      if (message == "ON") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED encendido.");
      } else if (message == "OFF") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED apagado.");
      }
    }
  }

  // Pequeña pausa para no saturar la red
  delay(100); 
}

// --- Función para conectar al WiFi ---
void connectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    delay(5000);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

// --- Función para conectar/reconectar a MQTT ---
void MQTT_connect() {
  int8_t ret;

  // Detener si ya está conectado
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Conectando a MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect() devuelve 0 si tiene éxito
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando conexión MQTT en 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      // Si no se puede conectar después de varios intentos, reiniciar
      while (1);
    }
  }
  Serial.println("MQTT Conectado!");
}
```

4.  **Subir el Código:**
    *   Conecta tu Arduino Nano 33 IoT.
    *   Asegúrate de seleccionar la placa correcta: `Arduino NANO 33 IoT`.
    *   Selecciona el puerto COM correcto.
    *   Haz clic en "Subir".

**Parte IV: Ejecución y Verificación**

1.  **Abrir el Monitor Serie:** Configura la velocidad a **115200 baudios**.
2.  **Observar la Conexión:** Verás los mensajes de conexión al WiFi y luego a MQTT. Si todo es correcto, el dispositivo quedará a la espera.
3.  **Probar con el Dashboard:** Ve a tu dashboard de Adafruit IO. Haz clic en el interruptor de `luz-laboratorio`. Deberías ver el mensaje correspondiente en el Monitor Serie y el LED físico cambiará de estado casi instantáneamente.
4.  **¡Prueba Final con Alexa!**
    *   Di: **"Alexa, enciende la luz del laboratorio"**.
    *   Observa el Monitor Serie. Verás el mensaje "ON" llegar desde la nube.
    *   El LED conectado a tu Arduino se encenderá.
    *   Prueba a apagarlo con el comando de voz correspondiente.

#### **5. Análisis y Preguntas de Reflexión**

1.  **Arquitectura Simplificada:** Dibuja el nuevo diagrama de flujo de datos. Compara su simplicidad con la arquitectura de la guía anterior que usaba un gateway. ¿Cuál es la principal ventaja de usar un dispositivo como el Nano 33 IoT?
2.  **Protocolo MQTT:** Esta guía utiliza MQTT (Message Queuing Telemetry Transport). Investiga brevemente y describe por qué MQTT es tan popular para aplicaciones IoT. (Pista: es ligero y eficiente).
3.  **Seguridad de las Credenciales:** El código utiliza un archivo `arduino_secrets.h`. ¿Por qué esta práctica es mejor que escribir las contraseñas directamente en el archivo principal del sketch?
4.  **Autonomía del Dispositivo:** A diferencia de la solución con gateway, este dispositivo solo necesita alimentación eléctrica (vía USB) y cobertura WiFi para funcionar. ¿Qué nuevas aplicaciones o lugares de instalación permite esta autonomía?
5.  **Fiabilidad:** ¿Qué sucede si la conexión WiFi se interrumpe temporalmente? Observa el código de la función `MQTT_connect()`. ¿Cómo intenta el programa recuperarse de una desconexión?
