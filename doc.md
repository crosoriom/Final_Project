# Room Control System v2.0

## Overview

Este es un sistema de control de habitación basado en un microcontrolador STM32, diseñado como un proyecto de IoT completo. El sistema gestiona la seguridad de una puerta, el control ambiental y proporciona múltiples interfaces de usuario, incluyendo un teclado físico, una terminal de comandos en serie y un panel de control web remoto.

El firmware está construido sobre la capa de abstracción de hardware (HAL) de STM32, pero está estructurado con una capa de aplicación limpia y modular que gestiona toda la lógica del sistema. Esto permite una fácil expansión y mantenimiento.

## Features

-   **Control de Puerta Avanzado:**
    -   Bloqueo y desbloqueo seguro de la puerta.
    -   Desbloqueo temporal a través de un botón físico o un comando remoto.
    -   Desbloqueo permanente.
    -   Bloqueo automático después de un desbloqueo temporal.
-   **Múltiples Métodos de Acceso:**
    -   Teclado 4x4 para acceso con contraseña.
    -   Botón físico para anulación o cambio de estado.
    -   Panel de control web para control remoto completo.
-   **Control Ambiental:**
    -   Control de velocidad de un ventilador con 4 niveles (Off, Low, Med, High).
    -   Monitorización de la temperatura ambiente utilizando el sensor interno del STM32.
-   **Interfaz de Usuario y Monitorización:**
    -   Pantalla OLED para mostrar el estado del sistema en tiempo real (estado de la puerta, velocidad del ventilador, temperatura).
    -   Panel de control web basado en MQTT para monitorizar y controlar el sistema desde cualquier navegador.
    -   Terminal de comandos en serie (CLI) para depuración y control directo.
-   **Sistema Robusto y Seguro:**
    -   Modo de Emergencia que bloquea el sistema y maximiza el ventilador.
    -   Estado de "Acceso Denegado" que bloquea el teclado temporalmente tras un intento de contraseña fallido.
    -   Lógica no bloqueante en el bucle principal, utilizando interrupciones y buffers circulares para una comunicación eficiente.

## System Architecture

### Diagrama de Contexto

Este diagrama muestra las interacciones entre los componentes principales del sistema.

```mermaid
graph TD
    subgraph "User Interfaces"
        User[Usuario]
        WebApp["Navegador Web (Dashboard)"]
        SerialTerminal["PC (Terminal Serie)"]
    end

    subgraph "Network"
        Broker["MQTT Broker"]
    end

    subgraph "Embedded System"
        STM32((STM32 Nucleo L476RG))
        WiFiModule["Módulo WiFi (ESP8266/32)"]
        
        subgraph "Peripherals"
            Keypad["Teclado 4x4"]
            Button["Botón Físico"]
            OLED["Pantalla OLED 128x64"]
            Fan["Ventilador (PWM)"]
            StatusLED["LEDs de Estado"]
            TempSensor{"Sensor Temp. Interno"}
        end
    end

    User -- Clicks --> WebApp
    User -- Tecla --> Keypad
    User -- Presiona --> Button
    User -- Comandos --> SerialTerminal

    WebApp -- "MQTT (WebSocket)" --> Broker
    SerialTerminal -- "USB Serial" --> STM32

    STM32 -- "UART (AT Cmds)" --> WiFiModule
    WiFiModule -- "MQTT (TCP)" --> Broker
    Broker -- "MQTT (WebSocket)" --> WebApp

    Keypad -- GPIO --> STM32
    Button -- EXTI --> STM32
    STM32 -- I2C --> OLED
    STM32 -- PWM --> Fan
    STM32 -- GPIO --> StatusLED
    TempSensor -- ADC --> STM32
```

### Diagrama de Flujo del Programa

Este diagrama ilustra el flujo de ejecución principal del firmware del STM32, destacando la arquitectura de superloop no bloqueante y la gestión de interrupciones.

```mermaid
graph TD
    %% =================================================================
    %% STAGE 1: INITIALIZATION
    %% =================================================================
    subgraph "Etapa 1: Inicialización (Secuencial)"
        direction TB
        Start((Inicio)) --> Init_HAL["Inicializar HAL y Reloj del Sistema (SystemClock_Config)"]
        Init_HAL --> Init_Periphs["Inicializar Periféricos HAL (generado por CubeMX: GPIO, UART, I2C, ADC, TIM)"]
        Init_Periphs --> Init_Software["Inicializar Módulos de Software (Ring Buffers, Keypad)"]
        Init_Software --> Init_App["Inicializar Capa de Aplicación (room_control_init)"]
        Init_App --> Sub_Init_App_Details
        subgraph "Dentro de room_control_init"
            direction TB
            Sub_Init_App_Details[Establecer estados iniciales (Puerta, Ventilador)] --> Sub_Init_PWM[Iniciar PWM del Ventilador (HAL_TIM_PWM_Start)]
            Sub_Init_PWM --> Sub_Init_OLED[Inicializar Pantalla OLED (ssd1306_init)]
            Sub_Init_OLED --> Sub_Init_ADC[Iniciar y Calibrar ADC (HAL_ADC_Start, HAL_ADCEx_Calibration_Start)]
        end
        Sub_Init_App_Details --> End_Init_App
        End_Init_App --> Arm_Interrupts["Armar Interrupciones (HAL_UART_Receive_IT)"]
    end

    %% =================================================================
    %% STAGE 2: MAIN LOOP (SUPERLOOP)
    %% =================================================================
    Arm_Interrupts --> Superloop{"Bucle Principal: while(1)"}
    subgraph "Etapa 2: Superloop (Cíclico y No Bloqueante)"
        direction TB
        Superloop --> Task_Heartbeat{"¿Es tiempo del Heartbeat?"}
        Task_Heartbeat -->|Sí| Action_ToggleLED[Alternar LED (HAL_GPIO_TogglePin)]
        Task_Heartbeat -->|No| Task_Keypad
        Action_ToggleLED --> Task_Keypad

        Task_Keypad --> Action_PollKeypad["Sondear Teclado (keypad_get_key)"]
        Action_PollKeypad --> Decision_Key_Pressed{"¿Se presionó una tecla?"}
        Decision_Key_Pressed -->|Sí| Action_NotifyApp_Keypad["Notificar a la App (room_control_keypad_char_received)"]
        Decision_Key_Pressed -->|No| Task_AppProcess
        Action_NotifyApp_Keypad --> Task_AppProcess

        Task_AppProcess --> Action_AppProcess["Llamar a room_control_process()"]
        Action_AppProcess --> Superloop
    end
    
    %% =================================================================
    %% STAGE 3: ASYNCHRONOUS INTERRUPTS
    %% =================================================================
    subgraph "Etapa 3: Manejo de Interrupciones (Asíncrono)"
        direction LR
        HW_Event("Evento de Hardware<br/>(Byte en UART, Botón presionado)") -- "Dispara Interrupción" --> HAL_ISR["Handler de Interrupción HAL (p.ej., USART2_IRQHandler)"]
        HAL_ISR -- "Llama a" --> HAL_Callback["Callback de HAL<br/>(HAL_UART_RxCpltCallback, HAL_GPIO_EXTI_Callback)"]
        subgraph "Dentro de los Callbacks"
            HAL_Callback -- "Si es UART" --> Action_WriteBuffer["Escribir byte en Ring Buffer (ring_buffer_write)"]
            HAL_Callback -- "Si es Botón" --> Action_NotifyApp_Button["Notificar a la App (room_control_button_pressed)"]
        end
        Action_WriteBuffer -.-> Data_Consumed
    end
    
    %% Connection between main loop and interrupts
    subgraph "Dentro de room_control_process"
        direction TB
        Start_AppProcess("(Entrada a room_control_process)") --> Process_Timers{"¿Es tiempo de actualizar<br/>Temp/Display/Publish?"}
        Process_Timers -->|Sí| Action_UpdateSensors["Actualizar Sensores y Display"]
        Process_Timers -->|No| Process_Door
        Action_UpdateSensors --> Process_Door
        
        Process_Door --> Action_UpdateDoor[Actualizar estado de la puerta (timer de desbloqueo temporal)]
        Action_UpdateDoor --> Process_Buffers["Procesar Buffers de Datos (System y WiFi)"]
        Process_Buffers -- "Consume datos de" --> Data_Consumed((Ring Buffers))
        Process_Buffers --> End_AppProcess("(Salida)")
    end
    
    Action_AppProcess -- Contiene --> Start_AppProcess
```

## Requisitos de Hardware y Software

-   **Hardware:**
    -   Placa de desarrollo STM32 Nucleo-L476RG.
    -   Pantalla OLED 128x64 (SSD1306).
    -   Teclado matricial 4x4.
    -   Módulo WiFi con firmware AT (e.g., ESP8266, ESP32).
    -   Botón pulsador.
    -   LED y motor DC (o ventilador) para el control PWM.
-   **Software:**
    -   STM32CubeIDE.
    -   Un broker MQTT (e.g., Mosquitto) instalado en la red local.
    -   Un navegador web moderno.
    -   Un cliente de terminal serie (e.g., PuTTY, TeraTerm).

## Estructura del Proyecto

El firmware está organizado con una clara separación de responsabilidades:

-   `Core/Inc`: Contiene los archivos de cabecera (`.h`), incluyendo `main.h`, los manejadores de HAL, y nuestros drivers de aplicación como `room_control.h`.
-   `Core/Src`: Contiene los archivos fuente (`.c`), incluyendo `main.c`, `stm32l4xx_it.c` (manejadores de interrupciones), y la implementación de nuestra aplicación `room_control.c`.
-   **Capa de Aplicación (`room_control.c`)**: Contiene toda la lógica del sistema, estados y reglas. Es independiente del hardware y utiliza funciones HAL para interactuar con los periféricos.

## Configuración y Uso

1.  **Broker MQTT:** Asegúrate de que tu broker Mosquitto esté corriendo en tu red. Para la interfaz web, debes habilitar el listener de WebSockets. Añade estas líneas a tu archivo `mosquitto.conf`:
    ```conf
    listener 1883
    listener 9001
    protocol websockets
    allow_anonymous true
    ```
2.  **Firmware:**
    -   Abre el proyecto (`.ioc` file) en STM32CubeIDE.
    -   Verifica que todos los periféricos (USART2, USART3, I2C1, ADC1, TIM3, GPIOs) estén configurados correctamente.
    -   Genera el código desde CubeMX.
    -   Construye y flashea el firmware en la placa Nucleo.
3.  **Interfaz Web:**
    -   Abre el archivo `app.js`.
    -   Modifica la constante `BROKER_URL` con la dirección IP y el puerto WebSocket de tu broker MQTT.
    -   Abre el archivo `index.html` en un navegador web.

## Referencia de Comandos (Terminal Serie)

La comunicación con el sistema a través de la terminal serie (USART2) se realiza con los siguientes comandos, terminados con la tecla Enter.

| Comando | Parámetros | Descripción (Inglés / Español) |
| :--- | :--- | :--- |
| `HELP` | - | Displays the command list. / Muestra esta lista de comandos. |
| `STATUS` | - | Returns the full system status. / Devuelve el estado completo del sistema. |
| `GET_TEMP` | - | Returns the current temperature. / Devuelve la temperatura actual. |
| `FAN` | `[level]` | Sets fan speed. Level can be 0-3 or 0, 25, 60, 100. / Fija la velocidad del ventilador. |
| `SETPASS` | `[new_pass]` | Changes the keypad access password. / Permite cambiar la contraseña de acceso. |
| `LOCK` | `[0\|1]` | 0=Unlock permanently, 1=Lock door. / 0=Desbloquea permanentemente, 1=Bloquea. |
| `REMOTE_OPEN` | - | Temporarily unlocks the door. / Desbloquea la puerta temporalmente. |
| `EMERGENCY` | `[0\|1]` | 0=Disable, 1=Enable emergency mode. / 0=Desactiva, 1=Activa el modo emergencia. |

## Mejoras Futuras

-   Implementar un parser robusto de comandos AT para gestionar las respuestas del módulo WiFi.
-   Guardar la contraseña y el estado del sistema en memoria no volátil (EEPROM o Flash).
-   Migrar el bucle principal a un sistema operativo en tiempo real (FreeRTOS) para gestionar las tareas.
-   Expandir la interfaz web con gráficos y controles más avanzados.
