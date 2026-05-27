# API MQTT / HTTP

## Tabla de Contenidos
  * [Resumen](#resumen)
  * [Obtener Estado](#obtener-estado)
  * [Vista en Vivo](#vista-en-vivo)
  * [Control de Energía](#control-de-energía)
  * [Reproducción de Sonido](#reproducción-de-sonido)
  * [Iluminación Ambiental](#iluminación-ambiental)
  * [Indicadores de Color](#indicadores-de-color)
  * [Apps Personalizadas y Notificaciones](#apps-personalizadas-y-notificaciones)
    + [Interacción](#interacción)
    + [Propiedades JSON](#propiedades-json)
      - [Ejemplo](#ejemplo)
    + [Instrucciones de Dibujo](#instrucciones-de-dibujo)
    + [Ejemplo](#ejemplo-1)
    + [Mostrar Texto en Fragmentos Coloreados](#mostrar-texto-en-fragmentos-coloreados)
    + [Enviar Múltiples Apps Personalizadas Simultáneamente](#enviar-múltiples-apps-personalizadas-simultáneamente)
    + [Eliminar una App Personalizada](#eliminar-una-app-personalizada)
    + [Descartar Notificación](#descartar-notificación)
    + [Cambiar Apps](#cambiar-apps)
    + [Cambiar a App Específica](#cambiar-a-app-específica)
  * [Data Fetcher](#data-fetcher)
  * [Cambiar Configuración](#cambiar-configuración)
    + [Propiedades JSON](#propiedades-json-1)
  * [Actualizar](#actualizar)
      - [Reiniciar Svitrix](#reiniciar-svitrix)
      - [Borrar Svitrix](#borrar-svitrix)
      - [Limpiar Configuración](#limpiar-configuración)


## Resumen

Esta documentación de API cubre varias funcionalidades como obtener estadísticas del dispositivo, espejo de pantalla, notificaciones, apps personalizadas, reproducción de sonido e iluminación ambiental. Puedes interactuar con estas funciones tanto vía protocolos MQTT como HTTP.

## Obtener Estado

Accede a varias estadísticas del dispositivo como batería, RAM y más:

| Topic MQTT                     | URL HTTP                          | Descripción                                   |
| ------------------------------ | --------------------------------- | --------------------------------------------- |
| `[PREFIX]/stats`               | `http://[IP]/api/stats`           | Estadísticas generales del dispositivo (ej., batería, RAM) |
| `[PREFIX]/stats/effects`       | `http://[IP]/api/effects`         | Lista de todos los efectos                    |
| `[PREFIX]/stats/transitions`   | `http://[IP]/api/transitions`     | Lista de todos los efectos de transición      |
| `[PREFIX]/stats/loop`          | `http://[IP]/api/loop`            | Lista de todas las apps en el ciclo           |

> **Nota:** MQTT también transmite otros datos, como pulsaciones de botones y la app actual.

## Vista en Vivo

Obtén la pantalla actual de la matriz como un arreglo de colores de 24 bits:

| Topic MQTT             | URL HTTP                    | Payload/Body | Método HTTP |
| ---------------------- | --------------------------- | ------------ | ----------- |
| `[PREFIX]/sendscreen`  | `http://[IP]/api/screen`    | -            | GET         |

Al activar la API MQTT, SVITRIX envía el arreglo a `[PREFIX]/screen`.

**Extras:**
- Accede a una vista en vivo de la pantalla en tu navegador en `http://[IP]` (la página principal del SPA).
- La página de Pantalla renderiza la matriz en tiempo real en un elemento canvas con soporte de descarga PNG.

## Control de Energía

Alterna la matriz encendido o apagado:

| Topic MQTT      | URL HTTP                     | Payload/Body                           | Método HTTP |
| --------------- | ---------------------------- | -------------------------------------- | ----------- |
| `[PREFIX]/power`| `http://[IP]/api/power`      | `{"power": true}` o `{"power": false}` | POST        |

Envía la placa a modo deep sleep (también apaga la matriz), bueno para ahorrar batería:

| Topic MQTT      | URL HTTP                     | Payload/Body                                  | Método HTTP |
| --------------- | ---------------------------- | --------------------------------------------- | ----------- |
| `[PREFIX]/sleep`| `http://[IP]/api/sleep`      | `{"sleep": X}` donde X es número de segundos  | POST        |

SVITRIX solo despertará después del tiempo o si presionas el botón del medio una vez. No hay forma de despertar vía API.

## Reproducción de Sonido

Reproduce un sonido RTTTL de la carpeta MELODIES.

| Topic MQTT       | URL HTTP                    | Payload/Body         | Método HTTP |
| ---------------- | --------------------------- | -------------------- | ----------- |
| `[PREFIX]/sound` | `http://[IP]/api/sound`     | `{"sound":"alarma"}` | POST        |

Reproduce un sonido RTTTL desde una cadena RTTTL dada:

| Topic MQTT       | URL HTTP                    | Payload/Body    | Método HTTP |
| ---------------- | --------------------------- | --------------- | ----------- |
| `[PREFIX]/rtttl` | `http://[IP]/api/rtttl`     | `cadena rtttl`  | POST        |


## Iluminación Ambiental

Establece toda la matriz a un color o temperatura personalizada:

| Topic MQTT            | URL HTTP                         | Payload/Body | Método HTTP |
| --------------------- | -------------------------------- | ------------ | ----------- |
| `[PREFIX]/moodlight`  | `http://[IP]/api/moodlight`      | Ver abajo    | POST        |

> ⚠️ **Precaución:** Usar esta función resulta en mayor consumo de corriente y calor, especialmente cuando todos los píxeles están encendidos. Asegúrate de gestionar los valores de brillo responsablemente.

Para deshabilitar moodlight, envía un payload vacío.

**Ejemplo:**
```json
{
  "brightness": 170,
  "kelvin": 2300
}
```

Opciones posibles de moodlight:
```json
{"brightness":170,"kelvin":2300}
o
{"brightness":170,"color":[155,38,182]}
o
{"brightness":170,"color":"#FF00FF"}
```


## Indicadores de Color

Los indicadores de color sirven como pequeñas señales de notificación mostradas en áreas específicas de la pantalla:

- Esquina superior derecha: Indicador 1
- Lado derecho: Indicador 2
- Esquina inferior derecha: Indicador 3

| Topic MQTT            | URL HTTP                          | Payload/Body           | Método HTTP |
| --------------------- | --------------------------------- | ---------------------- | ----------- |
| `[PREFIX]/indicator1` | `http://[IP]/api/indicator1`      | `{"color":[255,0,0]}`  | POST        |
| `[PREFIX]/indicator2` | `http://[IP]/api/indicator2`      | `{"color":[0,255,0]}`  | POST        |
| `[PREFIX]/indicator3` | `http://[IP]/api/indicator3`      | `{"color":[0,255,0]}`  | POST        |

**Opciones de Color:**
- Usa un arreglo RGB, ej., `{"color":[255,0,0]}`
- Usa cadenas de color HEX, ej., `{"color":"#32a852"}`

**Ocultar Indicadores:**
- Para ocultar los indicadores, envía el color negro (`{"color":[0,0,0]}`) o usa la forma abreviada `{"color":"0"}`. Alternativamente, envía un payload vacío.

**Efectos Adicionales:**
- **Parpadeo**: Para hacer que el indicador parpadee, agrega la clave `"blink"` con un valor especificando el intervalo de parpadeo en milisegundos.
- **Desvanecimiento**: Para hacer que el indicador se desvanezca, agrega la clave `"fade"` con un valor especificando el intervalo de desvanecimiento en milisegundos.

## Apps Personalizadas y Notificaciones

Con SVITRIX, puedes diseñar apps personalizadas o notificaciones para mostrar tu texto e iconos únicos.

### Interacción

- **MQTT**: Envía un objeto JSON a `[PREFIX]/custom/[app]`, donde `[app]` denota el nombre de tu app (excluyendo espacios).
- **API HTTP**: Incorpora el nombre de la app en el parámetro de consulta (`name=[nombreapp]`).
- **Actualizar**: Para refrescar una página personalizada, envía un objeto JSON modificado al mismo endpoint. La pantalla se actualiza instantáneamente.
- **Notificación Única**: Usa el mismo formato JSON. Dirige tu objeto JSON a `[PREFIX]/notify` o `http://[IP]/api/notify`.

| Topic MQTT                 | URL HTTP                         | Payload/Body | Parámetros de Consulta | Método HTTP |
| -------------------------- | -------------------------------- | ------------ | ---------------------- | ----------- |
| `[PREFIX]/custom/[nombre]` | `http://[IP]/api/custom`         | JSON         | name=[nombreapp]       | POST        |
| `[PREFIX]/notify`          | `http://[IP]/api/notify`         | JSON         | -                      | POST        |

### Propiedades JSON

A continuación están las propiedades que puedes utilizar en el objeto JSON. **Todas las claves son opcionales**; solo incluye las propiedades que necesites.

| Clave | Tipo | Descripción | Por defecto | App Personalizada | Notificación |
| --- | ---- | ----------- | ----------- | ----------------- | ------------ |
| `text` | string | El texto a mostrar (UTF-8). Soporta caracteres ASCII, Cirílico (mayús/minús), Ucraniano y Latino Extendido. La fuente tiene ancho variable — `I` usa menos espacio que `W`, lo que afecta cuándo el texto comienza a desplazarse. | N/A | X | X |
| `textCase` | integer | Cambia la configuración de Mayúsculas. 0=configuración global, 1 = fuerza mayúsculas; 2 = muestra como se envió. | 0 | X | X |
| `topText` | boolean | Dibuja el texto en la parte superior. | false | X | X |
| `textOffset` | integer | Establece un offset para la posición x del texto inicial. | 0 | X | X |
| `center` | boolean | Centra un texto corto no desplazable. | true | X | X |
| `color` | string o array de integers | El color del texto, barra o línea. | N/A | X | X |
| `gradient` | Array de string o integers | Colorea el texto en un gradiente de dos colores dados. | N/A | X | X |
| `blinkText` | Integer | Parpadea el texto en un intervalo dado en ms, no compatible con gradiente o arcoíris. | N/A | X | X |
| `fadeText` | Integer | Desvanece el texto en un intervalo dado, no compatible con gradiente o arcoíris. | N/A | X | X |
| `background` | string o array de integers | Establece un color de fondo. | N/A | X | X |
| `rainbow` | boolean | Desvanece cada letra del texto diferentemente a través del espectro RGB completo. | false | X | X |
| `icon` | string | El ID del icono o nombre de archivo (sin extensión) para mostrar en la app. También puedes enviar un **jpg 8x8** como cadena Base64. | N/A | X | X |
| `pushIcon` | integer | 0 = El icono no se mueve. 1 = El icono se mueve con el texto y no aparecerá de nuevo. 2 = El icono se mueve con el texto pero aparece de nuevo cuando el texto comienza a desplazarse otra vez. | 0 | X | X |
| `repeat` | integer | Establece cuántas veces el texto debe desplazarse por la matriz antes de que la app termine. | -1 | X | X |
| `duration` | integer | Establece cuánto tiempo debe mostrarse la app o notificación. | 5 | X | X |
| `hold` | boolean | Ponlo en true para mantener tu **notificación** en la parte superior hasta que presiones el botón del medio o la descartes vía HomeAssistant. Esta clave solo pertenece a notificación. | false |  | X |
| `sound` | string | El nombre de archivo de tu archivo de tono RTTTL colocado en la carpeta MELODIES (sin extensión). | N/A |  | X |
| `rtttl` | string | Permite enviar la cadena de sonido RTTTL con JSON. | N/A |  | X |
| `loopSound` | boolean | Repite el sonido o RTTTL mientras la notificación está ejecutándose. | false |  | X |
| `bar` | array de integers | Dibuja un gráfico de barras. Sin icono máximo 16 valores, con icono 11 valores. | N/A | X | X |
| `line` | array de integers | Dibuja un gráfico de líneas. Sin icono máximo 16 valores, con icono 11 valores. | N/A | X | X |
| `autoscale` | boolean | Habilita o deshabilita autoescalado para gráficos de barras y líneas. | true | X | X |
| `barBC` | string o array de integers | Color de fondo de las barras. | 0 | X | X |
| `progress` | integer | Muestra una barra de progreso. El valor puede ser 0–100. | -1 | X | X |
| `progressC` | string o array de integers | El color de la barra de progreso. | -1 | X | X |
| `progressBC` | string o array de integers | El color del fondo de la barra de progreso. | -1 | X | X |
| `pos` | integer | Define la posición de tu página personalizada en el ciclo, comenzando en 0 para la primera posición. Esto solo se aplicará con tu primer push. Esta función es experimental. | N/A | X |  |
| `draw` | array de objetos | Array de instrucciones de dibujo. Cada objeto representa un comando de dibujo. Ver las instrucciones de dibujo abajo. |  | X | X |
| `lifetime` | integer | Elimina la app personalizada cuando no hay actualización después del tiempo dado en segundos. | 0 | X |  |
| `lifetimeMode` | integer | 0 = elimina la app, 1 = la marca como obsoleta con un rectángulo rojo alrededor de la app. | 0 | X |  |
| `stack` | boolean | Define si la **notificación** se apilará. `false` reemplazará inmediatamente la notificación actual. | true |  | X |
| `wakeup` | boolean | Si la Matriz está apagada, la notificación la despertará por el tiempo de la notificación. | false |  | X |
| `noScroll` | boolean | Deshabilita el desplazamiento de texto. | false | X | X |
| `clients` | array de strings | Permite reenviar una notificación a otros dispositivos SVITRIX. Usa el prefijo MQTT para MQTT y direcciones IP para HTTP. |  |  | X |
| `scrollSpeed` | integer | Modifica la velocidad de desplazamiento. Ingresa un valor porcentual de la velocidad de desplazamiento original. | 100 | X | X |
| `effect` | string | Muestra un [efecto](./effects) como fondo. El efecto puede eliminarse enviando una cadena vacía para effect. |  | X | X |
| `effectSettings` | mapa json | Cambia el color y velocidad del [efecto](./effects). |  | X | X |
| `save` | boolean | Guarda tu app personalizada en flash y la recarga después del arranque. Evita esto para apps personalizadas con alta frecuencia de actualización porque la memoria flash del ESP tiene ciclos de escritura limitados. |  | X |  |
| `layout` | string | Controla el posicionamiento del icono: `"left"` (por defecto), `"right"` o `"none"`. Cuando se establece a `"right"`, el icono aparece en el lado derecho de la pantalla. Cuando se establece a `"none"`, el icono se oculta y el texto usa el ancho completo de 32 píxeles. | `"left"` | X | X |
| `overlay`| string  | Establece una superposición de efecto (no puede usarse con superposiciones globales). |  | X | X |

**Color**: Acepta una cadena hex o un arreglo R,G,B: `"#FFFFFF"` o `[255,255,0]`.

**Efectos de superposición:**
- "clear"
- "snow"
- "rain"
- "drizzle"
- "storm"
- "thunder"
- "frost"


#### Ejemplo

Aquí hay un JSON de ejemplo para presentar el texto "¡Hola, SVITRIX!" en colores arcoíris por una duración de 10 segundos:

```json
{
  "text": "¡Hola, SVITRIX!",
  "rainbow": true,
  "duration": 10
}
```

### Placeholder MQTT

Esta función es particularmente útil para usuarios sin un sistema de hogar inteligente completo. Elimina la necesidad de un sistema externo para mostrar datos, como de un inversor que puede enviar sus datos vía MQTT. Puedes simplemente crear un archivo [NombreApp].json en la carpeta CUSTOMAPP con tus claves JSON de app personalizada. Este archivo JSON se cargará al arrancar, así que no necesitas enviarlo desde una fuente externa. O también puedes usarlo en tu petición de API HTTP o MQTT.

Los placeholders dentro del valor `text` encerrados en `{{}}` serán reemplazados con el payload del topic MQTT especificado. Actualmente, no hay opciones disponibles para formatear el payload.

```json
{"text": "Solar: {{inverter/total/P_AC}} W"}
```

### Instrucciones de Dibujo

::: warning
Por favor nota: Dependiendo del número de objetos, el uso de RAM puede ser muy alto. Esto podría causar congelamientos o reinicios.
:::
Es importante ser consciente del número de objetos y la complejidad de las instrucciones de dibujo para evitar problemas de rendimiento.

Cada instrucción de dibujo es un objeto con una clave de comando requerida y un arreglo de valores dependiendo del comando:

| Comando | Valores del Arreglo      | Descripción |
| ------- | ------------------------ | ----------- |
| `dp`    | `[x, y, cl]`             | Dibuja un píxel en la posición (`x`, `y`) con color `cl` |
| `dl`    | `[x0, y0, x1, y1, cl]`   | Dibuja una línea de (`x0`, `y0`) a (`x1`, `y1`) con color `cl` |
| `dr`    | `[x, y, w, h, cl]`       | Dibuja un rectángulo con esquina superior izquierda en (`x`, `y`), ancho `w`, alto `h`, y color `cl` |
| `df`    | `[x, y, w, h, cl]`       | Dibuja un rectángulo relleno con esquina superior izquierda en (`x`, `y`), ancho `w`, alto `h`, y color `cl` |
| `dc`    | `[x, y, r, cl]`          | Dibuja un círculo con centro en (`x`, `y`), radio `r`, y color `cl` |
| `dfc`   | `[x, y, r, cl]`          | Dibuja un círculo relleno con centro en (`x`, `y`), radio `r`, y color `cl` |
| `dt`    | `[x, y, t, cl]`          | Dibuja texto `t` con esquina superior izquierda en (`x`, `y`) y color `cl` |
| `db`    | `[x, y, w, h, [bmp]]`    | Dibuja un arreglo de bitmap RGB888 `[bmp]` con esquina superior izquierda en (`x`, `y`) y tamaño de (`w`, `h`) |


### Ejemplo

Aquí hay un objeto JSON de ejemplo para dibujar un círculo rojo, un rectángulo azul y el texto "Hola" en verde:

```json
{"draw":[
 {"dc": [28, 4, 3, "#FF0000"]},
 {"dr": [20, 4, 4, 4, "#0000FF"]},
 {"dt": [0, 0, "Hola", "#00FF00"]}
]}
```

### Mostrar Texto en Fragmentos Coloreados

SVITRIX te permite presentar texto donde fragmentos específicos pueden colorearse. Usa un arreglo de fragmentos con `"t"` representando el fragmento de texto y `"c"` denotando el valor hex del color.

```json
{
  "text": [
    {
      "t": "Hola, ",
      "c": "FF0000"
    },
    {
      "t": "mundo!",
      "c": "00FF00"
    }
  ],
  "repeat": 2
}
```

### Enviar Múltiples Apps Personalizadas Simultáneamente

SVITRIX te permite enviar múltiples apps personalizadas en una sola acción. En lugar de transmitir un objeto de app personalizada, puedes reenviar un arreglo de objetos.

**ej. Topic MQTT:** `/custom/test`

```json
[
  {"text":"1"},
  {"text":"2"}
]
```

**Manejo de Múltiples Apps Personalizadas:**
- **Asignación de Sufijo**: Internamente, el nombre de la app recibe un sufijo, convirtiéndolo en un formato como `test0`, `test1`, etc.
- **Actualizaciones**: Puedes refrescar cada app individualmente o actualizar todas colectivamente.
- **Eliminación**:
  - Al borrar apps, SVITRIX no coincide con el nombre exacto de la app. En cambio, identifica apps que comienzan con el nombre especificado.
  - Para eliminar todas las apps asociadas, envía un payload vacío a `/custom/test`. Esta acción eliminará `test0`, `test1`, y así sucesivamente.
  - Para eliminar una sola app, dirige el comando a, por ejemplo, `/custom/test1`.
  - Precaución: Eliminar solo una app puede alterar la secuencia de las apps restantes en el ciclo, ya que no hay provisión para placeholders para mantener el orden.




### Eliminar una App Personalizada

Para eliminar una app personalizada, envía un payload/body vacío al topic o URL asociado.

### Descartar Notificación

Descarta fácilmente una notificación que fue configurada con `"hold": true`.

| Topic MQTT                  | URL HTTP                          | Payload/Body       | Método HTTP |
| --------------------------- | --------------------------------- | ------------------ | ----------- |
| `[PREFIX]/notify/dismiss`   | `http://[IP]/api/notify/dismiss`  | Payload/body vacío | POST        |

### Cambiar Apps

Navega a la app siguiente o anterior.

| Topic MQTT                  | URL HTTP                          | Payload/Body       | Método HTTP |
| --------------------------- | --------------------------------- | ------------------ | ----------- |
| `[PREFIX]/nextapp`          | `http://[IP]/api/nextapp`         | Payload/body vacío | POST        |
| `[PREFIX]/previousapp`      | `http://[IP]/api/previousapp`     | Payload/body vacío | POST        |

### Cambiar a App Específica

Transiciona directamente a una app deseada usando su nombre.

| Topic MQTT                  | URL HTTP                          | Payload/Body       | Método HTTP |
| --------------------------- | --------------------------------- | ------------------ | ----------- |
| `[PREFIX]/switch`           | `http://[IP]/api/switch`          | `{"name":"Time"}`  | POST        |

**Nombres de Apps Integradas**:
- `Time`
- `Date`
- `Temperature`
- `Humidity`
- `Battery`

Para apps personalizadas, emplea el nombre que designaste en el topic o parámetro HTTP. En MQTT, si `[PREFIX]/custom/test` es tu topic, entonces `test` sería el nombre de la app.


## Data Fetcher

El Data Fetcher permite que SVITRIX obtenga periódicamente datos de APIs HTTP/HTTPS externas y muestre los resultados como apps personalizadas — sin necesidad de sistema de hogar inteligente.

### Gestionar Fuentes de Datos

| URL HTTP                             | Payload/Body | Método HTTP | Descripción |
| ------------------------------------ | ------------ | ----------- | ----------- |
| `http://[IP]/api/datafetcher`        | -            | GET         | Lista todas las fuentes configuradas |
| `http://[IP]/api/datafetcher`        | JSON         | POST        | Agrega o actualiza una fuente de datos |
| `http://[IP]/api/datafetcher?name=X` | -            | DELETE      | Elimina una fuente de datos |
| `http://[IP]/api/datafetcher/fetch?name=X` | -      | POST        | Fuerza una obtención inmediata |

> **Nota:** Data Fetcher es solo HTTP, no hay topics MQTT.

### JSON de Configuración de Fuente

| Clave | Tipo | Requerido | Descripción | Por defecto |
| --- | ---- | --------- | ----------- | ----------- |
| `name` | string | Sí | ID único de la fuente, se convierte en el nombre de la app personalizada | - |
| `url` | string | Sí | URL HTTP/HTTPS completa | - |
| `jsonPath` | string | Sí | Ruta con notación de punto para extraer de la respuesta JSON (ej., `"bitcoin.usd"`, `"data.0.price"`) | - |
| `displayFormat` | string | No | Formato estilo printf con una sola conversión (ej., `"$%.0f"`, `"%.1f°C"`). Permitidos: `%d %i %u %o %x %X %f %g %e %E %s` con modificadores de ancho y precisión; `%%` para un `%` literal. Otros formatos son rechazados con HTTP 400. | valor raw |
| `icon` | string | No | Nombre de icono de la carpeta ICONS (sin extensión) | ninguno |
| `color` | string | No | Color de texto como hex `"#RRGGBB"` | color de texto global |
| `interval` | integer | No | Intervalo de polling en segundos (mínimo 60) | 900 (15 min) |

### Ejemplo

Agregar un rastreador de precio de Bitcoin:

```json
{
  "name": "btc",
  "url": "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd",
  "jsonPath": "bitcoin.usd",
  "displayFormat": "$%.0f",
  "icon": "btc",
  "color": "#F7931A",
  "interval": 300
}
```

### Restricciones

- **Máximo 8 fuentes** pueden configurarse simultáneamente
- **Tamaño máximo de respuesta**: 4 KB — respuestas de API más grandes serán rechazadas
- **HTTPS**: soportado, pero sin validación de certificado (para APIs de terceros arbitrarias)
- **Polling round-robin**: una fuente se verifica por ciclo de loop para evitar bloqueos
- Las fuentes se persisten en flash y se restauran al arrancar
- También puedes configurar fuentes vía la [interfaz web](./webinterface) en `http://[IP]/datafetcher`


## Cambiar Configuración

Ajusta varias configuraciones relacionadas con la visualización de apps.

| Topic MQTT           | URL HTTP                         | Payload/Body | Método HTTP |
| -------------------- | -------------------------------- | ------------ | ----------- |
| `[PREFIX]/settings`  | `http://[IP]/api/settings`       | JSON         | GET/POST    |

### Propiedades JSON

Puedes ajustar cada propiedad en el objeto JSON según tus preferencias. Incluir una propiedad es opcional.

| Clave         | Tipo                      | Descripción                                                                                        | Rango de Valores                                   | Por defecto |
| ------------- | ------------------------- | -------------------------------------------------------------------------------------------------- | -------------------------------------------------- | ----------- |
| `ATIME`       | number                    | Duración que una app se muestra en segundos.                                                       | Entero positivo                                    | 7       |
| `TEFF`        | number                    | Elige entre efectos de transición de app.                                                          | 0–10                                               | 1       |
| `TSPEED`      | number                    | Tiempo tomado para la transición a la siguiente app en milisegundos.                               | Entero positivo                                    | 500     |
| `TCOL`        | string/array de ints      | Color de texto global.                                                                             | Arreglo RGB o color hex                            | N/A     |
| `TMODE`       | integer                   | Cambia el estilo de la app de hora.                                                                | 0–6                                                | 1       |
| `CHCOL`       | string/array de ints      | Color de encabezado del calendario de la app de hora.                                              | Arreglo RGB o color hex                            |`#FF0000`|
| `CBCOL`       | string/array de ints      | Color del cuerpo del calendario de la app de hora.                                                 | Arreglo RGB o color hex                            |`#FFFFFF`|
| `CTCOL`       | string/array de ints      | Color del texto del calendario en la app de hora.                                                  | Arreglo RGB o color hex                            |`#000000` |
| `WD`          | boolean                   | Habilita o deshabilita la visualización del día de semana.                                         | `true`/`false`                                     | true    |
| `WDCA`        | string/array de ints      | Color del día de semana activo.                                                                    | Arreglo RGB o color hex                            | N/A     |
| `WDCI`        | string/array de ints      | Color del día de semana inactivo.                                                                  | Arreglo RGB o color hex                            | N/A     |
| `BRI`         | number                    | Brillo de la matriz.                                                                               | 0–255                                              | N/A     |
| `ABRI`        | boolean                   | Control automático de brillo.                                                                      | `true`/`false`                                     | N/A     |
| `ATRANS`      | boolean                   | Cambio automático a la siguiente app.                                                              | `true`/`false`                                     | N/A     |
| `CCORRECTION` | array de ints             | Corrección de color para la matriz.                                                                | Arreglo RGB                                        | N/A     |
| `CTEMP`       | array de ints             | Temperatura de color para la matriz.                                                               | Arreglo RGB                                        | N/A     |
| `TFORMAT`     | string                    | Formato de hora para la App de Hora.                                                               | Varía (ver abajo)                                  | N/A     |
| `DFORMAT`     | string                    | Formato de fecha para la App de Fecha.                                                             | Varía (ver abajo)                                  | N/A     |
| `SOM`         | boolean                   | Comenzar la semana en lunes.                                                                       | `true`/`false`                                     | true    |
| `CEL`         | boolean                   | Muestra la temperatura en Celsius (Fahrenheit cuando es false).                                    | `true`/`false`                                     | true    |
| `BLOCKN`      | boolean                   | Bloquea teclas de navegación física (aún envía entrada a MQTT).                                    | `true`/`false`                                     | false   |
| `UPPERCASE`   | boolean                   | Muestra texto en mayúsculas.                                                                       | `true`/`false`                                     | true    |
| `TIME_COL`    | string/array de ints      | Color de texto de la app de hora. Usa 0 para color de texto global.                                | Arreglo RGB o color hex                            | N/A     |
| `DATE_COL`    | string/array de ints      | Color de texto de la app de fecha. Usa 0 para color de texto global.                               | Arreglo RGB o color hex                            | N/A     |
| `TEMP_COL`    | string/array de ints      | Color de texto de la app de temperatura. Usa 0 para color de texto global.                         | Arreglo RGB o color hex                            | N/A     |
| `HUM_COL`     | string/array de ints      | Color de texto de la app de humedad. Usa 0 para color de texto global.                             | Arreglo RGB o color hex                            | N/A     |
| `BAT_COL`     | string/array de ints      | Color de texto de la app de batería. Usa 0 para color de texto global.                             | Arreglo RGB o color hex                            | N/A     |
| `SSPEED`      | integer                   | Modificación de velocidad de desplazamiento.                                                       | Porcentaje de velocidad de desplazamiento original | 100     |
| `NILAYOUT`    | string                    | Disposición de icono para apps nativas (Temperatura, Humedad, Batería). Controla posición del icono.| `"left"`, `"right"`, `"none"`                     | `"left"` |
| `NMODE`       | boolean                   | Habilita modo nocturno — atenuación programada con anulación de color.                             | `true`/`false`                                     | false   |
| `NSTART`      | integer                   | Hora de inicio del modo nocturno en minutos desde medianoche (ej. 1260 = 21:00).                   | 0–1439                                             | 1260    |
| `NEND`        | integer                   | Hora de fin del modo nocturno en minutos desde medianoche (ej. 360 = 06:00).                       | 0–1439                                             | 360     |
| `NBRI`        | integer                   | Brillo durante modo nocturno.                                                                      | 0–255                                              | 5       |
| `NCOL`        | integer                   | Color de texto durante modo nocturno (hex como entero, ej. 16711680 = rojo).                       | Entero RGB                                         | 16711680 |
| `NBTRANS`     | boolean                   | Bloquear auto-transición durante modo nocturno (solo navegación manual).                           | `true`/`false`                                     | true    |
| `TIM`         | boolean                   | Habilita o deshabilita la app nativa de hora (requiere reinicio).                                  | `true`/`false`                                     | true    |
| `DAT`         | boolean                   | Habilita o deshabilita la app nativa de fecha (requiere reinicio).                                 | `true`/`false`                                     | true    |
| `HUM`         | boolean                   | Habilita o deshabilita la app nativa de humedad (requiere reinicio).                               | `true`/`false`                                     | true    |
| `TEMP`        | boolean                   | Habilita o deshabilita la app nativa de temperatura (requiere reinicio).                           | `true`/`false`                                     | true    |
| `BAT`         | boolean                   | Habilita o deshabilita la app nativa de batería (requiere reinicio).                               | `true`/`false`                                     | true    |
| `MATP`        | boolean                   | Habilita o deshabilita la matriz. Similar al endpoint `power` pero sin la animación.               | `true`/`false`                                     | true    |
| `VOL`         | integer                   | Permite establecer el volumen del buzzer.                                                          | 0–30                                               | 25      |
| `OVERLAY`     | string                    | Establece una superposición de efecto global (no puede usarse con superposiciones específicas de app).| Varía (ver abajo)                                | N/A     |
| `BEFF`        | integer                   | Índice de efecto de fondo — se reproduce detrás de cada app y notificación. Usa `-1` para deshabilitar.| `-1` o un índice de efecto válido (ver `/api/effects`)  | -1      |

**Valores de Color**: Pueden ser un arreglo RGB (ej., `[255,0,0]`) o un valor de color hexadecimal válido de 6 dígitos (ej., `"#FF0000"` para rojo).

**Efectos de superposición:**
- "clear"
- "snow"
- "rain"
- "drizzle"
- "storm"
- "thunder"
- "frost"

#### **Formatos de Hora Disponibles:**
| Formato      | Ejemplo    | Descripción                                |
|--------------|------------|--------------------------------------------|
| `%H:%M:%S`   | `13:30:45` | Hora 24h con segundos                      |
| `%l:%M:%S`   | `1:30:45`  | Hora 12h con segundos                      |
| `%H:%M`      | `13:30`    | Hora 24h                                   |
| `%H %M`      | `13.30`    | Hora 24h con dos puntos parpadeantes       |
| `%l:%M`      | `1:30`     | Hora 12h                                   |
| `%l %M`      | `1:30`     | Hora 12h con dos puntos parpadeantes       |
| `%l:%M %p`   | `1:30 PM`  | Hora 12h con indicador AM/PM               |
| `%l %M %p`   | `1:30 PM`  | Hora 12h con dos puntos parpadeantes y AM/PM|

#### **Formatos de Fecha Disponibles:**
| Formato      | Ejemplo    | Descripción              |
|--------------|------------|--------------------------|
| `%d.%m.%y`   | `16.04.22` | Día.Mes.Año (corto)      |
| `%d.%m`      | `16.04`    | Día.Mes                  |
| `%y-%m-%d`   | `22-04-16` | Año-Mes-Día              |
| `%m-%d`      | `04-16`    | Mes-Día                  |
| `%m/%d/%y`   | `04/16/22` | Mes/Día/Año              |
| `%m/%d`      | `04/16`    | Mes/Día                  |
| `%d/%m/%y`   | `16/04/22` | Día/Mes/Año              |
| `%d/%m`      | `16/04`    | Día/Mes                  |
| `%m-%d-%y`   | `04-16-22` | Mes-Día-Año              |

#### **Efectos de Transición Disponibles:**
| Código | Efecto      |
|--------|-------------|
| `0`    | Aleatorio   |
| `1`    | Deslizar    |
| `2`    | Atenuar     |
| `3`    | Zoom        |
| `4`    | Rotar       |
| `5`    | Pixelar     |
| `6`    | Cortina     |
| `7`    | Ondas       |
| `8`    | Parpadeo    |
| `9`    | Recargar    |
| `10`   | Desvanecer  |

## Actualizar

Puedes iniciar la actualización del firmware ya sea a través del botón de actualización en HA o usando lo siguiente:

| Topic MQTT         | URL HTTP                         | Payload/Body | Encabezado HTTP     | Método HTTP |
|--------------------|----------------------------------|--------------|---------------------|-------------|
| `[PREFIX]/doupdate`| `http://[IP]/api/doupdate`       | JSON         | payload/body vacío  | POST        |

#### Reiniciar Svitrix
Si necesitas reiniciar el Svitrix:

| Topic MQTT       | URL HTTP                    | Payload/Body | Método HTTP |
|------------------|-----------------------------|--------------|-------------|
| `[PREFIX]/reboot`| `http://[IP]/api/reboot`    | -            | POST        |

#### Borrar Svitrix
**ADVERTENCIA**: Esta acción formateará la memoria flash y EEPROM pero no modificará la configuración WiFi. Esencialmente sirve como un reset de fábrica.

| Topic MQTT     | URL HTTP                     | Payload/Body | Método HTTP |
|----------------|------------------------------|--------------|-------------|
| `N/A`          | `http://[IP]/api/erase`      | -            | POST        |

#### Limpiar Configuración
**ADVERTENCIA**: Esta acción restablecerá todas las configuraciones de la API de settings. No restablece los archivos flash ni la configuración WiFi.

| Topic MQTT     | URL HTTP                        | Payload/Body | Método HTTP |
|----------------|---------------------------------|--------------|-------------|
| `N/A`          | `http://[IP]/api/resetSettings` | -            | POST        |
