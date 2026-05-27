# Preguntas Frecuentes (FAQ)

Si no encuentras lo que buscas, por favor abre un issue en [GitHub](https://github.com/XE1E/svitrix-firmware-XE1E/issues).

## General

#### P: ¿SVITRIX funciona sin un sistema de hogar inteligente?

R: Sí, parcialmente. SVITRIX viene con apps integradas para hora, fecha, temperatura, humedad y nivel de batería — funcionan directamente sin ningún sistema externo.

Sin embargo, SVITRIX está diseñado principalmente como un compañero de hogar inteligente. Para desbloquear todo su potencial — apps personalizadas, notificaciones, control remoto — necesitas un sistema externo (HomeAssistant, IOBroker, NodeRed, etc.) que se comunique vía MQTT o API HTTP.

#### P: ¿Puedo usar tamaños de matriz diferentes a 32x8?

R: No. SVITRIX está optimizado para el Ulanzi TC001, que usa una matriz de 32x8 píxeles (256 LEDs). Otros tamaños no están soportados.

## Flasheo e Instalación

#### P: Estoy obteniendo un error al flashear con el SVITRIX Web Flasher

R: Prueba con un cable USB y puerto USB diferente. Algunos cables (incluyendo el que viene con el dispositivo) solo soportan carga, no transferencia de datos. Un buen cable de datos es la solución más común.

#### P: Solo tengo 192KB de memoria flash libre. ¿Qué está mal?

R: El firmware original de Ulanzi usa una tabla de particiones diferente con una partición de sistema de archivos más pequeña.

Para obtener los ~1350KB completos de almacenamiento disponible, flashea usando el [flasher online de SVITRIX](./flasher) — escribe en todas las particiones incluyendo el sistema de archivos. El actualizador OTA de Ulanzi solo escribe en la partición de la app y deja la configuración de particiones antigua.

## Pantalla e Iconos

#### P: Estoy experimentando problemas gráficos con mis iconos

R: El renderizador de GIF de SVITRIX soporta GIFs de 8 bits. Si notas glitches gráficos, tu GIF probablemente tiene transparencia. Reemplaza los píxeles transparentes con negro sólido usando una herramienta online como [esta](https://onlinegiftools.com/add-gif-background).

#### P: Tengo un indicador parpadeante en la esquina. ¿Qué significa?

R: SVITRIX usa pequeños LEDs indicadores en las esquinas de la matriz para señalar problemas de conexión:

| Indicador | Posición | Significado |
|-----------|----------|-------------|
| 1 | Esquina superior derecha | Problema de conexión WiFi |
| 2 | Esquina inferior derecha | Problema de conexión al broker MQTT |

Una vez que se restaura la conexión, el indicador desaparece automáticamente.

## Configuración

#### P: ¿Qué es `DoNotTouch.json` y qué pasa si lo modifiqué accidentalmente?

R: `DoNotTouch.json` almacena configuraciones críticas del dispositivo:

- Configuración de IP estática
- Configuración de conexión MQTT
- Prefijo de Home Assistant Discovery
- Servidor NTP / zona horaria
- Contraseña de autenticación HTTP

Si modificaste accidentalmente este archivo, elimínalo vía el [administrador de archivos](./webinterface) y reinicia el dispositivo. El archivo se recreará automáticamente, pero necesitarás reconfigurar las opciones listadas arriba.

#### P: El sensor de temperatura muestra un valor más alto de lo esperado

R: El sensor en el Ulanzi TC001 está ubicado dentro de la carcasa sellada. El calor interno de la matriz LED (dependiendo del brillo) y la batería eleva la lectura por encima de la temperatura real de la habitación.

Puedes compensar configurando offsets en `dev.json` vía el [administrador de archivos](./webinterface):

```json
{
  "temp_offset": -5,
  "hum_offset": -1
}
```

Reinicia el dispositivo después de guardar. Ajusta los valores para que coincidan con tu ambiente — el offset exacto depende de la configuración de brillo y uso de batería.

Consulta todas las configuraciones de desarrollador disponibles en la sección [Funciones Ocultas](./dev).

## Builds Personalizados

#### P: Quiero construir mi propio SVITRIX. ¿Qué firmware debo usar?

R: Usa el [flasher de SVITRIX](./flasher) — el firmware es compatible tanto con Ulanzi TC001 como con builds personalizados usando cualquier placa ESP32-WROOM (incluyendo ESP32 D1 Mini).

Asegúrate de seguir el pinout correcto de la [Guía de Hardware](./hardware).

#### P: Mi dispositivo auto-construido muestra caracteres ilegibles en la matriz

R: Necesitas cambiar el tipo de configuración de matriz. El valor por defecto es `0`, que puede no coincidir con tu cableado.

Crea un archivo `dev.json` vía el [administrador de archivos](./webinterface) e intenta con los valores `1` o `2`:

```json
{
  "matrix": "1"
}
```

| Valor | Configuración |
|-------|---------------|
| 0 | 32x8 único, fila mayor, zigzag |
| 1 | Cuatro paneles 8x8 en mosaico, fila mayor, progresivo |
| 2 | 32x8 único, columna mayor, zigzag |

Reinicia el dispositivo después de guardar.

## Actualizaciones OTA

#### P: ¿Cómo actualizo el firmware sin USB?

R: Abre la [interfaz web](./webinterface) en tu navegador, ve a la sección de actualización y sube el nuevo archivo `firmware.bin`. El dispositivo se reiniciará automáticamente después de la actualización.

Puedes descargar el firmware más reciente de la página de [releases de GitHub](https://github.com/XE1E/svitrix-firmware-XE1E/releases).
