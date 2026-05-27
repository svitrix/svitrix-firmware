# Funciones de Desarrollador

Esta sección contiene pequeñas opciones de configuración que la mayoría de los usuarios no cambian o cambian muy raramente y por lo tanto se ahorraron el esfuerzo de crear una interfaz de configuración elaborada.

Todas las funciones solo se aplican al arrancar. Así que tienes que reiniciar SVITRIX después de modificar.

Crea un `dev.json` en tu administrador de archivos.

## Propiedades JSON

El objeto JSON tiene las siguientes propiedades:

| Clave | Tipo | Descripción | Por defecto |
| --- | ---- | ----------- | ----------- |
| `hostname` | string | Cambia el nombre de host de tu SVITRIX. Esto se usa para descubrimiento de App, mDNS, etc. | ID único |
| `ap_timeout` | integer | El timeout en segundos antes de que SVITRIX cambie a modo AP si la WLAN guardada no se encontró. | 15 |
| `bootsound` | string | Usa una melodía personalizada al arrancar |  |
| `matrix` | integer | Cambia el diseño de la matriz (0, 1 o 2) | `0` |
| `color_correction` | array de int | Establece la corrección de color de la matriz | `[255,255,255]` |
| `color_temperature` | array de int | Establece la temperatura de color de la matriz | `[255,255,255]` |
| `rotate_screen` | boolean | Rota la pantalla al revés | `false` |
| `mirror_screen` | boolean | Refleja la pantalla | `false` |
| `temp_dec_places` | integer | Número de decimales para mediciones de temperatura | `0` |
| `sensor_reading` | boolean | Habilita o deshabilita la lectura del sensor de Temp y Humedad | `true` |
| `temp_offset` | float | Establece el offset para la medición de temperatura interna | `-9` |
| `hum_offset` | float | Establece el offset para la medición de humedad interna | `0` |
| `min_brightness` | integer | Establece el nivel mínimo de brillo para el control de Brillo Automático | `2` |
| `max_brightness` | integer | Establece el nivel máximo de brillo para el control de Brillo Automático. ¡En niveles altos, esto podría resultar en sobrecalentamiento! | `180` |
| `ldr_gamma` | float | Permite establecer la corrección gamma del control de brillo | 3.0 |
| `ldr_factor` | float | Este factor se calcula en el valor raw del LDR que es 0-1023 | 1.0 |
| `min_battery` | integer | Calibra la medición mínima de batería con el valor raw dado. Lo obtienes de la API de stats | `475` |
| `max_battery` | integer | Calibra la medición máxima de batería con el valor raw dado. Lo obtienes de la API de stats | `665` |
| `ha_prefix` | string | Establece el prefijo para descubrimiento de Home Assistant | `homeassistant` |
| `background_effect` | string | Establece un [efecto](./effects) como capa de fondo global | - |
| `stats_interval` | integer | Establece el intervalo en milisegundos cuando SVITRIX debe enviar sus estadísticas a HA y MQTT | 10000 |
| `debug_mode` | boolean | Habilita salidas de depuración serial. | false |
| `button_callback` | string | URL de callback HTTP para pulsaciones de botones. | - |
| `new_year` | boolean | Muestra fuegos artificiales y reproduce un jingle en año nuevo. | false |
| `swap_buttons` | boolean | Intercambia los botones de hardware izquierdo y derecho. | false |
| `ldr_on_ground` | boolean | Establece la configuración del LDR a LDR-en-tierra. | false |
| `update_check` | boolean | Habilita verificaciones automáticas de actualización de firmware. | true |
| `web_port` | integer | Cambia el puerto del servidor web HTTP. | `80` |
| `update_version_url` | string | URL personalizada para verificar la versión del firmware. | integrada |
| `update_firmware_url` | string | URL personalizada para descargar el binario del firmware. | integrada |


#### Ejemplo:
```json
{
  "temp_dec_places":1,
  "bootsound":true,
  "hum_offset":-2
}
```
