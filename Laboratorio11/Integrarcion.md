# Guía Final: Integrar Node-RED con Arduino Nano (Sensores y Actuador LCD) vía MQTT

Esta guía te mostrará cómo usar Node-RED para:
1.  Suscribirte a los tópicos MQTT donde el primer Arduino (sensor) publica datos de temperatura y humedad.
2.  Mostrar estos datos en un dashboard de Node-RED.
3.  Crear una entrada en Node-RED para enviar un mensaje de texto personalizado al segundo Arduino (actuador LCD) a través de un nuevo tópico MQTT, que se mostrará en la LCD.

## Prerrequisitos

1.  **Dos Arduino Nano 33 IoT configurados:**
    *   **Arduino #1 (Sensor):** Publicando temperatura y humedad a `tecsup/temperatura` y `tecsup/humedad` (como en la guía anterior).
    *   **Arduino #2 (Actuador LCD):** Suscrito a `tecsup/temperatura` y `tecsup/humedad`, mostrando datos en una LCD I2C (como en la guía anterior). **Modificaremos este Arduino** para que también se suscriba a un nuevo tópico para mensajes personalizados.
2.  **Broker MQTT:** Configurado y funcionando (por ejemplo, Mosquitto en tu servidor Ubuntu).
3.  **Node-RED:** Instalado y funcionando en tu servidor Ubuntu (o donde lo tengas).
4.  **Nodos de Node-RED necesarios instalados:**
    *   `node-red-dashboard` (para la interfaz de usuario).
    *   Los nodos MQTT In y MQTT Out (vienen por defecto con Node-RED).

## Paso 1: Modificar el Código del Arduino #2 (Actuador LCD)

Necesitamos que el Arduino que controla la LCD se suscriba a un nuevo tópico para recibir mensajes personalizados desde Node-RED. Lo llamaremos `tecsup/lcd/mensaje`.

1.  Abre el sketch del **Arduino #2 (el que tiene la LCD)** en el IDE de Arduino.
2.  **Añade el nuevo tópico:**
    ```cpp
    // ... (otras configuraciones MQTT) ...
    const char* topicLcdMsg = "tecsup/lcd/mensaje"; // Nuevo tópico para mensajes a la LCD
    ```

3.  **Modifica la función `reconnectMQTT()` para suscribirse al nuevo tópico:**
    ```cpp
    void reconnectMQTT() {
      while (!mqttClient.connected()) {
        // ... (código de conexión existente) ...
        if (mqttClient.connect(clientID /*, mqtt_user, mqtt_password*/ )) {
          // ... (código de conexión exitosa existente) ...

          // Suscribirse a los tópicos existentes
          if(mqttClient.subscribe(topicTemp)){ /* ... */ }
          if(mqttClient.subscribe(topicHum)){ /* ... */ }

          // *** NUEVA SUSCRIPCIÓN ***
          if(mqttClient.subscribe(topicLcdMsg)){
            Serial.print("Suscrito a: ");
            Serial.println(topicLcdMsg);
            lcd.setCursor(0,0); // Mostrar mensaje temporal
            lcd.print("Sub: LCD Msg OK");
            delay(500);
          } else {
            Serial.println("Fallo al suscribir a mensaje LCD");
            lcd.setCursor(0,0);
            lcd.print("Sub: LCD Msg Fail");
            delay(500);
          }
        } else {
          // ... (código de fallo de conexión existente) ...
        }
      }
    }
    ```

4.  **Modifica la función `callbackMQTT()` para manejar el nuevo tópico:**
    ```cpp
    // Variable para almacenar el mensaje personalizado
    String mensajePersonalizado = "";
    bool newLcdCustomMsg = false;

    void callbackMQTT(char* topic, byte* payload, unsigned int length) {
      Serial.print("Mensaje recibido [");
      Serial.print(topic);
      Serial.print("]: ");
      String message;
      for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
      }
      Serial.println(message);

      if (String(topic) == topicTemp) {
        temperaturaRecibida = message + " C";
        newDataTemp = true;
      } else if (String(topic) == topicHum) {
        humedadRecibida = message + " %";
        newDataHum = true;
      } else if (String(topic) == topicLcdMsg) { // *** NUEVO MANEJO DE TÓPICO ***
        mensajePersonalizado = message;
        newLcdCustomMsg = true; // Flag para indicar que hay un nuevo mensaje personalizado
        // Sobrescribimos temporalmente los datos del sensor si llega un mensaje personalizado
        // para que la función displayDataOnLCD() lo muestre.
        // O podemos tener una lógica de visualización separada/priorizada.
      }
    }
    ```

5.  **Modifica la función `displayDataOnLCD()` para mostrar el mensaje personalizado:**
    Podemos hacer que el mensaje personalizado tenga prioridad o se muestre de forma alterna. Para simplificar, si llega un mensaje personalizado, lo mostraremos durante un tiempo o hasta que llegue nueva data de sensores.
    Aquí una forma simple: si hay un mensaje personalizado, se muestra. Si no, se muestran los datos del sensor.

    ```cpp
    void displayDataOnLCD() {
      if (newLcdCustomMsg) { // Prioridad al mensaje personalizado
        lcd.clear();
        lcd.setCursor(0, 0);
        // Dividir mensaje largo en dos líneas si es necesario
        if (mensajePersonalizado.length() > LCD_COLS) {
          lcd.print(mensajePersonalizado.substring(0, LCD_COLS));
          if (LCD_ROWS > 1) {
            lcd.setCursor(0, 1);
            lcd.print(mensajePersonalizado.substring(LCD_COLS));
          }
        } else {
          lcd.print(mensajePersonalizado);
        }
        // Serial.println("Mostrando mensaje personalizado en LCD.");
        // Mantenemos newLcdCustomMsg = true hasta que llegue nueva data de sensores o se borre de otra forma.
        // O podríamos resetearlo después de un tiempo. Para este ejemplo, se queda hasta que
        // newDataTemp o newDataHum se activen.
        // Para resetearlo después de mostrarlo una vez:
        // newLcdCustomMsg = false;

      } else if (newDataTemp || newDataHum) { // Si no hay mensaje personalizado, muestra datos del sensor
        lcd.clear();
        
        lcd.setCursor(0, 0); // Primera línea
        lcd.print("Temp: ");
        lcd.print(temperaturaRecibida);

        lcd.setCursor(0, 1); // Segunda línea
        lcd.print("Hum:  ");
        lcd.print(humedadRecibida);

        newDataTemp = false; 
        newDataHum = false;
        // Cuando se actualizan los datos del sensor, borramos cualquier mensaje personalizado anterior
        mensajePersonalizado = ""; // Limpiar mensaje personalizado para que no se muestre de nuevo
                                   // hasta que llegue uno nuevo.
        newLcdCustomMsg = false; 
      }
    }
    ```
    *   **Nota sobre la lógica de visualización:** La lógica anterior prioriza el mensaje personalizado. Si llega un nuevo dato de sensor, se borra el mensaje personalizado y se muestran los datos del sensor. Puedes ajustar esta lógica como prefieras (por ejemplo, que el mensaje personalizado se muestre durante X segundos y luego vuelvan los datos del sensor, o que alterne).

6.  **Sube el código modificado al Arduino #2.**

## Paso 2: Configurar Node-RED para Recibir Datos de Sensores y Mostrar en Dashboard

1.  Abre Node-RED en tu navegador (ej. `http://<IP_SERVIDOR_NODERED>:1880`).
2.  **Añadir Nodos MQTT In:**
    *   Arrastra dos nodos `mqtt in` a tu flujo.
    *   **Configura el primer `mqtt in` para Temperatura:**
        *   Doble clic para editarlo.
        *   **Servidor:** Haz clic en el lápiz para añadir/editar tu broker MQTT.
            *   **Nombre (opcional):** `MiBrokerMQTT`
            *   **Servidor:** `IP_DE_TU_BROKER_MQTT` (la misma que usaste en los Arduinos).
            *   **Puerto:** `1883` (o el que uses).
            *   Si tu broker tiene autenticación, ve a la pestaña `Seguridad` y añade tu `Usuario` y `Contraseña`.
            *   Haz clic en `Añadir` o `Actualizar`.
        *   **Tópico:** `tecsup/temperatura`
        *   **QoS:** `0` (o `1` o `2` según tu configuración).
        *   **Salida:** `una cadena String`.
        *   **Nombre (opcional):** `Temperatura In`
        *   Haz clic en `Hecho`.
    *   **Configura el segundo `mqtt in` para Humedad:**
        *   **Servidor:** Selecciona el mismo broker que configuraste antes.
        *   **Tópico:** `tecsup/humedad`
        *   **QoS:** `0`
        *   **Salida:** `una cadena String`.
        *   **Nombre (opcional):** `Humedad In`
        *   Haz clic en `Hecho`.

3.  **Añadir Nodos de Dashboard:**
    *   Asegúrate de tener `node-red-dashboard` instalado (Menú -> Manage Palette -> Install -> busca `node-red-dashboard`).
    *   Arrastra un nodo `gauge` (indicador) al flujo para la temperatura.
    *   Arrastra otro nodo `gauge` al flujo para la humedad.
    *   (Opcional) Arrastra un nodo `chart` (gráfica) para la temperatura y otro para la humedad si quieres ver historiales.

4.  **Configurar el Gauge de Temperatura:**
    *   Doble clic en el nodo `gauge`.
    *   **Grupo:** Haz clic en el lápiz.
        *   **Nombre (de la pestaña del dashboard):** `Sensores Tecsup` (o lo que quieras).
        *   Haz clic en `Añadir`. Selecciona esta nueva pestaña.
    *   **Tamaño:** Auto (o ajústalo).
    *   **Tipo:** Gauge (o puedes probar Donut, Compass, Level).
    *   **Etiqueta:** `Temperatura`
    *   **Unidades:** `°C`
    *   **Rango:** Mín `0`, Máx `50` (ajusta según tus lecturas esperadas).
    *   **Nombre (opcional):** `Gauge Temperatura`
    *   Haz clic en `Hecho`.

5.  **Configurar el Gauge de Humedad:**
    *   Doble clic en el nodo `gauge`.
    *   **Grupo:** Selecciona el mismo grupo `[Sensores Tecsup]` creado antes.
    *   **Etiqueta:** `Humedad`
    *   **Unidades:** `%`
    *   **Rango:** Mín `0`, Máx `100`.
    *   **Nombre (opcional):** `Gauge Humedad`
    *   Haz clic en `Hecho`.

6.  **Conectar los Nodos:**
    *   Conecta la salida del nodo `Temperatura In` a la entrada del nodo `Gauge Temperatura`.
    *   Conecta la salida del nodo `Humedad In` a la entrada del nodo `Gauge Humedad`.

7.  **Desplegar (Deploy):**
    *   Haz clic en el botón rojo `Deploy` en la esquina superior derecha de Node-RED.

8.  **Ver el Dashboard:**
    *   Abre una nueva pestaña en tu navegador y ve a la URL del dashboard. Usualmente es:
        `http://<IP_SERVIDOR_NODERED>:1880/ui`
    *   Deberías ver los indicadores de temperatura y humedad actualizándose con los datos enviados por el Arduino #1.

## Paso 3: Configurar Node-RED para Enviar Mensajes Personalizados a la LCD

1.  **Añadir Nodos de Entrada y MQTT Out:**
    *   Arrastra un nodo `inject` al flujo. Lo usaremos para enviar un mensaje de prueba.
    *   Arrastra un nodo `text input` (del dashboard) al flujo. Esto permitirá escribir el mensaje desde el dashboard.
    *   Arrastra un nodo `mqtt out` al flujo.

2.  **Configurar el Nodo `text input`:**
    *   Doble clic para editarlo.
    *   **Grupo:** Selecciona el mismo grupo `[Sensores Tecsup]` o crea uno nuevo.
    *   **Etiqueta:** `Mensaje para LCD`
    *   **Modo de Envío de Mensaje:** `al pulsar Intro` o `tras un retardo fijo`.
    *   **Nombre (opcional):** `Input LCD Msg`
    *   Haz clic en `Hecho`.

3.  **Configurar el Nodo `mqtt out`:**
    *   Doble clic para editarlo.
    *   **Servidor:** Selecciona el mismo broker MQTT que usaste antes.
    *   **Tópico:** `tecsup/lcd/mensaje` (el nuevo tópico al que se suscribió el Arduino #2).
    *   **QoS:** `0`
    *   **Retener:** `false` (generalmente no queremos que los mensajes a la LCD se retengan).
    *   **Nombre (opcional):** `Publicar a LCD`
    *   Haz clic en `Hecho`.

4.  **Conectar los Nodos:**
    *   Conecta la salida del nodo `text input` a la entrada del nodo `Publicar a LCD`.

5.  **(Opcional) Configurar el Nodo `inject` para Pruebas:**
    *   Doble clic en el nodo `inject`.
    *   **Payload:** selecciona `string` (cadena) y escribe un mensaje de prueba, por ejemplo, `Hola Arduino!`.
    *   **Tópico (opcional aquí, ya que el nodo MQTT Out lo define):** puedes dejarlo en blanco o poner `tecsup/lcd/mensaje`.
    *   **Nombre (opcional):** `Test LCD Msg`
    *   Haz clic en `Hecho`.
    *   Conecta la salida del nodo `Test LCD Msg` también a la entrada del nodo `Publicar a LCD`.

6.  **Desplegar (Deploy):**
    *   Haz clic en el botón `Deploy`.

## Paso 4: Probar Todo el Sistema

1.  **Asegúrate de que ambos Arduinos estén encendidos y conectados a WiFi y MQTT.**
    *   El Arduino #1 (sensor) debe estar publicando datos.
    *   El Arduino #2 (LCD) debe estar suscrito y mostrando temperatura/humedad.
2.  **Verifica el Dashboard de Node-RED:**
    *   Los indicadores de temperatura y humedad deben estar mostrando los valores actuales.
3.  **Enviar un Mensaje Personalizado desde Node-RED a la LCD:**
    *   Ve al dashboard de Node-RED (`http://<IP_SERVIDOR_NODERED>:1880/ui`).
    *   Busca el campo de texto "Mensaje para LCD".
    *   Escribe un mensaje (ej. "Node-RED Saluda") y presiona Intro (o espera el retardo si así lo configuraste).
    *   **Observa la pantalla LCD del Arduino #2.** Debería limpiarse y mostrar el mensaje que enviaste desde Node-RED. Si el mensaje es más largo que las columnas de la LCD, debería mostrarse en dos líneas (según la lógica del código Arduino).
    *   (Opcional) Haz clic en el botón del nodo `inject` ("Test LCD Msg") en el editor de Node-RED para enviar el mensaje de prueba.
4.  **Verificar el Comportamiento:**
    *   Después de que el Arduino #2 muestre el mensaje personalizado, cuando el Arduino #1 envíe nuevos datos de temperatura/humedad, la pantalla LCD del Arduino #2 debería volver a mostrar esos datos (según la lógica implementada).
