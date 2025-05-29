// Incluir las librerías necesarias
#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h> // A veces solo DHT.h es suficiente, pero esta es más completa para Adafruit_Sensor

// --- Configuración WiFi ---
char ssid[] = "APIoT";        // Tu SSID de red
char pass[] = "Tecsup00";    // Tu contraseña de red
int status = WL_IDLE_STATUS;              // Estado de la conexión WiFi

// --- Configuración MQTT ---
const char* mqtt_server = "192.168.12.246"; // IP de tu servidor Mosquitto
// Asegúrate de reemplazar "IP_DE_TU_BROKER_MQTT" con la IP real
// Por ejemplo: const char* mqtt_server = "192.168.1.100";

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

  int wifi_retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifi_retries++;
    if (wifi_retries > 20) { // Evitar bucle infinito si el WiFi no conecta
        Serial.println("\nFallo al conectar a WiFi. Reiniciando...");
        delay(1000);
        NVIC_SystemReset(); // Resetea el microcontrolador
    }
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
    // if (mqttClient.connect(clientID, mqtt_user, mqtt_password )) { // Descomenta usuario/pass si es necesario
    if (mqttClient.connect(clientID)) { // Asumiendo conexión anónima por ahora
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
  Serial.begin(115200); // Aumentar velocidad para logs más rápidos si es necesario
  // while (!Serial); // Descomentar si es necesario esperar al Monitor Serie en ciertas placas

  pinMode(LED_BUILTIN, OUTPUT); // Para feedback visual
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Inicializando sensor DHT...");
  dht.begin(); // Inicializa el sensor DHT

  setup_wifi(); // Conecta a WiFi

  mqttClient.setServer(mqtt_server, mqtt_port); // Configura el servidor MQTT
  mqttClient.setCallback(callbackMQTT);         // Configura la función de callback para mensajes MQTT

  Serial.println("Setup completado.");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { // Verifica conexión WiFi continuamente
    Serial.println("Conexión WiFi perdida. Intentando reconectar...");
    setup_wifi(); // Intenta reconectar al WiFi
    // Podrías querer resetear o manejar mejor la pérdida de WiFi aquí
  }

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

    // Verificar si las lecturas fallaron (devuelve NaN en caso de error)
    if (isnan(h) || isnan(t)) {
      Serial.println("Error al leer del sensor DHT!");
      return; // Salir de esta iteración del loop si hay error de lectura
    }

    // --- MODIFICACIÓN: Usar String() para convertir float a String ---
    String tempStringObj = String(t, 2); // Convierte 't' a String con 2 decimales
    String humStringObj = String(h, 2);  // Convierte 'h' a String con 2 decimales

    Serial.print("Temperatura: ");
    Serial.print(tempStringObj);        // Imprimir el objeto String
    Serial.print(" °C, Humedad: ");
    Serial.print(humStringObj);         // Imprimir el objeto String
    Serial.println(" %");

    // Publicar en MQTT
    // --- MODIFICACIÓN: Usar .c_str() para obtener un char* del objeto String ---
    if (mqttClient.publish(topicTemp, tempStringObj.c_str())) {
      Serial.print("Publicado en ");
      Serial.print(topicTemp);
      Serial.print(": ");
      Serial.println(tempStringObj);
      digitalWrite(LED_BUILTIN, HIGH); // Encender LED brevemente al publicar
    } else {
      Serial.println("Fallo al publicar temperatura");
    }

    if (mqttClient.publish(topicHum, humStringObj.c_str())) {
      Serial.print("Publicado en ");
      Serial.print(topicHum);
      Serial.print(": ");
      Serial.println(humStringObj);
    } else {
      Serial.println("Fallo al publicar humedad");
    }
    
    delay(100); // Pequeña pausa
    digitalWrite(LED_BUILTIN, LOW); // Apagar LED
  }
}
