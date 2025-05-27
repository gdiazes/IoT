# Instalación del Broker MQTT Mosquitto en Ubuntu

Esta guía te mostrará cómo instalar y configurar el broker MQTT Mosquitto en un servidor Ubuntu.

## Prerrequisitos

*   Un servidor Ubuntu (se recomienda una versión LTS como 20.04 o 22.04).
*   Acceso a una terminal con privilegios `sudo`.

## Paso 1: Actualizar el Sistema

Antes de instalar cualquier paquete nuevo, es una buena práctica actualizar la lista de paquetes del sistema y actualizar los paquetes existentes.

```bash
sudo apt update
sudo apt upgrade -y
```
*   `sudo apt update`: Refresca la lista de paquetes disponibles desde los repositorios configurados.
*   `sudo apt upgrade -y`: Actualiza todos los paquetes instalados a sus versiones más recientes sin eliminar ninguno; `-y` confirma automáticamente.

## Paso 2: Instalar Mosquitto y los Clientes Mosquitto

Instalaremos el broker Mosquitto y las herramientas cliente, que son útiles para probar y diagnosticar.

```bash
sudo apt install mosquitto mosquitto-clients -y
```
*   `mosquitto`: Es el paquete del broker MQTT.
*   `mosquitto-clients`: Proporciona utilidades de línea de comandos como `mosquitto_pub` (para publicar mensajes) y `mosquitto_sub` (para suscribirse a tópicos).

## Paso 3: Verificar el Estado del Servicio Mosquitto

Después de la instalación, el servicio Mosquitto debería iniciarse automáticamente.

```bash
sudo systemctl status mosquitto
```
Deberías ver una salida que indique `Active: active (running)`.
*   Si no está corriendo, puedes intentar iniciarlo manualmente:
    ```bash
    sudo systemctl start mosquitto
    ```
*   Para asegurarte de que Mosquitto se inicie automáticamente cada vez que el sistema arranque:
    ```bash
    sudo systemctl enable mosquitto
    ```

## Paso 4: Configuración Básica de Mosquitto

Por defecto, las versiones más recientes de Mosquitto pueden restringir las conexiones a `localhost` o no permitir conexiones anónimas. Crearemos un archivo de configuración para definir cómo queremos que se comporte.

1.  Crea un archivo de configuración personalizado en el directorio `conf.d`. Este enfoque es preferible a editar directamente `/etc/mosquitto/mosquitto.conf`.
    ```bash
    sudo nano /etc/mosquitto/conf.d/default.conf
    ```

2.  Pega el siguiente contenido básico en el archivo. Este ejemplo permite conexiones anónimas y hace que Mosquitto escuche en todas las interfaces de red.

    ```conf
    # Archivo: /etc/mosquitto/conf.d/default.conf

    # Permite que los clientes se conecten sin proporcionar un nombre de usuario y contraseña.
    # ¡IMPORTANTE! Para un entorno de producción, deberías establecer esto en 'false'
    # y configurar la autenticación con nombre de usuario y contraseña (ver Paso 7).
    allow_anonymous true

    # Especifica en qué interfaces de red y puerto escuchará Mosquitto.
    # '0.0.0.0' significa todas las interfaces de red IPv4 disponibles.
    # 1883 es el puerto estándar no encriptado para MQTT.
    listener 1883 0.0.0.0

    # Opcional: Para habilitar la persistencia de mensajes (QoS 1 y 2, mensajes retenidos)
    # Esto guarda los mensajes en disco para que sobrevivan a reinicios del broker.
    # persistence true
    # persistence_location /var/lib/mosquitto/

    # Opcional: Para logs más detallados en un archivo específico
    # (además de los logs del sistema gestionados por journald)
    # Asegúrate de que el directorio /var/log/mosquitto/ exista y tenga permisos para el usuario 'mosquitto'.
    # log_dest file /var/log/mosquitto/mosquitto.log
    # log_type all
    # log_timestamp true
    # log_timestamp_format %Y-%m-%dT%H:%M:%S
    ```

3.  Guarda el archivo (`Ctrl+O`, luego `Enter`) y sal del editor nano (`Ctrl+X`).

4.  Reinicia el servicio Mosquitto para aplicar los cambios de configuración:
    ```bash
    sudo systemctl restart mosquitto
    ```

## Paso 5: Probar la Conexión MQTT Localmente

Verifica que el broker esté funcionando correctamente probando la publicación y suscripción de mensajes localmente.

1.  Abre dos terminales en tu servidor Ubuntu.

2.  **En la Terminal 1 (Suscriptor):**
    Suscríbete a un tópico de prueba. El flag `-v` (verbose) muestra el tópico junto con el mensaje.
    ```bash
    mosquitto_sub -h localhost -t "test/topic" -v
    ```
    Esta terminal se quedará esperando mensajes.

3.  **En la Terminal 2 (Publicador):**
    Publica un mensaje en el mismo tópico.
    ```bash
    mosquitto_pub -h localhost -t "test/topic" -m "Hola MQTT desde Ubuntu!"
    ```

    Deberías ver el mensaje `"test/topic Hola MQTT desde Ubuntu!"` aparecer en la Terminal 1. Si es así, ¡tu broker Mosquitto está funcionando!

## Paso 6: Configurar el Firewall (si está activo)

Si tienes `ufw` (Uncomplicated Firewall) u otro firewall activo en tu servidor Ubuntu, necesitas permitir el tráfico entrante en el puerto MQTT (1883 por defecto).

1.  Verifica el estado de `ufw`:
    ```bash
    sudo ufw status
    ```

2.  Si está `active`, permite el tráfico en el puerto 1883/tcp:
    ```bash
    sudo ufw allow 1883/tcp
    ```

3.  Recarga las reglas de `ufw` (si ya estaba activo) o habilítalo si no lo estaba:
    ```bash
    sudo ufw reload
    ```
    O, si `ufw` no estaba activo y lo quieres habilitar (esto puede desconectarte si estás en SSH y no has permitido el puerto SSH previamente):
    ```bash
    # sudo ufw allow ssh  # ¡Importante si estás conectado por SSH!
    # sudo ufw enable
    ```

## Paso 7: (Recomendado para Seguridad) Configurar Usuarios y Contraseñas

Una vez que las pruebas iniciales funcionen y vayas a usar el broker en un entorno más abierto, es **altamente recomendable** deshabilitar el acceso anónimo y configurar la autenticación con nombre de usuario y contraseña.

1.  **Deshabilitar el acceso anónimo:**
    Edita tu archivo de configuración:
    ```bash
    sudo nano /etc/mosquitto/conf.d/default.conf
    ```
    Cambia `allow_anonymous true` a:
    ```conf
    allow_anonymous false
    ```
    Y añade una línea para especificar la ubicación del archivo de contraseñas:
    ```conf
    password_file /etc/mosquitto/passwd
    ```
    Guarda y cierra el archivo.

2.  **Crear un archivo de contraseñas:**
    Usa la utilidad `mosquitto_passwd`.
    *   Para el primer usuario, usa el flag `-c` para crear el archivo:
        ```bash
        sudo mosquitto_passwd -c /etc/mosquitto/passwd tu_usuario_mqtt
        ```
        Reemplaza `tu_usuario_mqtt` con el nombre de usuario que desees. Se te pedirá que ingreses y confirmes una contraseña.
    *   Para añadir usuarios adicionales después, omite el flag `-c`:
        ```bash
        # sudo mosquitto_passwd /etc/mosquitto/passwd otro_usuario_mqtt
        ```

3.  **Asegurar los permisos del archivo de contraseñas:**
    Es importante que solo el usuario `mosquitto` (con el que corre el servicio) pueda leer este archivo.
    ```bash
    sudo chown mosquitto:mosquitto /etc/mosquitto/passwd
    sudo chmod 600 /etc/mosquitto/passwd
    ```

4.  **Reiniciar Mosquitto:**
    ```bash
    sudo systemctl restart mosquitto
    ```

5.  **Probar con autenticación:**
    Ahora, tus clientes MQTT necesitarán proporcionar el nombre de usuario y la contraseña para conectarse.
    *   **Suscriptor:**
        ```bash
        mosquitto_sub -h localhost -t "test/topic" -u "tu_usuario_mqtt" -P "tu_contraseña" -v
        ```
    *   **Publicador:**
        ```bash
        mosquitto_pub -h localhost -t "test/topic" -m "Mensaje autenticado!" -u "tu_usuario_mqtt" -P "tu_contraseña"
        ```

## Paso 8: Configuración Adicional (Opcional)

Mosquitto es muy configurable. Algunas opciones avanzadas que podrías explorar incluyen:

*   **TLS/SSL:** Para encriptar el tráfico MQTT (usando el puerto 8883). Esto requiere generar certificados.
*   **WebSockets:** Para permitir conexiones MQTT sobre WebSockets (útil para aplicaciones web).
*   **Listas de Control de Acceso (ACLs):** Para definir con precisión qué usuarios pueden publicar o suscribirse a qué tópicos específicos.
*   **Bridging:** Para conectar tu broker Mosquitto a otros brokers MQTT.

Puedes encontrar más información en la documentación oficial de Mosquitto:
*   `man mosquitto.conf`
*   Página web de Mosquitto.
