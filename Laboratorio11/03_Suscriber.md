# Guía: Suscribirse a MQTT con Arduino Nano 33 IoT y Mostrar Datos en LCD I2C

Esta guía te mostrará cómo configurar un segundo Arduino Nano 33 IoT para conectarse a WiFi, suscribirse a tópicos MQTT donde se publican datos de temperatura y humedad, y mostrar estos valores en una pantalla LCD I2C (como una 16x2 o 20x4).

## Prerrequisitos Hardware

1.  **Arduino Nano 33 IoT (Segundo dispositivo):**
2.  **Pantalla LCD I2C:** Comúnmente una LCD 16x2 (16 caracteres, 2 líneas) o 20x4 con un módulo adaptador I2C ya soldado en la parte posterior. Estos módulos suelen tener 4 pines: GND, VCC, SDA, SCL.
3.  **Cables de conexión (jumpers).**
4.  **Cable Micro USB** para programar el Arduino.
5.  **Acceso a la misma red WiFi** que el primer Arduino y el broker MQTT.
6.  **Broker MQTT configurado y accesible** (el mismo que usaste antes).

## Prerrequisitos Software

1.  **IDE de Arduino:** Instalado y configurado.
2.  **Placa "Arduino SAMD Boards (32-bits ARM Cortex-M0+)" instalada en el Gestor de Placas.**
3.  **Librerías de Arduino necesarias:**
    *   `WiFiNINA`: Para la conectividad WiFi.
    *   `PubSubClient` de Nick O'Leary: Para la comunicación MQTT.
    *   `LiquidCrystal_I2C` de Frank de Brabander (o una similar compatible con tu LCD I2C). Esta es una librería muy popular y fácil de usar.

## Paso 1: Instalar las Librerías de Arduino

1.  Abre el IDE de Arduino.
2.  Ve a `Herramientas` -> `Gestionar Librerías...`.
3.  Busca e instala las siguientes librerías:
    *   **`WiFiNINA`** (probablemente ya instalada).
    *   **`PubSubClient`** (de Nick O'Leary).
    *   **`LiquidCrystal_I2C`** (Busca "LiquidCrystal I2C". La de Frank de Brabander es una buena opción. Hay varias, la mayoría funcionan de manera similar).

## Paso 2: Conexión del Hardware (Pantalla LCD I2C al Arduino)

Las pantallas LCD con módulo I2C son muy fáciles de conectar, ya que solo requieren 4 cables.

*   **GND (de la LCD):** Conectar a `GND` en el Arduino Nano 33 IoT.
*   **VCC (de la LCD):** Conectar a `5V` en el Arduino Nano 33 IoT. (Aunque el Nano 33 IoT es de 3.3V para sus lógicas, el pin 5V puede alimentar periféricos de 5V como la mayoría de las LCDs I2C. Verifica la hoja de datos de tu LCD si tienes dudas, pero 5V es lo más común).
*   **SDA (de la LCD):** Conectar al pin `SDA` del Arduino (Pin A4 en el Nano 33 IoT).
*   **SCL (de la LCD):** Conectar al pin `SCL` del Arduino (Pin A5 en el Nano 33 IoT).

**Diagrama de conexión simplificado:**

```
Arduino Nano 33 IoT        LCD I2C Module
--------------------        --------------
      GND   ----------------  GND
      5V    ----------------  VCC
      A4 (SDA) -------------  SDA
      A5 (SCL) -------------  SCL
```

## Paso 3: Encontrar la Dirección I2C de tu Pantalla LCD

No todas las pantallas LCD I2C tienen la misma dirección. Necesitas encontrarla para que la librería pueda comunicarse con ella.

1.  Conecta tu Arduino Nano 33 IoT con la LCD I2C a tu computadora.
2.  Abre el IDE de Arduino.
3.  Ve a `Archivo` -> `Ejemplos` -> `Wire` -> `i2c_scanner`.
4.  Sube este sketch a tu Arduino.
5.  Abre el Monitor Serie (`Herramientas` -> `Monitor Serie`) con la velocidad de 9600 baudios.
6.  El escáner mostrará las direcciones I2C de los dispositivos conectados. Deberías ver algo como:
    `I2C device found at address 0x27`
    o
    `I2C device found at address 0x3F`
    Anota esta dirección hexadecimal (por ejemplo, `0x27`). Esta es la que usarás en el código principal.

## Paso 4: Código del Arduino

Copia y pega el siguiente código en un nuevo sketch del IDE de Arduino.

**Modifica las siguientes constantes en el código según tu configuración:**

*   `ssid`: Nombre de tu red WiFi.
*   `pass`: Contraseña de tu red WiFi.
*   `mqtt_server`: Dirección IP de tu broker MQTT.
*   `mqtt_user` y `mqtt_password`: Si tu broker requiere autenticación.
*   `LCD_ADDR`: La dirección I2C de tu LCD que encontraste en el Paso 3 (ej. `0x27`).
*   `LCD_COLS`: Número de columnas de tu LCD (ej. `16` para una 16x2).
*   `LCD_ROWS`: Número de filas de tu LCD (ej. `2` para una 16x2).

```cpp
// Incluir las librerías necesarias
#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Para la LCD I2C

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
const char* clientID = "arduinoNano33IoT_LCD";   // ID de cliente MQTT único para este Arduino

// Tópicos MQTT a los que nos suscribiremos
const char* topicTemp = "tecsup/temperatura";
const char* topicHum = "tecsup/humedad";

// --- Configuración LCD I2C ---
const int LCD_COLS = 16; // Número de columnas de tu LCD (ej. 16 o 20)
const int LCD_ROWS = 2;  // Número de filas de tu LCD (ej. 2 o 4)
const uint8_t LCD_ADDR = 0x27; // Dirección I2C de tu LCD (ej. 0x27 o 0x3F)
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// --- Variables Globales ---
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Variables para almacenar los últimos valores recibidos
String temperaturaRecibida = "--.- C";
String humedadRecibida = "--.- %";
bool newDataTemp = false;
bool newDataHum = false;

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
    lcd.setCursor(0,0);
    lcd.print("WiFi Conn...");
    lcd.setCursor(0,1);
    lcd.print("            "); // Limpiar línea
    for(int i=0; i<WiFi.status(); i++) lcd.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi OK!");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(2000);
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Actualizar las variables con los nuevos datos
  if (String(topic) == topicTemp) {
    temperaturaRecibida = message + " C";
    newDataTemp = true;
  } else if (String(topic) == topicHum) {
    humedadRecibida = message + " %";
    newDataHum = true;
  }

  // No actualizamos la LCD aquí directamente para evitar parpadeos,
  // se hará en el loop principal cuando haya nuevos datos.
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conexión MQTT...");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MQTT Conn...");

    // Si tu broker no requiere usuario/contraseña:
    // if (mqttClient.connect(clientID)) {
    // Si tu broker SÍ requiere usuario/contraseña:
    if (mqttClient.connect(clientID /*, mqtt_user, mqtt_password*/ )) { // Descomenta usuario/pass si es necesario
      Serial.println("conectado!");
      lcd.setCursor(0,1);
      lcd.print("Conectado!");
      delay(1000);

      // Suscribirse a los tópicos
      if(mqttClient.subscribe(topicTemp)){
        Serial.print("Suscrito a: ");
        Serial.println(topicTemp);
      } else {
        Serial.println("Fallo al suscribir a temperatura");
      }
      if(mqttClient.subscribe(topicHum)){
        Serial.print("Suscrito a: ");
        Serial.println(topicHum);
      } else {
        Serial.println("Fallo al suscribir a humedad");
      }
    } else {
      Serial.print("falló, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      lcd.setCursor(0,1);
      lcd.print("Fallo rc=");
      lcd.print(mqttClient.state());
      delay(5000);
    }
  }
}

void displayDataOnLCD() {
  if (newDataTemp || newDataHum) { // Solo actualiza si hay nuevos datos
    lcd.clear();
    
    lcd.setCursor(0, 0); // Primera línea
    lcd.print("Temp: ");
    lcd.print(temperaturaRecibida);

    lcd.setCursor(0, 1); // Segunda línea
    lcd.print("Hum:  ");
    lcd.print(humedadRecibida);

    newDataTemp = false; // Resetea los flags
    newDataHum = false;
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inicializar la LCD
  lcd.init();       // Inicializa la LCD (algunas librerías usan lcd.begin())
  lcd.backlight();  // Enciende la luz de fondo
  lcd.setCursor(0,0);
  lcd.print("Iniciando...");

  setup_wifi(); // Conecta a WiFi

  mqttClient.setServer(mqtt_server, mqtt_port); // Configura el servidor MQTT
  mqttClient.setCallback(callbackMQTT);         // Configura la función de callback para mensajes MQTT

  Serial.println("Setup completado.");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Setup OK");
  delay(1000);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT(); // Si no está conectado a MQTT, intenta reconectar
  }
  mqttClient.loop(); // Esencial para mantener la conexión MQTT y procesar mensajes

  displayDataOnLCD(); // Actualiza la pantalla LCD si hay nuevos datos

  // Pequeña pausa para no sobrecargar el loop si no hay nada más que hacer
  delay(100);
}
```

**Explicación del Código:**

1.  **Includes:** Se añade `Wire.h` y `LiquidCrystal_I2C.h`.
2.  **Configuración LCD I2C:** Define las columnas, filas y la dirección I2C de tu LCD. Se crea un objeto `lcd`.
3.  **Variables Globales:** Se añaden `temperaturaRecibida`, `humedadRecibida` para almacenar los últimos valores, y `newDataTemp`, `newDataHum` como flags para saber cuándo actualizar la LCD.
4.  **`setup_wifi()`:** Se añaden mensajes a la LCD durante el proceso de conexión WiFi.
5.  **`callbackMQTT()`:**
    *   Esta función es ahora crucial. Se llama cada vez que llega un mensaje a un tópico al que este Arduino está suscrito.
    *   Convierte el payload (datos del mensaje) a un `String`.
    *   Verifica a qué tópico pertenece el mensaje (`tecsup/temperatura` o `tecsup/humedad`).
    *   Actualiza las variables `temperaturaRecibida` o `humedadRecibida` correspondientes.
    *   Establece el flag `newDataTemp` o `newDataHum` a `true`.
6.  **`reconnectMQTT()`:**
    *   Se añaden mensajes a la LCD durante el proceso de conexión MQTT.
    *   **Importante:** Después de conectar, se suscribe a los tópicos `topicTemp` y `topicHum` usando `mqttClient.subscribe()`.
7.  **`displayDataOnLCD()`:**
    *   Nueva función que se llama en el `loop()`.
    *   Solo actualiza el contenido de la LCD si los flags `newDataTemp` o `newDataHum` son `true`.
    *   Limpia la pantalla y muestra los valores de temperatura y humedad formateados.
    *   Resetea los flags a `false` después de actualizar.
8.  **`setup()`:**
    *   Se inicializa la LCD con `lcd.init()` y `lcd.backlight()`.
    *   Se muestran mensajes de inicio en la LCD.
9.  **`loop()`:**
    *   Mantiene la conexión MQTT y procesa mensajes entrantes con `mqttClient.loop()`.
    *   Llama a `displayDataOnLCD()` para actualizar la pantalla.

## Paso 5: Subir el Código al Segundo Arduino

1.  Conecta tu segundo Arduino Nano 33 IoT (el que tiene la LCD) a tu computadora.
2.  En el IDE de Arduino, asegúrate de que la placa "Arduino Nano 33 IoT" y el puerto COM correcto estén seleccionados.
3.  Haz clic en el botón "Subir".

## Paso 6: Probar y Verificar

1.  **Arduino Publicador (el del DHT11):** Asegúrate de que el primer Arduino (el que lee el DHT11 y publica los datos) esté funcionando y enviando datos a los tópicos `tecsup/temperatura` y `tecsup/humedad`.
2.  **Arduino Suscriptor (el de la LCD):**
    *   Observa el Monitor Serie (Herramientas -> Monitor Serie, 9600 baudios) de este segundo Arduino. Verás los mensajes de conexión WiFi, MQTT y los mensajes recibidos.
    *   **Observa la Pantalla LCD:** Debería mostrar "Iniciando...", luego el estado de conexión WiFi y MQTT. Una vez conectado y suscrito, cuando el primer Arduino publique nuevos datos, la pantalla LCD se actualizará mostrando la temperatura y la humedad.

**Ejemplo de lo que verías en la LCD:**
```
Temp: 25.50 C
Hum:  60.20 %
```
Y estos valores se actualizarán cada vez que el primer Arduino envíe nuevos datos.
