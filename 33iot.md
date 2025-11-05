/*
  Proyecto: Control de LED de Arduino Nano 33 IoT desde Node-RED via MQTT
  Red Wi-Fi: APIoT / Tecsup00
  Broker MQTT: test.mosquitto.org
  Tópico de Control (Node-RED -> Arduino): tecsup/led_docdnte
  Tópico de Estado (Arduino -> Node-RED): tecsup/led_docdnte/status

  Funcionalidad:
  - Se conecta a la red Wi-Fi especificada.
  - Se conecta al broker MQTT.
  - Se suscribe al tópico de control para recibir mensajes "ON" o "OFF".
  - Enciende o apaga el LED_BUILTIN de acuerdo al mensaje recibido.
  - Publica el estado actual del LED (ON/OFF) al tópico de estado:
    - Inmediatamente después de un cambio de estado.
    - Periódicamente cada 5 segundos.
    - Al conectarse exitosamente al broker MQTT.
*/

#include <WiFiNINA.h>        // Librería para la funcionalidad Wi-Fi del Nano 33 IoT
#include <PubSubClient.h>    // Librería para el cliente MQTT (de Nick O'Leary)

// --- Configuración de la Red Wi-Fi ---
char ssid[] = "APIoT";         // Tu nombre de red Wi-Fi
char password[] = "Tecsup00"; // Tu contraseña de Wi-Fi

// --- Configuración del Broker MQTT ---
const char* mqtt_server = "test.mosquitto.org"; // Dirección del broker MQTT público
const int mqtt_port = 1883;                   // Puerto estándar de MQTT (sin SSL)
const char* mqtt_client_id = "ArduinoNanoIoT_LED_Client"; // ID único para tu cliente MQTT

// --- Tópicos MQTT ---
// Node-RED publicará comandos a este tópico, Arduino se suscribe para recibirlos.
const char* mqtt_topic_control = "tecsup/led_docdnte";
// Arduino publicará el estado del LED a este tópico, Node-RED se suscribe para mostrarlo.
const char* mqtt_topic_status = "tecsup/led_docdnte/status";

// --- Configuración del LED ---
const int ledPin = LED_BUILTIN; // El LED integrado en la placa Arduino Nano 33 IoT

// --- Objetos de Cliente ---
WiFiClient wifiClient;          // Cliente Wi-Fi para la conexión de red
PubSubClient client(wifiClient); // Cliente MQTT que usa el cliente Wi-Fi

// --- Variables de Estado y Temporización ---
long lastMsg = 0;              // Para controlar el tiempo de la última publicación periódica
bool ledState = false;         // true = LED ON, false = LED OFF (estado actual del LED)

// --- Funciones ---

// Función para inicializar y conectar a la red Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Inicia la conexión Wi-Fi

  // Espera hasta que la conexión Wi-Fi sea exitosa
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP()); // Muestra la dirección IP asignada
}

// Función callback que se ejecuta cuando se recibe un mensaje MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message); // Imprime el mensaje recibido

  // Verifica si el mensaje es para el tópico de control del LED
  if (String(topic) == mqtt_topic_control) {
    if (message == "ON") {
      digitalWrite(ledPin, HIGH); // Enciende el LED
      ledState = true;
      Serial.println("LED ON");
      client.publish(mqtt_topic_status, "ON"); // Publica el nuevo estado del LED
    } else if (message == "OFF") {
      digitalWrite(ledPin, LOW);  // Apaga el LED
      ledState = false;
      Serial.println("LED OFF");
      client.publish(mqtt_topic_status, "OFF"); // Publica el nuevo estado del LED
    }
  }
}

// Función para reconectar al broker MQTT si la conexión se pierde
void reconnect() {
  // Bucle hasta que estemos conectados
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT a ");
    Serial.print(mqtt_server);
    Serial.print("...");

    // Intentar conectar con el ID de cliente
    if (client.connect(mqtt_client_id)) {
      Serial.println("conectado");
      // Una vez conectado, suscribirse al tópico de control
      client.subscribe(mqtt_topic_control);
      Serial.print("Suscrito a: ");
      Serial.println(mqtt_topic_control);
      // Publicar el estado inicial del LED
      client.publish(mqtt_topic_status, ledState ? "ON" : "OFF");
      Serial.print("Publicado estado inicial: ");
      Serial.println(ledState ? "ON" : "OFF");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state()); // Código de error de conexión
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000); // Esperar 5 segundos antes de reintentar
    }
  }
}

// --- Setup: Se ejecuta una vez al iniciar o resetear el Arduino ---
void setup() {
  Serial.begin(9600); // Inicia la comunicación serial para depuración
  pinMode(ledPin, OUTPUT); // Configura el pin del LED como salida
  digitalWrite(ledPin, LOW); // Asegura que el LED esté apagado al inicio

  setup_wifi(); // Conecta a la red Wi-Fi
  client.setServer(mqtt_server, mqtt_port); // Configura el servidor y puerto MQTT
  client.setCallback(callback);             // Asigna la función callback para mensajes entrantes
}

// --- Loop: Se ejecuta repetidamente después del setup ---
void loop() {
  // Si no está conectado al broker MQTT, intentar reconectar
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Procesa la comunicación MQTT (envío/recepción de mensajes)

  // Publicar el estado actual del LED periódicamente (cada 5 segundos)
  long now = millis();
  if (now - lastMsg > 5000) { // Si han pasado 5 segundos desde la última publicación
    lastMsg = now;
    client.publish(mqtt_topic_status, ledState ? "ON" : "OFF"); // Publica el estado actual
    Serial.print("Publicando estado periódico del LED: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
}
