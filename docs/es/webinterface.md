# Interfaz Web

SVITRIX usa una moderna Aplicación de Página Única (SPA) construida con Preact como su interfaz web. La SPA se sirve desde el sistema de archivos LittleFS del dispositivo y se comunica con el firmware vía API REST.

Una vez que SVITRIX está conectado a tu red WiFi, accede a la interfaz web ingresando la dirección IP del dispositivo en tu navegador (puerto 80). La dirección IP se muestra en la matriz en cada arranque.

## Páginas

| Página | Ruta | Descripción |
|--------|------|-------------|
| **Pantalla** | `/` | Vista en vivo de la matriz LED 32x8 con navegación de apps (anterior/siguiente), descarga PNG y grabación GIF. |
| **Configuración** | `/settings` | Configuración del dispositivo dividida en secciones independientes, cada una con su propio botón Guardar: WiFi, Red, MQTT, NTP/Zona horaria, Autenticación, Pantalla, Apps, Hora y Fecha, Sonido, Enviar Notificación y Selector de Iconos. Incluye alternador de tema oscuro/claro. |
| **Data Fetcher** | `/datafetcher` | Configura fuentes de datos HTTP externas que automáticamente obtienen y muestran datos en la matriz. Ver [Data Fetcher](./datafetcher) para detalles. |
| **Archivos** | `/files` | Administrador de archivos integrado para navegar, subir, descargar, editar y eliminar archivos en el dispositivo (iconos, melodías, apps personalizadas, paletas). |
| **Respaldo** | `/backup` | Descarga todos los archivos del dispositivo como un respaldo JSON, o restaura desde un respaldo previamente descargado. |
| **Actualizar** | `/update` | Sube firmware (.bin) para actualización OTA. El dispositivo se reinicia automáticamente después de una subida exitosa. |

## Primera Configuración (Modo AP)

Cuando SVITRIX no puede conectarse a una red WiFi guardada, crea su propio punto de acceso:

| Parámetro | Valor |
|-----------|-------|
| Nombre de red | `svitrix_XXXXX` |
| Contraseña | `12345678` |

Conéctate a esta red y abre **http://192.168.4.1** en tu navegador. Aparecerá una página mínima de configuración WiFi con escaneo de redes y conexión. Después de conectarte a tu WiFi doméstico, el dispositivo se reinicia y la SPA completa estará disponible.

## Despliegue de la SPA

La interfaz web se almacena por separado del firmware en la partición del sistema de archivos LittleFS. Después de flashear el firmware, sube los archivos de la SPA al dispositivo:

```bash
cd web && npm run upload
```

Esto compila la SPA y la sube al directorio raíz de LittleFS del dispositivo. El bundle de la SPA es aproximadamente 18 KB (comprimido con gzip) e incluye las 6 páginas.

::: tip
Una vez subida, la SPA persiste entre actualizaciones de firmware. Solo necesitas volver a subir la SPA cuando la interfaz web misma se actualice.
:::

## Guía de Configuración

La página de Configuración está organizada en secciones independientes. Cada sección tiene su propio botón **Guardar** — solo guardas lo que cambiaste.

Un **alternador de tema oscuro/claro** (☀/☽) está disponible en la esquina superior derecha de la barra de navegación. Tu preferencia se guarda en el navegador.

### Barra de Estadísticas

Una barra de solo lectura en la parte superior mostrando información del dispositivo en tiempo real: versión del firmware, RAM libre, intensidad de señal WiFi, luz ambiental (lux), tiempo de actividad, temperatura, humedad y brillo actual.

### WiFi

Escanea redes disponibles, selecciona una e ingresa la contraseña para conectar. El dispositivo se reinicia después de conectar.

### Red

Habilita **IP Estática** para configurar una dirección IP fija, puerta de enlace, subred y servidor DNS en lugar de DHCP.

### MQTT

Conéctate a un broker MQTT para integración con Home Assistant y control remoto:
- **Broker** — hostname o IP de tu broker MQTT
- **Puerto** — por defecto 1883
- **Usuario / Contraseña** — credenciales del broker
- **Prefijo** — prefijo del topic MQTT (por defecto: ID del dispositivo)
- **Home Assistant Discovery** — habilita auto-descubrimiento de entidades del dispositivo en HA

### NTP y Zona Horaria

- **Servidor NTP** — servidor de tiempo (por defecto: `pool.ntp.org`)
- **Zona horaria** — cadena de zona horaria POSIX (encuentra la tuya en [posix_tz_db](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv))

### Autenticación

Ver la sección [Autenticación](#autenticación) abajo.

### Modo Nocturno

Programa un modo de bajo brillo y un solo color para uso nocturno (ej. reloj de dormitorio):

- **Habilitar Modo Nocturno** — activa/desactiva la función
- **Inicio / Fin** — rango de tiempo (ej. 21:00 a 06:00, soporta cruzar medianoche)
- **Brillo Nocturno** — brillo de pantalla durante horas nocturnas (1–50)
- **Color Nocturno** — color de texto durante modo nocturno (por defecto: rojo — más fácil para los ojos)
- **Bloquear Auto-Transición** — cuando está marcado, las apps no ciclan automáticamente; usa botones para navegar

Durante el tiempo programado, la pantalla reduce su brillo a la configuración establecida y todo el texto se renderiza en el color nocturno elegido. Cuando termina la ventana de tiempo, la configuración normal se restaura automáticamente.

### Pantalla

- **Energía de Matriz** — enciende/apaga la matriz LED
- **Brillo Automático** — ajusta automáticamente el brillo basado en luz ambiental
- **Brillo** — nivel de brillo manual (0–255)
- **Gamma** — curva de corrección gamma (0.5–3.0)
- **Mayúsculas** — fuerza todo el texto a mayúsculas
- **Color de Texto** — color de texto por defecto para todas las apps
- **Corrección de Color / Temperatura de Color** — ajuste avanzado de color LED

### Apps

Activa/desactiva apps integradas. Cada app tiene opciones de personalización:

**Apps básicas:**
- **Hora** — reloj digital con duración configurable
- **Fecha** — fecha con duración configurable

**Apps con color:**
- **Temperatura** — sensor interno, con selector de color, escala Celsius/Fahrenheit y offset
- **Humedad** — sensor interno, con selector de color
- **Batería** — nivel de batería, con selector de color

**Indicador de alarmas:**
- **Alarms Indicator** — muestra un LED en la esquina inferior derecha cuando hay alarmas activas

Comportamiento de apps:
- **Duración de App** — cuánto tiempo se muestra cada app antes de cambiar (1–60s)
- **Auto Transición** — cicla automáticamente a través de las apps
- **Efecto de Transición** — efecto visual al cambiar apps (Ninguno, Deslizar, Atenuar, Zoom, etc.)
- **Velocidad de Transición** — qué tan rápido reproduce la animación de transición (100–2000ms)
- **Velocidad de Desplazamiento** — velocidad de desplazamiento de texto para texto largo
- **Bloquear Navegación** — deshabilita navegación con botones entre apps

### Hora y Fecha

- **Formato de Hora / Formato de Fecha** — cadenas de formato strftime (ej., `%H:%M`, `%d.%m.%y`)
- **Modo de Hora** — estilo de visualización: Texto Plano, Calendario, Calendario Arriba, Calendario Alt, Dígitos Grandes o Binario
- **Comenzar en Lunes** — la semana comienza en lunes en lugar de domingo
- **Celsius** — muestra temperatura en °C (apagado = °F)
- **Color de Hora / Fecha** — colores individuales para apps de hora y fecha
- **Mostrar Día de Semana** — muestra barra indicadora de día de semana
- **Color Día Activo / Inactivo** — colores para puntos de día de semana
- **Color Encabezado / Texto / Cuerpo de Calendario** — colores para el cuadro de calendario

### Sonido

- **Sonido Habilitado** — habilita/deshabilita el buzzer
- **Volumen** — nivel de volumen del buzzer (0–30)

### Enviar Notificación

Envía un mensaje único a la pantalla:
- **Texto** — mensaje a mostrar (requerido)
- **Icono** — ID de icono o nombre de archivo de `/ICONS/`
- **Disposición** — posiciona el icono a la izquierda o derecha (solo visible cuando hay icono)
- **Hold (indefinido)** — mantiene la notificación hasta presionar Dismiss o el botón central
- **Duración** — cuánto tiempo se muestra la notificación (1–60s, solo cuando Hold está desactivado)
- **Arcoíris** — cicla el texto a través de colores del arcoíris
- **Color** — color de texto (cuando arcoíris está apagado)
- **Sonido** — nombre de archivo de melodía RTTTL en `/MELODIES/` (sin extensión)
- **RTTTL** — cadena de melodía en [formato RTTTL](https://en.wikipedia.org/wiki/Ring_Tone_Text_Transfer_Language) directo

::: tip
Si no especificas icono, el texto usa los 32 píxeles completos de la pantalla. Los textos largos hacen scroll automáticamente.
:::

Ver [Sonidos](./sounds) para crear archivos de melodía.

### Selector de Iconos

Descarga iconos de la [biblioteca de iconos LaMetric](https://developer.lametric.com/icons):
1. Ingresa el número de ID del icono
2. Haz clic en **Vista Previa** para verlo
3. Haz clic en **Descargar** para guardarlo en la carpeta `/ICONS/` del dispositivo

### Acciones

- **Guardar Configuración de Pantalla** — guarda todas las configuraciones relacionadas con la pantalla de una vez (alternativa si se omitieron los botones individuales de Guardar)
- **Restablecer Valores** — restaura todas las configuraciones a valores de fábrica (requiere confirmación)
- **Reiniciar** — reinicia el dispositivo (requiere confirmación)

## Autenticación

Puedes establecer un nombre de usuario y contraseña en Configuración → Autenticación. Cuando está configurado, cada página, llamada API y la app SVITRIX requerirán estas credenciales. Deja ambos campos vacíos para deshabilitar la autenticación.

::: warning
No pierdas tus credenciales de autenticación — de lo contrario necesitarás hacer un reset de fábrica del dispositivo.
:::
