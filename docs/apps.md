# Apps Nativas
## Hora
La app nativa de Hora ofrece amplias opciones de personalización. Casi todo puede configurarse vía la API, y la mayoría de las configuraciones también pueden ajustarse a través de la app SVITRIX.
El formato de hora puede personalizarse según tus preferencias. Por defecto, está configurado a `HH:mm`.
Si el formato seleccionado no cabe en la pantalla, automáticamente volverá a este valor por defecto.
Podrás notar algunas líneas en la parte inferior de la pantalla. Estas líneas representan los días de la semana, con el día actual resaltado más brillante. También es posible desactivar esta barra de días de semana.
También puedes personalizar los colores para el icono del calendario, la barra de días de semana y también el color del texto con la [API de Settings](./api#change-settings) o la app móvil.

La configuración `TMODE` determina el diseño y estilo de la App de Hora.

 **TMODE=0**
<img src="./assets/TMODE0.png" style="max-height:70px;">
Muestra la hora junto con una barra de días de semana en la parte inferior de la pantalla.

**TMODE=1**
<img src="./assets/TMODE1.png" style="max-height:70px;">
Muestra la hora con la barra de días de semana en la parte inferior y un cuadro de calendario que resalta el día actual del mes.

**TMODE=2**
<img src="./assets/TMODE2.png" style="max-height:70px;">
Similar a `TMODE=1`, pero coloca la barra de días de semana en la parte superior.

**TMODE=3**
<img src="./assets/TMODE3.png" style="max-height:70px;">
Muestra la hora con la barra de días de semana en la parte inferior y un icono de calendario diferente.

**TMODE=4**
<img src="./assets/TMODE4.png" style="max-height:70px;">
Similar a `TMODE=3`, pero coloca la barra de días de semana en la parte superior.

**TMODE=5**
<img src="./assets/TMODE5.png" style="max-height:70px;">
Introduce una visualización de "hora grande", utilizando una fuente grande para la hora.
Si existe un GIF de 32x8 llamado `bigtime.gif` en el directorio raíz, se reproducirá en el fondo.
Por favor nota: Una vez que el GIF se muestra en modo BigTime, no puede reemplazarse directamente porque el archivo está en uso.
Para reemplazar el icono, cambia el modo a TMODE primero, y luego podrás actualizar el GIF.
Si no se encuentra ningún GIF, se usa el color de texto global.

[Puedes obtener algunos ejemplos de GIFs bigtime aquí](https://github.com/XE1E/svitrix-firmware-XE1E/tree/main/docs/assets/bigtime-gifs).

**TMODE=6**
<img src="./assets/TMODE6.png" style="max-height:70px;">
Muestra la hora en **formato binario**:
La fila superior muestra la hora, la fila del medio muestra los minutos, y la fila inferior muestra los segundos.
Cada fila tiene seis puntos, donde los puntos encendidos representan "1" binario y los puntos blancos representan "0" binario.
Para leer la hora, convierte los puntos encendidos en cada fila a un número decimal.

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

---
## Fecha
<img src="./assets/DATE.png" style="max-height:100px;">

La app de Fecha muestra la fecha actual por supuesto. Hay varios formatos de fecha 'DFORMAT' que puedes elegir:

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

---
## Temperatura

<img src="./assets/TEMP.png" style="max-height:100px;">

La App de Temperatura muestra la lectura actual del sensor de temperatura integrado.
Sin embargo, debido a la ubicación del sensor dentro de la carcasa, la medición puede no ser completamente precisa.
Factores como la placa de alimentación, la matriz LED, el brillo, el color y el número de píxeles encendidos pueden afectar la lectura de temperatura.
Para una medición más precisa, puedes utilizar el [dev.json](./dev) para calibrar la temperatura con la clave `temp_offset`.

---
## Humedad

<img src="./assets/HUM.png" style="max-height:100px;">

La app de Humedad muestra la lectura actual del sensor de humedad integrado.
Sin embargo, debido a la ubicación del sensor dentro de la carcasa, la medición puede no ser completamente precisa.
Factores como la placa de alimentación, la matriz LED, el brillo, el color y el número de píxeles encendidos pueden afectar la lectura de humedad.
Para una medición más precisa, puedes utilizar el [dev.json](./dev) para calibrar la humedad con la clave `hum_offset`.

---
## Batería

<img src="./assets/BAT.png" style="max-height:100px;">

La App de Batería muestra el nivel de carga actual de la batería integrada.
Debido a diferencias en lotes de baterías y la degradación de la batería barata con el tiempo, puede que necesites calibrarla manualmente.

1. Usa la [API de Estado](./api#status-retrieval) para obtener las lecturas de `bat_raw`.
2. Abre el archivo [dev.json](./dev) para configurar los valores `min_battery` y `max_battery`:
   - **`min_battery`**: Ingresa el valor `bat_raw` cuando la batería está vacía.
   - **`max_battery`**: Ingresa el valor `bat_raw` cuando la batería está completamente cargada.


---
## Apps del Clima

Las apps del clima también son **nativas** (integradas en el firmware), pero requieren configurar la **API del Clima** (una clave de [weatherapi.com](https://www.weatherapi.com/) + ubicación) en **Settings → API del Clima**. Una vez activas, comparten el mismo ciclo de rotación que las demás apps y se pueden reordenar libremente (ver *Orden de Apps* en el manual de la interfaz web).

| App | Descripción |
|-----|-------------|
| **Temp. Exterior** | Temperatura exterior. Color configurable; respeta la escala Celsius/Fahrenheit. |
| **Humedad Exterior** | Humedad relativa exterior (%). Color configurable. |
| **Presión** | Presión atmosférica (mb/hPa). Color configurable. |
| **Calidad del Aire (AQI)** | Índice de calidad del aire (US EPA, 1–6). Color automático según el nivel (verde → amarillo → naranja → rojo) o color fijo. |
| **Índice UV** | Índice ultravioleta (0–11+). Color automático según el nivel o color fijo. |

Cada app del clima tiene su propia **duración** configurable y se activa/desactiva de forma individual. El intervalo de actualización de los datos se ajusta en la sección de API del Clima.

---
# Apps Personalizadas

Además de las apps nativas, SVITRIX está diseñado para integrarse perfectamente con tu ecosistema de hogar inteligente, se pueden crear aplicaciones adicionales usando peticiones MQTT o HTTP.

::: warning
En SVITRIX, el término 'Apps Personalizadas' no se refiere a aplicaciones tradicionales de smartphone que descargas e instalas. En cambio, en SVITRIX, las CustomApps funcionan más como páginas dinámicas que rotan dentro del ciclo de rotación de apps de la pantalla. Estas páginas no almacenan ni ejecutan su propia lógica; en cambio, muestran contenido que se envía desde un sistema externo, como un hogar inteligente. Este contenido debe transmitirse usando protocolos MQTT o HTTP vía la [API de CustomApp](./api#custom-apps-and-notifications).
Es importante notar que toda la lógica para gestionar el contenido mostrado en estas CustomApps necesita ser manejada por tu sistema externo. SVITRIX solo proporciona la plataforma para mostrar la información. Tienes la flexibilidad de actualizar el contenido mostrado en tus CustomApps en tiempo real en cualquier momento, convirtiéndolo en una herramienta versátil para mostrar información personalizada en tu configuración de hogar inteligente.
:::

Hay numerosos beneficios de este enfoque:

- **Personalización:** Personaliza cada aplicación según tus preferencias y necesidades.
- **Flexibilidad:** Desarrolla tus propias aplicaciones sin necesidad de modificar el firmware.
- **Gestión eficiente de recursos:** Ahorra valioso espacio de memoria flash en el módulo ESP.
- **Adaptabilidad:** No es necesario reescribir el firmware si una API sufre cambios.

Puedes usar cualquier sistema que te guste que sea capaz de construir cadenas JSON y enviarlas a un topic MQTT.

## SVITRIX FLOWS
Este es tu centro principal para compartir y descubrir automatizaciones de SVITRIX, también conocidas como Apps personalizadas para varios servicios.
Mejora tu experiencia SVITRIX, intercambia ideas e inspírate. ¡Vamos a dar vida a nuestras automatizaciones creativas juntos!
No es necesario iniciar sesión, ni siquiera para crear nuevos flows. Como creador obtendrás un enlace con el que siempre podrás editar tu flow. ¡Guárdalo bien!
Puedes subir tus iconos a tu flow, y los usuarios pueden copiarlos directamente a su SVITRIX con un clic!
Los nuevos flows son moderados regularmente.
https://flows.svitrix.dev/


## Ejemplo de Flow con Node-RED
[Node-RED](https://nodered.org/) sirve como una solución de software ideal para crear estas aplicaciones.
Está disponible como un programa independiente o como un plugin para Home Assistant y ioBroker, permitiéndote mejorar aún más las capacidades de tu sistema SVITRIX.

Aquí hay una demo, por favor presiona el triángulo para desplegar.

<details>
  <summary>Ejemplo para agregar una App de Youtube como flow de NodeRED</summary>
  <pre><code class="language-json">
[
  {
    "id": "2a59d30d07abe14f",
    "type": "group",
    "z": "54b42d8d.cda474",
    "style": {
      "stroke": "#999999",
      "stroke-opacity": "1",
      "fill": "none",
      "fill-opacity": "1",
      "label": true,
      "label-position": "nw",
      "color": "#a4a4a4"
    },
    "nodes": [
      "f0f17299.3736c",
      "dc7878f9.4756c8",
      "f234aae371d72680",
      "555bb8624b88c9c3",
      "69c388146e28049d",
      "a349ade5a57f7537"
    ],
    "x": 34,
    "y": 39,
    "w": 892,
    "h": 122
  },
  {
    "id": "f0f17299.3736c",
    "type": "inject",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "",
    "props": [],
    "repeat": "3600",
    "crontab": "",
    "once": true,
    "onceDelay": 0.1,
    "topic": "",
    "x": 130,
    "y": 120,
    "wires": [
      [
        "a349ade5a57f7537"
      ]
    ]
  },
  {
    "id": "dc7878f9.4756c8",
    "type": "http request",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "",
    "method": "GET",
    "ret": "obj",
    "paytoqs": "query",
    "url": "https://youtube.googleapis.com/youtube/v3/channels",
    "tls": "",
    "persist": false,
    "proxy": "",
    "insecureHTTPParser": false,
    "authType": "",
    "senderr": false,
    "headers": [],
    "x": 430,
    "y": 120,
    "wires": [
      [
        "f234aae371d72680"
      ]
    ]
  },
  {
    "id": "f234aae371d72680",
    "type": "function",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "parser",
    "func": "var json = msg.payload;\nvar subscriberCount = json.items[0].statistics.subscriberCount;\n\nmsg.payload = { \"text\": subscriberCount, \"icon\": 5029};\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "initialize": "",
    "finalize": "",
    "libs": [],
    "x": 590,
    "y": 120,
    "wires": [
      [
        "555bb8624b88c9c3"
      ]
    ]
  },
  {
    "id": "555bb8624b88c9c3",
    "type": "mqtt out",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "",
    "topic": "ulanzi/custom/youtube",
    "qos": "",
    "retain": "",
    "respTopic": "",
    "contentType": "",
    "userProps": "",
    "correl": "",
    "expiry": "",
    "broker": "346df2a95aac5785",
    "x": 800,
    "y": 120,
    "wires": []
  },
  {
    "id": "69c388146e28049d",
    "type": "comment",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "Youtube Follower",
    "info": "Just enter your channelID and Youtube API key in the \"Data\" node  and set your SVITRIX MQTT prefix.\nUses Icon 5029 (LM)",
    "x": 140,
    "y": 80,
    "wires": []
  },
  {
    "id": "a349ade5a57f7537",
    "type": "function",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "Data",
    "func": "msg.payload = { \"id\": \"UCpGLALzRO0uaasWTsm9M99w\", \"key\": \"XXX\", \"part\":\"statistics\"}\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "initialize": "",
    "finalize": "",
    "libs": [],
    "x": 270,
    "y": 120,
    "wires": [
      [
        "dc7878f9.4756c8"
      ]
    ]
  },
  {
    "id": "346df2a95aac5785",
    "type": "mqtt-broker",
    "name": "",
    "broker": "localhost",
    "port": "1883",
    "clientid": "",
    "autoConnect": true,
    "usetls": false,
    "protocolVersion": "4",
    "keepalive": "60",
    "cleansession": true,
    "birthTopic": "",
    "birthQos": "0",
    "birthPayload": "",
    "birthMsg": {},
    "closeTopic": "",
    "closeQos": "0",
    "closePayload": "",
    "closeMsg": {},
    "willTopic": "",
    "willQos": "0",
    "willPayload": "",
    "willMsg": {},
    "userProps": "",
    "sessionExpiry": ""
  }
]
  </code></pre>
</details>

Este flow de Node-RED obtiene y muestra el conteo de suscriptores de un canal de YouTube especificado en un dispositivo SVITRIX. El flow consiste en los siguientes nodos:

1. **Inject**: Este nodo dispara el flow periódicamente (cada hora) o manualmente.
2. **Data (Function)**: Este nodo contiene el ID del canal de YouTube y la API key de YouTube. Reemplaza "XXX" con tu API key de YouTube e ID de Youtube. El nodo construye un payload que contiene el ID del canal, API key y estadísticas requeridas y lo envía al nodo "HTTP request".
3. **HTTP request**: Este nodo envía una petición GET a la API de YouTube para obtener las estadísticas del canal. La respuesta se devuelve como un objeto JavaScript y se pasa al nodo "parser".
4. **parser (Function)**: Este nodo extrae el conteo de suscriptores de las estadísticas del canal recibidas y construye un payload que contiene el conteo y un icono (Icono 5029). El payload se envía al nodo "MQTT out".
5. **MQTT out**: Este nodo publica el payload en el topic MQTT "ulanzi/custom/youtube" en un broker MQTT local. También debes cambiar el topic en este nodo para que coincida con tu prefijo MQTT.
6. **Comment (Youtube Follower)**: Este nodo contiene información adicional sobre el flow. No afecta la funcionalidad del flow.

Para usar este flow, reemplaza "XXX" en el nodo "Data" con tu API key de YouTube y asegúrate de que la configuración del broker MQTT en el nodo "MQTT out" sea correcta.
El flow entonces obtendrá el conteo de suscriptores del canal de YouTube especificado y lo mostrará en tu dispositivo SVITRIX junto con el icono.
Este Flow usa el icono 5029 de LM (Solo descárgalo desde la interfaz web de SVITRIX). Puedes cambiar el icono en el flow por tu favorito.
