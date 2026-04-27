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

## 8. Apps Nativas (Sin configuración)

El firmware incluye estas apps preinstaladas:

| App | Descripción |
|-----|-------------|
| **Time** | Reloj con 7 modos (TMODE 0-6) |
| **Date** | Fecha actual |
| **Temperature** | Sensor interno del dispositivo |
| **Humidity** | Sensor interno del dispositivo |
| **Battery** | Nivel de batería |

---

## 9. Efectos de Fondo Disponibles

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

## 10. Solución de Problemas

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

## 11. Enlaces Útiles

- **Documentación oficial:** https://svitrix.dev
- **Repositorio original:** https://github.com/svitrix/svitrix-firmware
- **WeatherAPI docs:** https://www.weatherapi.com/docs/
- **Iconos LaMetric:** Disponibles en la interfaz web

---

## 12. Configurar GitHub Pages (Flasher Propio)

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

## 13. Información del Fork

| Campo | Valor |
|-------|-------|
| **Repositorio** | https://github.com/XE1E/svitrix-firmware-XE1E |
| **Flasher** | https://xe1e.github.io/svitrix-firmware-XE1E/ |
| **Basado en** | svitrix/svitrix-firmware |
| **Autor fork** | XE1E |

---

## 14. Git - Flujo de Trabajo del Fork

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
