#include <WiFiNINA.h>         // Biblioteca para el manejo del WiFi
#include <PubSubClient.h>     // Biblioteca para manejar MQTT

// Variables para la conexión WiFi
const char* ssid = "APIoT";       // Cambia esto por el nombre de tu red WiFi (SSID)
const char* password = "Tecsup00"; // Cambia esto por la contraseña de tu red WiFi

// Configuración del broker MQTT
const char* mqttServer = "192.168.12.100"; // Cambia esto por la IP del broker MQTT
const int mqttPort = 1883;            // Puerto del broker MQTT (por defecto es 1883)
// const char* mqttUser = "usuario";     // Ya no se necesita para conexión anónima
// const char* mqttPassword = "password"; // Ya no se necesita para conexión anónima

// Variables para la conexión WiFi y MQTT
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Configuración del LED BUILTIN
const int ledPin = LED_BUILTIN; // Pin del LED incorporado

// Función de callback para manejar mensajes MQTT entrantes
void callback(char* topic, byte* payload, unsigned int length) {
  // Convierte el payload en un string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Mensaje recibido ["); // Añadido para depuración
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);


  // Verifica si el mensaje es para controlar el LED_BUILTIN
  if (String(topic) == "inputled") {
    if (message == "ON") {
      digitalWrite(ledPin, HIGH);  // Enciende el LED
      Serial.println("inputled encendido desde MQTT con un ON");
    } else if (message == "OFF") {
      digitalWrite(ledPin, LOW);   // Apaga el LED
      Serial.println("inputled apagado desde MQTT con un OFF");
    }
  }
}

// ----------- FUNCIÓN MODIFICADA -----------
// Función para conectarse al broker MQTT (sin usuario/contraseña) y suscribirse al tópico `inputled`
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Conectando al broker MQTT (anónimamente)...");
    
    // Intenta conectar solo con el ClientID.
    // Asegúrate de que "ArduinoNano33" sea único para este cliente en el broker.
    // Esto funcionará si el broker permite conexiones anónimas.
    if (mqttClient.connect("ArduinoNano33")) { // <--- SOLO SE PASA EL clientID
      Serial.println("Conectado al broker MQTT.");
      
      // Suscribirse al tópico `inputled` después de conectar exitosamente
      if (mqttClient.subscribe("inputled")) {
        Serial.println("Suscrito al tópico 'inputled'.");
      } else {
        Serial.println("Error al suscribirse al tópico 'inputled'.");
      }
    } else {
      Serial.print("Error de conexión MQTT, rc="); // rc = return code
      Serial.print(mqttClient.state()); // Imprime el código de error de PubSubClient
      Serial.println(" Intentando de nuevo en 2 segundos...");
      // Espera 2 segundos antes de reintentar
      delay(2000);
    }
  }
}
// ----------- FIN DE LA FUNCIÓN MODIFICADA -----------


// Función de configuración
void setup() {
  // Inicia la comunicación serial
  Serial.begin(9600);
  // while (!Serial); // Puedes descomentar esto si necesitas esperar a que se abra el monitor serial

  // Configura el LED BUILTIN como salida
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Asegurarse que el LED empieza apagado

  // Conexión a WiFi
  Serial.print("Conectando a la red WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado a la red WiFi.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configura el cliente MQTT y la función de callback
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback); // Asigna la función de callback para manejar mensajes
  
  // El primer intento de conexión se hace aquí
  // connectMQTT(); // Se llamará desde el loop si no está conectado
}

// Función principal de ejecución
void loop() {
  // Mantiene la conexión MQTT activa e intenta reconectar si es necesario
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop(); // Es crucial llamar a loop() para procesar MQTT

  // --- Lógica de publicación movida a un temporizador para no bloquear ---
  // (Opcional pero recomendado) En lugar de un delay grande, 
  // usa millis() para publicar periódicamente sin detener otras tareas.
  static unsigned long lastPublishTime = 0;
  const long publishInterval = 5000; // Publicar cada 5 segundos (5000 ms)

  if (millis() - lastPublishTime >= publishInterval) {
    lastPublishTime = millis(); // Actualiza el tiempo de la última publicación

    // Lee el estado del LED_BUILTIN y lo envía por MQTT
    int ledState = digitalRead(ledPin);
    if (ledState == HIGH) {
      Serial.println("Publicando estado LED: ON");
      mqttClient.publish("LED_BUILTIN", "ON"); // Publica "ON" si el LED está encendido
    } else {
      Serial.println("Publicando estado LED: OFF");
      mqttClient.publish("LED_BUILTIN", "OFF"); // Publica "OFF" si el LED está apagado
    }
  }
  // Ya no se necesita el delay(5000); aquí si usas millis()
}
