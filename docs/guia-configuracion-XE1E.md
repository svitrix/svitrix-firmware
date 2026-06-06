# Guía de Configuración SVITRIX - XE1E

Guía completa para configurar el firmware SVITRIX en el reloj Ulanzi TC001.

## Requisitos

- Ulanzi TC001 Smart Pixel Clock
- Cable USB (que soporte datos, no solo carga)
- Navegador Chrome o Edge
- Cuenta en WeatherAPI.com (gratis)

---

## 1. Flashear el Firmware

### 1.1 Preparación

**Hardware necesario:**
- Ulanzi TC001 Smart Pixel Clock
- Cable USB-C (debe soportar datos, no solo carga)
- PC con Windows, Mac o Linux

**Software necesario:**
- Google Chrome o Microsoft Edge (Firefox y Safari NO funcionan)
- Drivers CH340 (generalmente se instalan automáticamente)

### 1.2 Qué se Instala

El flasher instala estos componentes en un solo paso:

| Componente | Archivo | Offset | Descripción |
|------------|---------|--------|-------------|
| Bootloader | `bootloader.bin` | 0x1000 | Cargador inicial ESP32 |
| Particiones | `partitions.bin` | 0x8000 | Tabla de particiones |
| Boot App | `boot_app0.bin` | 0xE000 | Aplicación de arranque |
| Firmware | `firmware.bin` | 0x10000 | Firmware SVITRIX |
| LittleFS | `littlefs.bin` | 0x3C0000 | Interfaz web (SPA) + archivos |

### 1.3 Método Online - Flasher XE1E (Recomendado)

1. Abre **Google Chrome** o **Microsoft Edge**
2. Ve al flasher: **https://xe1e.github.io/svitrix-firmware-XE1E/**
3. Conecta el Ulanzi TC001 al PC via USB-C
4. Haz clic en **"Instalar Firmware"**
5. Selecciona el puerto COM del dispositivo
6. **Primera instalación:** Marca la casilla **"Erase device"** para borrar configuración anterior
7. Haz clic en **"Install"**
8. Espera a que termine (2-3 minutos)
9. El dispositivo se reiniciará automáticamente

### 1.4 Proceso de Flasheo Paso a Paso

```
[1/5] Conectando al dispositivo...
[2/5] Borrando flash (si seleccionaste Erase)...
[3/5] Escribiendo bootloader...
[4/5] Escribiendo firmware...
[5/5] Escribiendo sistema de archivos...
[OK]  Instalación completada!
```

### 1.5 Solución de Problemas de Flasheo

| Problema | Solución |
|----------|----------|
| No detecta el dispositivo | Prueba otro cable USB (debe ser de datos) |
| No aparece el puerto COM | Instala drivers CH340: https://sparks.gogo.co.nz/ch340.html |
| Error de conexión | Cierra otras apps que usen el puerto serial |
| Flasheo interrumpido | Desconecta, reconecta y vuelve a intentar |
| Dispositivo no arranca | Vuelve a flashear con "Erase device" marcado |

### 1.6 Modo de Descarga Manual (si falla el automático)

Si el flasher no detecta el dispositivo:

1. Desconecta el USB
2. Mantén presionado el **botón central** del Ulanzi
3. Conecta el USB mientras mantienes presionado
4. Suelta el botón después de 2 segundos
5. El dispositivo entrará en modo bootloader
6. Intenta flashear de nuevo

### 1.7 Flasher Original SVITRIX

Alternativa oficial: https://svitrix.dev/flasher

### 1.8 Método Local con PlatformIO (Desarrolladores)

Requiere: Python, PlatformIO, Node.js

```bash
# Clonar repositorio
git clone https://github.com/XE1E/svitrix-firmware-XE1E.git
cd svitrix-firmware-XE1E

# Compilar firmware
pio run -e ulanzi

# Flashear al dispositivo
pio run -e ulanzi -t upload

# Compilar y subir interfaz web
cd web
npm install
npm run upload
```

### 1.9 Verificar Instalación Exitosa

Después del flasheo exitoso:

1. La pantalla LED mostrará el logo SVITRIX
2. Aparecerá "AP Mode" indicando el modo punto de acceso
3. El dispositivo creará la red WiFi `svitrix_XXXXX`

Si ves estos indicadores, el flasheo fue exitoso.

---

## 2. Conexión WiFi Inicial

Después del flasheo, el dispositivo crea un punto de acceso WiFi:

| Parámetro | Valor |
|-----------|-------|
| Red | `svitrix_XXXXX` |
| Contraseña | `12345678` |

### Pasos:

1. Desde tu celular o PC, conéctate a la red `svitrix_XXXXX`
2. Abre el navegador y ve a: **http://192.168.4.1**
3. Selecciona tu red WiFi doméstica
4. Ingresa la contraseña de tu WiFi
5. Haz clic en **Conectar**
6. El dispositivo se reiniciará

---

## 3. Acceder al Dispositivo

Una vez conectado a tu WiFi:

1. La **dirección IP** aparecerá en la pantalla LED
2. Abre el navegador y ve a: `http://[IP_DEL_DISPOSITIVO]`
3. Accederás a la interfaz web de configuración

> **Tip:** Si no viste la IP, búscala en la lista de dispositivos conectados de tu router.

---

## 4. Configurar Data Fetcher (APIs Externas)

El Data Fetcher permite mostrar datos de APIs públicas sin necesidad de un servidor externo.

### Acceder al Data Fetcher

1. Ve a: `http://[IP]/datafetcher`
2. Haz clic en **"Add Source"** para agregar una fuente de datos

### Limitaciones

- Máximo **8 fuentes** simultáneas
- Respuesta API máxima: **4 KB**
- Solo APIs públicas (sin autenticación personalizada)
- Intervalo mínimo: **60 segundos**

---

## 5. Configurar WeatherAPI.com

### Obtener API Key (Gratis)

1. Regístrate en: https://www.weatherapi.com/signup.aspx
2. Plan gratuito: 1 millón de llamadas/mes
3. Copia tu **API Key** del dashboard
4. Busca el **ID de tu estación** en: https://www.weatherapi.com/docs/

### URL Base

```
https://api.weatherapi.com/v1/current.json?key=TU_API_KEY&q=id:TU_STATION_ID&lang=es
```

Reemplaza:
- `TU_API_KEY` con tu clave de API
- `TU_STATION_ID` con el ID de tu ubicación

---

## 6. Configuraciones de Clima Recomendadas

Agrega estas fuentes en el Data Fetcher (`http://[IP]/datafetcher`):

### 6.1 Temperatura Actual

| Campo | Valor |
|-------|-------|
| **Name** | `temp_ext` |
| **URL** | `https://api.weatherapi.com/v1/current.json?key=TU_API_KEY&q=id:TU_STATION_ID&lang=es` |
| **JSON Path** | `current.temp_c` |
| **Format** | `%.0f°C` |
| **Icon** | `temp` |
| **Color** | `#FF9800` |
| **Interval** | `900` |

### 6.2 Sensación Térmica

| Campo | Valor |
|-------|-------|
| **Name** | `feels` |
| **URL** | (misma URL base) |
| **JSON Path** | `current.feelslike_c` |
| **Format** | `%.0f°C` |
| **Icon** | `temp` |
| **Color** | `#FF5722` |
| **Interval** | `900` |

### 6.3 Humedad Exterior

| Campo | Valor |
|-------|-------|
| **Name** | `hum_ext` |
| **URL** | (misma URL base) |
| **JSON Path** | `current.humidity` |
| **Format** | `%d%%` |
| **Icon** | `hum` |
| **Color** | `#2196F3` |
| **Interval** | `900` |

### 6.4 Condición del Clima

| Campo | Valor |
|-------|-------|
| **Name** | `clima` |
| **URL** | (misma URL base) |
| **JSON Path** | `current.condition.text` |
| **Format** | `%s` |
| **Icon** | (según condición) |
| **Color** | `#FFFFFF` |
| **Interval** | `900` |

### 6.5 Velocidad del Viento

| Campo | Valor |
|-------|-------|
| **Name** | `viento` |
| **URL** | (misma URL base) |
| **JSON Path** | `current.wind_kph` |
| **Format** | `%.0f km/h` |
| **Icon** | `wind` |
| **Color** | `#00BCD4` |
| **Interval** | `900` |

### 6.6 Índice UV

| Campo | Valor |
|-------|-------|
| **Name** | `uv` |
| **URL** | (misma URL base) |
| **JSON Path** | `current.uv` |
| **Format** | `UV %.0f` |
| **Icon** | `sun` |
| **Color** | `#9C27B0` |
| **Interval** | `900` |

---

## 7. Otros Datos Disponibles de WeatherAPI

| JSON Path | Descripción | Formato sugerido |
|-----------|-------------|------------------|
| `current.temp_c` | Temperatura °C | `%.0f°C` |
| `current.temp_f` | Temperatura °F | `%.0f°F` |
| `current.feelslike_c` | Sensación térmica °C | `%.0f°C` |
| `current.humidity` | Humedad % | `%d%%` |
| `current.wind_kph` | Viento km/h | `%.0f km/h` |
| `current.wind_mph` | Viento mph | `%.0f mph` |
| `current.wind_dir` | Dirección viento | `%s` |
| `current.pressure_mb` | Presión mbar | `%.0f mb` |
| `current.precip_mm` | Precipitación mm | `%.1f mm` |
| `current.cloud` | Nubosidad % | `%d%%` |
| `current.uv` | Índice UV | `UV %.0f` |
| `current.vis_km` | Visibilidad km | `%.0f km` |
| `current.condition.text` | Condición texto | `%s` |

---

## 8. Datos de Criptomonedas

El Data Fetcher también puede mostrar precios de criptomonedas usando la API gratuita de CoinGecko.

### 8.1 Bitcoin (BTC)

| Campo | Valor |
|-------|-------|
| **Name** | `btc` |
| **URL** | `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd` |
| **JSON Path** | `bitcoin.usd` |
| **Format** | `$%.0f` |
| **Icon** | `12460` (icono de Bitcoin) |
| **Color** | `#F7931A` |
| **Interval** | `300` |

### 8.2 Ethereum (ETH)

| Campo | Valor |
|-------|-------|
| **Name** | `eth` |
| **URL** | `https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=usd` |
| **JSON Path** | `ethereum.usd` |
| **Format** | `$%.0f` |
| **Icon** | `14630` (icono de Ethereum) |
| **Color** | `#627EEA` |
| **Interval** | `300` |

### 8.3 Otras Criptomonedas

Para obtener precios de otras criptomonedas, cambia el ID en la URL:

| Cripto | ID para URL | JSON Path |
|--------|-------------|-----------|
| Bitcoin | `bitcoin` | `bitcoin.usd` |
| Ethereum | `ethereum` | `ethereum.usd` |
| Solana | `solana` | `solana.usd` |
| Cardano | `cardano` | `cardano.usd` |
| Dogecoin | `dogecoin` | `dogecoin.usd` |

> **Tip:** Encuentra más IDs en [CoinGecko](https://www.coingecko.com/). El ID está en la URL de cada criptomoneda.

---

## 9. Iconos

### Descargar Iconos

1. Ve a: `http://[IP]/settings` (pestaña Iconos)
2. Ingresa el **ID del icono** de LaMetric
3. Haz clic en **Vista Previa** para verlo
4. Haz clic en **Descargar** para guardarlo en `/ICONS/`

Explora iconos disponibles en la [Galería de LaMetric](https://developer.lametric.com/icons).

### Galería de Iconos Guardados

Debajo del selector de iconos verás una **galería** con todos los iconos almacenados en el dispositivo:

- **Vista en grid** — cada icono muestra su imagen y número de ID
- **Eliminar** — pasa el mouse sobre un icono para ver el botón (×)
- **Actualizar** — recarga la lista después de subir iconos manualmente

### Iconos Personalizados

Puedes crear iconos personalizados y subirlos vía el **Administrador de Archivos** a la carpeta `/ICONS/`:

| Formato | Tamaño máximo |
|---------|---------------|
| **GIF** | 32x8 píxeles |
| **JPG** | 8x8 píxeles |

> **Nota:** El renderizador solo soporta GIFs de 8 bits sin transparencia. Si hay glitches, reemplaza píxeles transparentes con negro sólido.

---

## 10. Apps Nativas

El firmware incluye estas apps preinstaladas:

### Apps Básicas (con duración configurable)

| App | Descripción |
|-----|-------------|
| **Time** | Reloj con 7 modos (TMODE 0-6) |
| **Date** | Fecha actual |
| **Temperature** | Sensor interno, con color y offset configurable |
| **Humidity** | Sensor interno, con color configurable |
| **Battery** | Nivel de batería, con color configurable |

### Indicador de Alarmas

El dispositivo muestra un LED indicador en la esquina inferior derecha cuando hay alarmas activas. Este indicador se puede activar/desactivar desde **Settings > Apps > Alarms Indicator**.

### Colores de Apps

En **Settings > Apps** puedes configurar colores para cada app nativa:

- **Time, Date, Temperature, Humidity, Battery:** Selector de color directo

### Rotación Unificada

La página de **Apps** contiene una lista unificada de rotación que controla **todas** las apps y efectos del ciclo:

- **Apps nativas** — Hora, Fecha, Temperatura, Humedad, Batería
- **Apps del clima** — Temp. Exterior, Humedad Exterior, Presión, Calidad del Aire, UV
- **Apps personalizadas** — creadas vía MQTT, HTTP o Data Fetcher
- **Efectos** — efectos visuales independientes (sin texto)

**Configuración por elemento:**

| Campo | Descripción |
|-------|-------------|
| **Toggle** | Activa/desactiva el elemento |
| **Duración** | Tiempo de visualización (0 = usar duración global) |
| **Color** | Color de texto personalizado (0 = usar color por defecto) |

Arrastra cualquier fila para reordenarla. Los cambios se guardan al instante y persisten entre reinicios.

**Agregar elementos:**
- Botón **+ Agregar** — abre un modal para agregar apps nativas, del clima o efectos
- Las apps personalizadas se agregan automáticamente cuando se crean vía MQTT/HTTP/DataFetcher

---

## 11. Alarmas

El dispositivo funciona como **despertador y recordatorio**: hasta 10 alarmas programables, configurables por **botones físicos**, **web** y **MQTT/Home Assistant**. Incluye un RTC (DS1307) que mantiene la hora durante reinicios, así que **las alarmas suenan aunque no haya WiFi**.

### 11.1 Configurar desde la web

1. Ve a: `http://[IP]/autonomous`
2. Cada alarma tiene estos campos editables en línea:

| Campo | Descripción |
|-------|-------------|
| **Hora** | Formato 24h (HH:MM), editable |
| **Días** | Selector de días (S M T W T F S) — recurrente |
| **Una vez** | Recordatorio puntual: suena una sola vez y se desactiva (ignora los días) |
| **Posponer (min)** | Minutos de snooze para esa alarma |
| **Label** | Etiqueta opcional (se muestra al sonar) |
| **Melodía** | RTTTL o nombre de archivo (vacío = solo alerta) |
| **Toggle** | Activar/desactivar |

Arriba se muestra la **próxima alarma**. Si suena una alarma, aparece una alerta con botones de Posponer/Descartar.

### 11.2 Configurar desde los botones

Mantén pulsado el botón central para entrar al menú y navega hasta **ALARMS**:
- Izquierda/Derecha: recorre las alarmas (`07:30 ON`).
- Pulsación corta: entra a editar — la pulsación corta cicla entre **activado / hora / minuto**, e Izquierda/Derecha ajusta el valor.
- Pulsación larga: **guarda** y vuelve a la lista.

(Los días, la etiqueta y la melodía se configuran por web/MQTT.)

### 11.3 Cuando suena una alarma

- La pantalla muestra una alerta roja con la etiqueta y el buzzer reproduce la melodía hasta descartarla.
- **Botones:** Izquierda/Derecha = **Posponer** (usa los minutos de esa alarma); Central (corto o largo) = **Apagar**.

### 11.4 Control por MQTT / Home Assistant

Con la discovery de HA activada aparecen estas entidades:
- `binary_sensor` **Alarm ringing** (sonando)
- `button` **Snooze alarm** y `button` **Dismiss alarm**
- `sensor` **Next alarm** (próxima alarma, HH:MM)

También por topics MQTT: `<prefijo>/alarm/snooze`, `<prefijo>/alarm/dismiss`, `<prefijo>/alarm/add` (JSON con hour, minute, days, oneTime, snoozeMinutes, label, melody).

### 11.5 RTC (Reloj de Tiempo Real)

El Ulanzi TC001 tiene un chip DS1307 con batería de respaldo.

| Comportamiento | Descripción |
|----------------|-------------|
| **Con WiFi** | Sincroniza con NTP y actualiza el RTC |
| **Sin WiFi** | Usa la hora del RTC |
| **Reinicio** | El RTC mantiene la hora |

**Precisión del DS1307:** ±2 ppm típico (~1 segundo/semana). Suficiente para uso normal; se recorrige automáticamente cuando hay WiFi.

### 11.6 API HTTP

| Endpoint | Método | Descripción |
|----------|--------|-------------|
| `/api/alarms` | GET | Lista de alarmas |
| `/api/alarms` | POST | Agregar alarma o control: `{"action": "snooze", "minutes": 5}` o `{"action": "dismiss"}` |
| `/api/alarms` | PUT | Actualizar alarma |
| `/api/alarms?id=N` | DELETE | Eliminar alarma |

### 11.7 Habilitar Indicador de Alarmas

El indicador LED de alarmas está deshabilitado por defecto.

**Desde la web:**
1. Ve a: `http://[IP]` (Settings)
2. En la sección de apps, activa **Alarms Indicator**

**Desde el dispositivo (menú físico):**
1. Mantén presionado el botón central para entrar al menú
2. Navega con izquierda/derecha hasta **APPS**
3. Presiona el botón central para entrar
4. Navega hasta **Alarms**
5. Presiona el botón central para activar/desactivar (ON/OFF)
6. Mantén presionado el botón central para guardar y salir

---

## 12. Efectos de Fondo Disponibles

20 efectos visuales para usar como fondo:

| Categoría | Efectos |
|-----------|---------|
| **Wave** | Pacifica, Plasma, ColorWaves, PlasmaCloud, SwirlIn, SwirlOut |
| **Pattern** | Fade, MovingLine, Radar, Checkerboard, TheaterChase |
| **Particle** | TwinklingStars, Fireworks, Ripple, Matrix, Fire |
| **Game** | Snake, PingPong, BrickBreaker, LookingEyes |

### Weather Overlays

| Overlay | Descripción |
|---------|-------------|
| Rain | Lluvia |
| Drizzle | Llovizna |
| Storm | Tormenta con viento |
| Snow | Nieve |
| Thunder | Relámpagos + lluvia |
| Frost | Escarcha |

---

## 13. Enviar Notificaciones

Las notificaciones muestran mensajes temporales en la pantalla con opciones de personalización.

### Desde la Interfaz Web

1. Ve a: `http://[IP]` (Settings)
2. Busca la sección **"Send Notification"**

| Campo | Descripción |
|-------|-------------|
| **Text** | Mensaje a mostrar (requerido) |
| **Icon** | ID de icono de LaMetric o nombre de archivo en `/ICONS/` |
| **Layout** | Posición del icono: Left o Right (solo si hay icono) |
| **Hold** | Mantiene la notificación hasta presionar Dismiss |
| **Duration** | Duración en segundos (1-60, solo si Hold está desactivado) |
| **Rainbow** | Cicla el texto a través de colores del arcoíris |
| **Color** | Color del texto (cuando Rainbow está desactivado) |
| **Sound** | Nombre de archivo RTTTL en `/MELODIES/` (sin extensión) |
| **RTTTL** | Cadena de melodía RTTTL directa |

### Archivos de Sonido

Los sonidos son melodías en formato RTTTL (Ring Tone Text Transfer Language).

**Ubicación:** `/MELODIES/{nombre}.txt`

**Ejemplo de archivo** (`alarma.txt`):
```
Alarm:d=4,o=5,b=140:8c6,8p,8c6,8p,8c6,8p,8c6
```

**Recursos para melodías RTTTL:**
- [Laub-Home Wiki: RTTTL Songs](https://www.laub-home.de/wiki/RTTTL_Songs)
- [Reproductor RTTTL Online](https://adamonsoon.github.io/rtttl-play/)
- [Editor RTTTL](https://corax89.github.io/esp8266Game/soundEditor.html)

### API HTTP

```bash
# Notificación simple
curl -X POST http://[IP]/api/notify -d '{"text":"Hola mundo!"}'

# Con icono y duración
curl -X POST http://[IP]/api/notify -d '{"text":"Mensaje","icon":"1234","duration":10}'

# Hold (indefinida hasta dismiss)
curl -X POST http://[IP]/api/notify -d '{"text":"Importante","hold":true}'

# Dismiss
curl -X POST http://[IP]/api/notify/dismiss
```

> **Tip:** Si no especificas icono, el texto usa los 32 píxeles completos. Textos largos hacen scroll automáticamente.

---

## 14. Backup y Restauración

El sistema de backup permite guardar y restaurar toda la configuración del dispositivo.

### Acceder al Backup

1. Ve a: `http://[IP]/backup`
2. Verás dos opciones: **Backup** y **Restore**

### Qué se Incluye en el Backup

| Categoría | Contenido |
|-----------|-----------|
| **Archivos (LittleFS)** | Iconos personalizados, melodías RTTTL, apps personalizadas, alarmas |
| **Configuración (NVS)** | Brillo, colores, WiFi, MQTT, Weather API, config de apps, tiempo, audio, display |

### Crear un Backup

1. Haz clic en **"Download Backup"**
2. Espera mientras se recopilan archivos y configuración
3. Se descargará un archivo `svitrix-backup.json`

### Restaurar un Backup

1. Haz clic en **"Choose File"** y selecciona tu archivo de backup
2. El sistema restaurará:
   - Primero los archivos (iconos, melodías, apps)
   - Luego la configuración
3. El dispositivo se reiniciará automáticamente

### Formato del Backup

```json
{
  "version": 2,
  "files": {
    "/ICONS/miicono.jpg": "base64...",
    "/MELODIES/alarma.txt": "base64..."
  },
  "settings": {
    "BRI": 120,
    "TCOL": 16777215,
    "TIME_COL": 0,
    ...
  }
}
```

### Compatibilidad

| Versión | Contenido | Compatibilidad |
|---------|-----------|----------------|
| **v1** | Solo archivos | ✅ Se puede restaurar en firmware actual |
| **v2** | Archivos + configuración | ✅ Formato actual |

> **Tip:** Haz un backup antes de actualizar el firmware o hacer cambios importantes. Los backups v1 (solo archivos) siguen siendo compatibles.

---

## 15. Solución de Problemas

### El flasher no detecta el dispositivo
- Prueba otro cable USB (debe soportar datos)
- Prueba otro puerto USB
- Reinicia el navegador

### No puedo conectarme al WiFi del dispositivo
- Asegúrate de estar cerca del dispositivo
- Reinicia el Ulanzi manteniendo presionado el botón central

### La IP no aparece en pantalla
- Revisa la lista de dispositivos en tu router
- El hostname es `svitrix_XXXXXX`

### El Data Fetcher no muestra datos
- Verifica que la URL sea correcta
- Verifica que el JSON Path sea exacto
- Revisa que el intervalo sea >= 60 segundos

---

## 16. Enlaces Útiles

- **Documentación oficial:** https://svitrix.dev
- **Repositorio original:** https://github.com/svitrix/svitrix-firmware
- **WeatherAPI docs:** https://www.weatherapi.com/docs/
- **Iconos LaMetric:** Disponibles en la interfaz web

---

## 17. Configurar GitHub Pages (Flasher Propio)

Para tener tu propio flasher web:

1. Ve a tu repositorio: https://github.com/XE1E/svitrix-firmware-XE1E
2. Click en **Settings** (Configuración)
3. En el menú lateral, click en **Pages**
4. En "Build and deployment":
   - **Source:** Deploy from a branch
   - **Branch:** `main`
   - **Folder:** `/flasher`
5. Click en **Save**
6. Espera 1-2 minutos

Tu flasher estará disponible en:
```
https://xe1e.github.io/svitrix-firmware-XE1E/
```

---

## 18. Información del Fork

| Campo | Valor |
|-------|-------|
| **Repositorio** | https://github.com/XE1E/svitrix-firmware-XE1E |
| **Flasher** | https://xe1e.github.io/svitrix-firmware-XE1E/ |
| **Basado en** | svitrix/svitrix-firmware |
| **Autor fork** | XE1E |

---

## 19. Git - Flujo de Trabajo del Fork

Este repositorio es un fork del proyecto original SVITRIX.

### Estructura de Remotos

```
┌─────────────────────────────────────┐
│  upstream (svitrix/svitrix-firmware)│  ← Repo original (solo lectura)
└─────────────────────────────────────┘
                ↓ git pull upstream main
┌─────────────────────────────────────┐
│  Tu PC (local)                      │
│  main → trackea origin/main         │
└─────────────────────────────────────┘
                ↓ git push
┌─────────────────────────────────────┐
│  origin (XE1E/svitrix-firmware-XE1E)│  ← Tu fork (lectura + escritura)
└─────────────────────────────────────┘
```

### Comandos Básicos

| Comando | Resultado |
|---------|-----------|
| `git push` | Push a tu fork (origin) |
| `git pull` | Pull de tu fork (origin) |
| `git pull upstream main` | Sincronizar con el repo original |

### Sincronizar tu Fork con el Original

```bash
git pull upstream main
git push
```

### Pull Requests

- Tus cambios se quedan en tu fork a menos que hagas un **Pull Request** al repo original
- Tu fork es independiente — puedes experimentar sin afectar el repo original
- Para contribuir cambios al proyecto original, crea un PR desde tu fork

### Conflictos de Merge

Los conflictos ocurren cuando tú y el repo original modifican las mismas líneas del mismo archivo.

| Situación | ¿Conflicto? |
|-----------|-------------|
| Tú modificas un archivo, upstream no lo toca | No |
| Upstream modifica un archivo, tú no lo tocas | No |
| Ambos modifican el mismo archivo en **líneas diferentes** | No (merge automático) |
| Ambos modifican el mismo archivo en **las mismas líneas** | **Sí** |

#### Qué pasa cuando hay conflicto

```bash
git pull upstream main
# Auto-merging archivo.cpp
# CONFLICT (content): Merge conflict in archivo.cpp
```

Git marca el archivo con:

```cpp
<<<<<<< HEAD
// Tu código
=======
// Código de upstream
>>>>>>> upstream/main
```

Debes:
1. Abrir el archivo y decidir qué código mantener
2. Eliminar los marcadores `<<<<<<<`, `=======`, `>>>>>>>`
3. Guardar el archivo
4. `git add archivo.cpp`
5. `git commit -m "fix: resolve merge conflict"`

#### Estrategias para Minimizar Conflictos

1. **Archivos separados** — Haz tus cambios en archivos que upstream no toca (ej: `guia-configuracion-XE1E.md`)
2. **Sync frecuente** — Ejecuta `git pull upstream main` seguido para tener conflictos pequeños y manejables
3. **Cambios modulares** — Prefiere agregar código nuevo en lugar de modificar código existente

### Archivos con Conflicto Permanente

Estos archivos tienen cambios específicos para tu fork y **siempre causarán conflicto** al sincronizar con upstream:

| Archivo | Tu versión | Upstream |
|---------|------------|----------|
| `docs/.vitepress/config.mts` | `base: '/svitrix-firmware-XE1E/'` | `base: '/svitrix-firmware/'` |
| `docs/flasher.md` | iframe: `/svitrix-firmware-XE1E/...` | iframe: `/svitrix-firmware/...` |
| `docs/uk/flasher.md` | iframe: `/svitrix-firmware-XE1E/...` | iframe: `/svitrix-firmware/...` |

#### Cómo resolver estos conflictos

Cuando veas conflicto en estos archivos después de `git pull upstream main`:

```bash
# Git mostrará:
# CONFLICT (content): Merge conflict in docs/.vitepress/config.mts
```

1. Abre el archivo en conflicto
2. Busca las líneas con `base:` o `iframe src=`
3. **Siempre mantén tu versión** con `/svitrix-firmware-XE1E/`
4. Elimina los marcadores de conflicto
5. `git add <archivo>` y `git commit`

**Importante:** Si upstream actualiza otras partes de estos archivos (no el base path), acepta esos cambios pero mantén tu base path.

### Sincronización Automática

Este fork tiene un GitHub Action que sincroniza automáticamente con el repo original:

| Configuración | Valor |
|---------------|-------|
| **Frecuencia** | Diario a las 6:00 UTC |
| **Workflow** | `.github/workflows/sync-upstream.yml` |
| **Ejecución manual** | Actions > "Sync Upstream" > "Run workflow" |

**Qué hace:**
1. Verifica si hay commits nuevos en upstream
2. Si hay cambios → hace merge automático y push
3. Si hay conflicto → falla y notifica (no rompe nada, requiere intervención manual)

**Ver estado:**
```
https://github.com/XE1E/svitrix-firmware-XE1E/actions/workflows/sync-upstream.yml
```
