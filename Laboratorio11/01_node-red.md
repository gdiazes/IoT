# Guía Completa: Instalación de Node-RED y Nodos Esenciales en Ubuntu.

Esta guía te mostrará cómo instalar Node-RED en un servidor Ubuntu (Virtual Machine), que obtiene su dirección IP mediante DHCP y opera en modo de red "Bridge". También cubrirá la instalación de un conjunto de nodos populares para extender su funcionalidad. Node-RED es una potente herramienta de programación basada en flujos visuales, construida sobre Node.js, ideal para proyectos de IoT y automatización.

## Prerrequisitos

*   Un servidor Ubuntu (se recomienda una versión LTS como 20.04 o 22.04) configurado en modo de red **Bridge**.
*   La VM debe poder obtener una dirección IP automáticamente desde el servidor DHCP de la red local.
*   Acceso a una terminal con privilegios `sudo`.
*   Conexión a internet para descargar los paquetes.

## Paso 1: Actualizar el Sistema

Asegúrate de que tu sistema esté actualizado antes de instalar nuevo software.

*   **Comando:**
    ```bash
    sudo apt update && sudo apt upgrade -y
    ```
*   **Explicación:**
    *   `sudo apt update`: Refresca la lista de paquetes disponibles.
    *   `sudo apt upgrade -y`: Actualiza los paquetes instalados.

## Paso 2: Instalar Node.js y npm (Usando el Script Recomendado por Node-RED)

Node-RED requiere Node.js y su gestor de paquetes, npm. La forma más sencilla y recomendada de instalar una versión compatible de Node.js en sistemas Linux es utilizando el script oficial proporcionado por el equipo de Node-RED.

1.  **Descargar y ejecutar el script de instalación:**
    *   **Comando:**
        ```bash
        bash <(curl -sL https://raw.githubusercontent.com/node-red/linux-installers/master/deb/update-nodejs-and-nodered)
        ```
    *   **Explicación:**
        *   Este script instala Node.js, npm, Node-RED y lo configura como servicio.

2.  **Seguir las instrucciones del script:**
    *   Responde `y` a `Are you sure you want to do this? (y/N)?`.
    *   Para un servidor Ubuntu genérico, responde `n` a `Would you like to install the Pi-specific nodes? (y/N)?`.

## Paso 3: Verificar la Instalación de Node.js y npm

1.  **Verificar Node.js:**
    *   **Comando:**
        ```bash
        node -v
        ```
2.  **Verificar npm:**
    *   **Comando:**
        ```bash
        npm -v
        ```

## Paso 4: Gestionar el Servicio Node-RED

1.  **Iniciar el servicio (si no se inició automáticamente):**
    *   **Comando:**
        ```bash
        sudo systemctl start nodered.service
        ```
2.  **Habilitar el servicio para el arranque automático:**
    *   **Comando:**
        ```bash
        sudo systemctl enable nodered.service
        ```
3.  **Verificar el estado del servicio:**
    *   **Comando:**
        ```bash
        sudo systemctl status nodered.service
        ```
    *   **Explicación:**
        *   Busca `Active: active (running)`.

## Paso 5: Identificar la Dirección IP de la VM y Acceder a Node-RED

1.  **Obtener la dirección IP de la VM:**
    *   **Comando:**
        ```bash
        hostname -I
        ```
    *   **Explicación:**
        *   Anota la dirección IP (ej. `192.168.1.150`).

2.  **Acceder al editor de Node-RED:**
    Abre un navegador web y navega a:
    `http://<IP_DE_LA_VM>:1880`

    **Consideración sobre DHCP:** Para un acceso fiable, considera una **reserva DHCP** o una **IP estática** para tu VM.

## Paso 6: Configurar el Firewall de la VM

Permite el tráfico entrante al puerto de Node-RED (1880 por defecto).

1.  **Verificar estado de `ufw`:**
    *   **Comando:**
        ```bash
        sudo ufw status
        ```
2.  **Permitir el puerto de Node-RED (si `ufw` está activo):**
    *   **Comando:**
        ```bash
        sudo ufw allow 1880/tcp
        ```
3.  **Recargar `ufw`:**
    *   **Comando (si `ufw` ya estaba activo):**
        ```bash
        sudo ufw reload
        ```

## Paso 7: (Recomendado para Seguridad) Asegurar el Editor de Node-RED

1.  **Localizar el archivo `settings.js`:**
    *   **Comando:**
        ```bash
        sudo journalctl -u nodered.service | grep "Settings file"
        ```
    *   **Explicación:**
        *   Busca la ruta al archivo `settings.js`.

2.  **Generar un hash de contraseña:**
    *   **Comando:**
        ```bash
        node-red admin hash-pw
        ```
    *   **Acción:**
        *   Introduce la contraseña deseada y copia el hash generado.

3.  **Editar `settings.js` para habilitar la autenticación:**
    *   **Comando (ejemplo, reemplaza la ruta):**
        ```bash
        sudo nano /home/nodered/.node-red/settings.js
        ```
    *   **Acción:**
        *   Descomenta y configura la sección `adminAuth:` con tu usuario y el hash de la contraseña.
            ```javascript
            adminAuth: {
                type: "credentials",
                users: [{
                    username: "tu_usuario_admin",
                    password: "PEGAR_AQUI_EL_HASH_DE_LA_CONTRASEÑA",
                    permissions: "*"
                }]
            },
            ```
        *   Guarda y cierra.

4.  **Reiniciar Node-RED:**
    *   **Comando:**
        ```bash
        sudo systemctl restart nodered.service
        ```

## Paso 8: Instalación de Nodos Adicionales

Puedes instalar nodos para extender la funcionalidad de Node-RED. Hay dos métodos principales: a través de la Paleta en la interfaz web (recomendado para facilidad) o mediante la línea de comandos `npm`.

**Método 1: Usando la "Manage Palette" en la Interfaz Web de Node-RED (Recomendado)**

1.  Abre el editor de Node-RED en tu navegador (`http://<IP_DE_LA_VM>:1880`).
2.  Haz clic en el menú de hamburguesa (☰) en la esquina superior derecha.
3.  Selecciona `Manage palette`.
4.  Ve a la pestaña `Install`.
5.  Busca cada uno de los siguientes nodos por su nombre y haz clic en el botón `install` correspondiente:
    *   `node-red-contrib-alexa-smart-home`
    *   `node-red-contrib-drawsvg`
    *   `node-red-contrib-telegram-lists`
    *   `node-red-contrib-telegrambot`
    *   `node-red-contrib-ui-led`
    *   `node-red-contrib-ui-level`
    *   `node-red-contrib-ui-svg`
    *   `node-red-node-openweathermap`
    *   `node-red-node-ping`
    *   `node-red-node-random`
    *   `node-red-dashboard` (Este es fundamental para muchos nodos `ui-*`)

    Node-RED gestionará la descarga e instalación. Usualmente no se requiere reiniciar Node-RED después de instalar nodos de esta manera, pero si alguno no aparece inmediatamente, un reinicio del servicio puede ayudar.

**Método 2: Usando `npm` en la Línea de Comandos**

Si prefieres usar la línea de comandos, o si necesitas instalar una versión específica, puedes usar `npm`. Estos comandos deben ejecutarse en el **directorio de usuario de Node-RED**.

1.  **Identificar el directorio de usuario de Node-RED:**
    Suele ser `~/.node-red/` para el usuario que ejecuta Node-RED (por ejemplo, `/home/nodered/.node-red/` si el servicio se ejecuta como usuario `nodered`, o el directorio home de tu usuario actual si ejecutas Node-RED manualmente). Si no estás seguro, puedes revisar los logs de inicio de Node-RED (ver Paso 10) que a menudo indican este directorio.

2.  **Navegar al directorio de usuario de Node-RED:**
    *   **Comando (ejemplo, ajusta la ruta si es necesario):**
        ```bash
        cd /home/nodered/.node-red/
        ```
    O si lo ejecutas como tu usuario actual:
    ```bash
    cd ~/.node-red/
    ```

3.  **Instalar los nodos uno por uno (o todos juntos):**
    Asegúrate de ejecutar estos comandos como el usuario que posee el directorio `.node-red` o usa `sudo -u <usuario_nodered> npm install ...` si es necesario por permisos. Generalmente, si estás en el directorio correcto y eres el propietario, no necesitas `sudo` para `npm install` local.

    *   **Comando para instalar `node-red-contrib-alexa-smart-home`:**
        ```bash
        npm install node-red-contrib-alexa-smart-home
        ```
    *   **Comando para instalar `node-red-contrib-drawsvg`:**
        ```bash
        npm install node-red-contrib-drawsvg
        ```
    *   **Comando para instalar `node-red-contrib-telegram-lists`:**
        ```bash
        npm install node-red-contrib-telegram-lists
        ```
    *   **Comando para instalar `node-red-contrib-telegrambot`:**
        ```bash
        npm install node-red-contrib-telegrambot
        ```
    *   **Comando para instalar `node-red-contrib-ui-led`:**
        ```bash
        npm install node-red-contrib-ui-led
        ```
    *   **Comando para instalar `node-red-contrib-ui-level`:**
        ```bash
        npm install node-red-contrib-ui-level
        ```
    *   **Comando para instalar `node-red-contrib-ui-svg`:**
        ```bash
        npm install node-red-contrib-ui-svg
        ```
    *   **Comando para instalar `node-red-node-openweathermap`:**
        ```bash
        npm install node-red-node-openweathermap
        ```
    *   **Comando para instalar `node-red-node-ping`:**
        ```bash
        npm install node-red-node-ping
        ```
    *   **Comando para instalar `node-red-node-random`:**
        ```bash
        npm install node-red-node-random
        ```
    *   **Comando para instalar `node-red-dashboard`:**
        ```bash
        npm install node-red-dashboard
        ```

    *   **Explicación:**
        *   `npm install <nombre-del-paquete>`: Descarga e instala el paquete de nodo especificado y sus dependencias en el directorio `node_modules` local (dentro de `~/.node-red/`).

4.  **Reiniciar Node-RED después de instalar nodos con `npm`:**
    Después de instalar nodos usando `npm` desde la línea de comandos, siempre debes reiniciar el servicio Node-RED para que cargue los nuevos nodos.
    *   **Comando:**
        ```bash
        sudo systemctl restart nodered.service
        ```

    *   **Nota sobre dependencias de compilación:** Algunos nodos (como `node-red-node-ping` o nodos que interactúan con hardware) pueden requerir herramientas de compilación (`build-essential`, `python`, etc.) instaladas en tu sistema. Si la instalación de un nodo falla con errores de compilación, asegúrate de tener estas herramientas:
        ```bash
        # sudo apt install -y build-essential git python3
        ```

## Paso 9: Verificar los Nodos Instalados

Después de instalar los nodos (por cualquiera de los métodos) y reiniciar Node-RED (si usaste `npm`), abre el editor de Node-RED en tu navegador. Los nuevos nodos deberían aparecer en la paleta de la izquierda, listos para ser usados en tus flujos.

## Paso 10: Ver los Logs de Node-RED (para Diagnóstico)

Si tienes problemas con la instalación de nodos o con Node-RED en general, los logs son esenciales.

*   **Comando (logs en tiempo real, formato simple):**
    ```bash
    sudo journalctl -f -u nodered.service -o cat
    ```
*   **Comando (últimos logs, formato simple):**
    ```bash
    sudo journalctl -u nodered.service -n 100 -o cat --no-pager
    ```
