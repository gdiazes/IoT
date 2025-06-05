¡Excelente punto! Eso fomenta una documentación continua y asegura que los entregables no se dejen para el último minuto. Aquí está la versión actualizada, enfatizando la creación progresiva de un documento de laboratorio:

---

**Laboratorio Calificado: Sistema de Automatización Doméstica con Arduino, Node-RED y MQTT**

**1. Presentación del Caso**

El objetivo es implementar un sistema de automatización doméstica simple utilizando un **Arduino Nano IoT**, donde controlaremos tres LEDs que representan las luces de tres ambientes distintos: sala, comedor y dormitorio. La activación de cada LED se realizará a través de **controles (por ejemplo, botones o interruptores) en un dashboard de Node-RED**, donde el usuario seleccionará el LED que desea encender o apagar. Se utiliza MQTT para la comunicación entre el Arduino y **Node-RED**, que mostrará el dashboard en tiempo real con el estado de cada LED y permitirá su control directo.

**2. Objetivo del Laboratorio**

*   Implementar un sistema funcional para controlar tres LEDs (representando luces de sala, comedor y dormitorio) utilizando un Arduino Nano IoT.
*   Utilizar el protocolo MQTT como medio de comunicación entre el Arduino y Node-RED.
*   Desarrollar un dashboard en Node-RED que permita al usuario:
    *   Controlar (encender/apagar) cada uno de los tres LEDs individualmente.
    *   Visualizar el estado actual (encendido/apagado) de cada uno de los tres LEDs en tiempo real.
*   Demostrar el trabajo colaborativo y la comprensión de los conceptos involucrados a través de un documento de laboratorio progresivo.

**3. Logística del Laboratorio**

*   **Tipo:** Evaluación práctica en grupo de dos estudiantes.
*   **Duración Total:** 100 minutos.

**4. Entregable Principal**

Al finalizar el tiempo asignado, cada grupo deberá entregar **un único documento digital (ej. Word, Google Docs, PDF)** que contenga, en orden:

*   **A. Captura de Pantalla del Dashboard de Node-RED en Funcionamiento** (con su breve explicación).
*   **B. Captura de Pantalla del Flujo de Node-RED** (con su breve explicación).
*   **C. (Opcional, recomendado) Breve Video o Serie de Capturas Secuenciales** (con su breve explicación, o un enlace al video si es muy grande).
*   **D. Breve Explicación General del Resultado.**

    *(Instrucción general: El documento debe ser claro, organizado y todas las capturas legibles. Nombren el archivo final de forma descriptiva, ej. `GrupoX_Lab_MQTT.pdf`)*

**5. Desarrollo del Laboratorio Calificado: Distribución de Tiempos y Actividades**

*Se asume que el entorno de desarrollo (Arduino IDE, Node-RED, broker MQTT) está preinstalado y funcional.*
*Duración Total del Laboratorio: 100 minutos.*

**Instrucción Clave: A medida que avancen en el laboratorio, deberán ir construyendo un documento digital (su "Informe de Laboratorio") donde incorporarán los elementos solicitados (capturas, explicaciones) en las fases correspondientes.**

---
**Fase 0: Preparación y Planificación (Duración: 10 minutos) - AMBOS ESTUDIANTES**
---
*   **(Primeros 5 min de la fase) Entendimiento y Acuerdo:**
    *   Releer rápidamente los requisitos y objetivos.
    *   **ACCIÓN CRÍTICA:** Definir y acordar los **nombres exactos de los tópicos MQTT** para comandos y estados de cada LED (Ej: `domotica/sala/comando`, `domotica/sala/estado`, etc.).
    *   Definir los pines del Arduino que se usarán para cada LED.
    *   **Iniciar su documento de Informe de Laboratorio:** Pueden crear un encabezado simple y anotar los tópicos y pines acordados.
*   **(Siguientes 5 min de la fase) División de Tareas Iniciales:**
    *   **Estudiante 1:** Se enfocará inicialmente en el montaje del hardware y la programación del Arduino.
    *   **Estudiante 2:** Se enfocará inicialmente en la configuración del broker MQTT (si es necesario) y el diseño del flujo y dashboard en Node-RED.

---
**Fase 1: Desarrollo Individual Inicial (Duración: 40 minutos)**
---
*   **Estudiante 1 (Foco: Arduino y Hardware):**
    *   **(Primeros 10 min de esta fase) Montaje Físico:** Conectar los 3 LEDs (con sus resistencias) a los pines definidos del Arduino Nano IoT. Conectar el Arduino al PC.
    *   **(Siguientes 30 min de esta fase) Programación Arduino:**
        *   Configurar conexión WiFi.
        *   Configurar cliente MQTT (IP del broker, puerto, tópicos acordados).
        *   Implementar función `callback` para recibir mensajes MQTT de comando.
        *   Controlar físicamente **UN LED** según el comando MQTT recibido.
        *   Publicar el estado inicial y los cambios de estado de ESE LED al tópico MQTT de estado correspondiente.
        *   *Objetivo Mínimo Funcional Arduino al final de esta fase: Arduino conectado, UN LED controlable vía MQTT y reportando su estado.*

*   **Estudiante 2 (Foco: Node-RED y Broker):**
    *   **(Primeros 5 min de esta fase) Broker MQTT:** Asegurar que el broker MQTT esté funcionando y accesible.
    *   **(Siguientes 35 min de esta fase) Diseño y Configuración en Node-RED:**
        *   Crear un nuevo flujo.
        *   Configurar el nodo de servidor MQTT broker en Node-RED.
        *   Añadir nodos `mqtt in` para suscribirse a los tópicos de ESTADO de los 3 LEDs.
        *   Añadir nodos `mqtt out` para publicar en los tópicos de COMANDO de los 3 LEDs.
        *   Diseñar el Dashboard inicial:
            *   Añadir 3 elementos de control (ej. `ui_switch` o `ui_button`) para enviar comandos "ON"/"OFF" a cada LED.
            *   Añadir 3 elementos de visualización (ej. `ui_text` o `ui_led`) para mostrar el estado de cada LED.
            *   Conectar inicialmente los controles y visualizadores para **UN LED** a sus respectivos nodos MQTT.
        *   *Objetivo Mínimo Funcional Node-RED al final de esta fase: Dashboard con capacidad de enviar comando MQTT para UN LED y visualizar su estado (recibido del Arduino o simulado).*

---
**Fase 2: Integración y Desarrollo Conjunto (Duración: 35 minutos) - AMBOS ESTUDIANTES COLABORANDO**
---
*   **(Primeros 15 min de esta fase) Primera Integración y Pruebas (Foco en UN LED):**
    *   **ACCIÓN GRUPAL:** Verificar que el Arduino (Estudiante 1) y Node-RED (Estudiante 2) estén conectados al mismo broker MQTT y utilizando los tópicos MQTT correctos para el PRIMER LED.
    *   Probar el ciclo completo y depurar conjuntamente.

*   **(Siguientes 20 min de esta fase) Expandir a los 3 LEDs y Refinar:**
    *   **Estudiante 1:** Replicar la lógica en el código Arduino para los otros dos LEDs.
    *   **Estudiante 2:** Completar y refinar el flujo y el dashboard en Node-RED para los tres LEDs.
    *   **ACCIÓN GRUPAL:** Probar y depurar el funcionamiento completo de los tres LEDs.
        *   **A medida que el sistema funciona, comenzar a pensar en las capturas de pantalla que se necesitarán para el informe.**

---
**Fase 3: Pruebas Finales, Documentación y Cierre (Duración: 15 minutos) - AMBOS ESTUDIANTES COLABORANDO**
---
*   **(Primeros 10 min de esta fase) Pruebas Exhaustivas e Incorporación de Evidencia al Informe:**
    *   **ACCIÓN GRUPAL:**
        *   Probar todas las combinaciones de encendido/apagado.
        *   Verificar la consistencia del sistema.
        *   **Capturar e insertar en su Informe de Laboratorio el Entregable A (Dashboard de Node-RED):**
            *   Asegúrense que la captura muestre los controles e indicadores para los tres LEDs, con al menos un LED en estado diferente.
        *   **Capturar e insertar en su Informe de Laboratorio el Entregable B (Flujo de Node-RED):**
            *   Asegúrense que la captura muestre los nodos MQTT y del dashboard relevantes.
        *   **(Opcional) Capturar/Grabar e insertar (o enlazar) en su Informe de Laboratorio el Entregable C (Video/Secuencia):**
            *   Demostrar la interacción cambiando el estado de un LED.

*   **(Siguientes 5 min de esta fase) Revisión Final y Redacción de Explicaciones en el Informe:**
    *   **ACCIÓN GRUPAL:**
        *   Asegurarse de que el sistema cumple con todos los requisitos.
        *   **Redactar e insertar directamente en su Informe de Laboratorio las Breves Explicaciones para Entregables A, B y (si aplica) C:**
            *   Junto a la captura del Dashboard (A): 1-2 frases describiendo el estado mostrado.
            *   Junto a la captura del Flujo (B): 2-3 frases describiendo la estructura general y tópicos clave.
            *   Junto al Video/Secuencia (C): 1-2 frases describiendo la acción demostrada.
        *   **Redactar e insertar en su Informe de Laboratorio el Entregable D (Breve Explicación General del Resultado):**
            *   Confirmar funcionamiento, mencionar tópicos MQTT principales y cualquier desafío/aspecto destacable (1 párrafo - máximo 5 frases).
        *   **Revisar la organización y claridad general del Informe de Laboratorio.**
        *   Guardar la versión final del documento para la entrega.

---
**FIN DEL LABORATORIO CALIFICADO**
---
