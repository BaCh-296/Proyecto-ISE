# Presentacion_ISE

Trabajo final — Introducción a los Sistemas Embebidos
Autor: Bartolomé Chiappe

## Descripción

Sistema sobre NUCLEO-F446RE que lee una señal de audio con un micrófono KY-038
(acoplado a un estetoscopio), la suaviza con un promedio móvil, estima la
frecuencia cardíaca (BPM) y envía los datos por UART a Node-RED para graficarlos.

## Periféricos

- **ADC1 (PA0):** lectura de la señal del micrófono (polling, 12 bits).
- **USART2:** envío de datos al PC vía ST-Link (115200 baudios).

## Conexiones

| KY-038 | NUCLEO |
|--------|--------|
| AO | PA0 |
| VCC | 3.3V |
| GND | GND |

## Uso

1. Compilar y flashear el proyecto en STM32CubeIDE.
2. Importar el flujo de `node-red/` en Node-RED, ajustar el puerto serie y hacer Deploy.
3. Abrir `http://localhost:1880/dashboard`.

## Carpetas

- `node-red/` — flujo exportado
- `docs/` — documentación técnica y evidencia
