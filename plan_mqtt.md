# Plan: Mejoras MQTT

Análisis y propuesta de mejoras para la integración MQTT y Home Assistant.

## Estado Actual

### Entidades HA (25 total)

| Tipo | Cantidad | Entidades |
|------|----------|-----------|
| **HALight** | 4 | Matrix (brillo+RGB), Indicator 1/2/3 (RGB) |
| **HASelect** | 2 | BriMode (Manual/Auto), transEffect (11 transiciones) |
| **HAButton** | 4 | dismiss, nextApp, prevApp, doUpdate |
| **HASwitch** | 1 | transition (auto-transición toggle) |
| **HASensor** | 10-11 | curApp, myOwnID, temp, hum, lux, signal, version, ram, uptime, ipAddr, battery* |
| **HABinarySensor** | 3 | btnleft, btnmid, btnright |

*battery solo en build ULANZI

### Comandos MQTT (20 topics)

| Suffix | Acción | HTTP equivalente |
|--------|--------|------------------|
| `/notify` | Generar notificación | `POST /api/notify` |
| `/notify/dismiss` | Descartar notificación | `ANY /api/notify/dismiss` |
| `/custom/#` | Custom app (wildcard) | `POST /api/custom?name=X` |
| `/switch` | Cambiar a app | `POST /api/switch` |
| `/apps` | Actualizar lista apps | `POST /api/apps` |
| `/nextapp`, `/previousapp` | Navegación | `ANY /api/nextapp`, `/api/previousapp` |
| `/settings` | Cambiar settings | `POST /api/settings` |
| `/power` | Encender/apagar | `POST /api/power` |
| `/sleep` | Dormir N segundos | `POST /api/sleep` |
| `/indicator1-3` | Control indicadores | `POST /api/indicator1..3` |
| `/moodlight` | Modo ambiente | `POST /api/moodlight` |
| `/rtttl`, `/sound`, `/r2d2` | Audio | `POST /api/rtttl`, `/sound`, `/r2d2` |
| `/doupdate` | Actualizar firmware | `POST /api/doupdate` |
| `/sendscreen` | Publicar LEDs como JSON | `GET /api/screen` |
| `/reboot` | Reiniciar | `ANY /api/reboot` |

### Topics de Publicación

| Topic | Trigger |
|-------|---------|
| `<prefix>/stats` | Periódico (cada statsInterval ms) |
| `<prefix>/stats/currentApp` | Al cambiar app (deduplicado) |
| `<prefix>/stats/effects` | Al conectar |
| `<prefix>/stats/transitions` | Al conectar |
| `<prefix>/button/left\|select\|right` | Al presionar botón |

### Archivos Involucrados

| Archivo | Propósito |
|---------|-----------|
| `src/MQTTManager/MQTTManager.h` | API pública, singleton |
| `src/MQTTManager/MQTTManager.cpp` | Conexión, tick, publish |
| `src/MQTTManager/MQTTManager_internal.h` | Extern para 25 entidades HA |
| `src/MQTTManager/MQTTManager_Messages.cpp` | Recepción y dispatch de comandos |
| `src/MQTTManager/MQTTManager_Callbacks.cpp` | 7 handlers de callbacks HA |
| `src/MQTTManager/MQTTManager_Discovery.cpp` | Creación entidades HA |
| `src/MQTTManager/MQTTManager_StateUpdates.cpp` | Publicación stats, botones, indicadores |
| `lib/services/src/HADiscovery.h` | Descriptores de entidades |
| `lib/services/src/HADiscovery.cpp` | Implementación descriptores |
| `lib/services/src/MessageRouter.h` | Enrutamiento topic → comando |

---

## Problemas Detectados

### 1. Sin sensores de Weather en HA
- Los datos de clima (`weatherData`) existen pero no se exponen a Home Assistant
- HTTP API tiene `/api/weather/data` pero MQTT no publica estos datos
- Campos disponibles: `outdoorTemp`, `outdoorHumidity`, `pressure`, `aqi`, `condition`

### 2. Sin control de Night Mode via MQTT
- Night mode solo configurable desde web UI
- No hay switch HA para activar/desactivar
- No hay comando MQTT para control

### 3. Comandos HTTP sin equivalente MQTT
- `/api/erase` (factory reset)
- `/api/resetSettings` (reset sin borrar WiFi)
- `/api/weather` (configurar weather API)
- `/api/reorder` (reordenar apps)
- `/api/loop` (control de loop)
- `/api/datafetcher*` (fuentes de datos externas)

### 4. Weather data no se publica
- `weatherData.valid` puede ser true pero no hay topic para publicar
- Usuarios de HA no pueden ver clima exterior en dashboards

---

## Propuesta de Cambios

### Fase 1: Weather MQTT (Prioridad Alta)

#### 1.1 Agregar sensores de weather a HA Discovery
```cpp
// Nuevas entidades HASensor (5):
HASensor* outdoorTemp;      // Temperatura exterior
HASensor* outdoorHum;       // Humedad exterior
HASensor* pressure;         // Presión atmosférica (hPa)
HASensor* aqi;              // Índice calidad aire (1-6)
HASensor* weatherCondition; // Condición (text)

// Archivos a modificar:
// - lib/services/src/HADiscovery.h   (agregar descriptores)
// - lib/services/src/HADiscovery.cpp (implementar descriptores)
// - src/MQTTManager/MQTTManager_internal.h (extern pointers)
// - src/MQTTManager/MQTTManager_Discovery.cpp (crear entidades)
// - src/MQTTManager/MQTTManager_StateUpdates.cpp (actualizar valores)
```

#### 1.2 Publicar weather data periódicamente
```
Topic: <prefix>/stats/weather
Payload: {
  "valid": true,
  "outdoorTemp": 25.5,
  "outdoorHum": 60,
  "pressure": 1013,
  "aqi": 2,
  "condition": "Sunny",
  "conditionCode": 1000,
  "lastUpdate": 1234567890
}
Trigger: Junto con stats normales (si weatherData.valid)
```

#### 1.3 Agregar comando para fetch manual
```
Topic: <prefix>/weather/fetch
Payload: {} (vacío)
Acción: DataFetcher.forceWeatherFetch()

// Archivos:
// - lib/services/src/MessageRouter.h (CMD_WEATHER_FETCH)
// - lib/services/src/MessageRouter.cpp (routeTopic)
// - src/MQTTManager/MQTTManager_Messages.cpp (handler)
```

### Fase 2: Night Mode MQTT (Prioridad Media)

#### 2.1 Agregar switch de Night Mode a HA
```cpp
HASwitch* nightMode;  // Toggle ON/OFF

// Callback:
void onNightModeCommand(bool state, HASwitch* sender) {
    appConfig.nightMode = state;
    saveSettings();
    sender->setState(state);
}
```

#### 2.2 Agregar comando MQTT
```
Topic: <prefix>/nightmode
Payload: {"enabled": true} o {"enabled": false}
```

### Fase 3: Comandos Faltantes (Prioridad Baja)

#### 3.1 Factory Reset (con protección)
```
Topic: <prefix>/erase
Payload: {"confirm": "ERASE"}  // Requiere confirmación explícita
```

#### 3.2 Reset Settings
```
Topic: <prefix>/resetSettings
Payload: {}
```

#### 3.3 Weather Config
```
Topic: <prefix>/weather/config
Payload: {
  "apiKey": "xxx",
  "locationType": 0,
  "city": "Mexico City",
  ...
}
```

---

## Orden de Implementación Sugerido

1. **Weather sensors HA** - Mayor valor para usuarios de Home Assistant
2. **Weather stats publish** - Complementa los sensores
3. **Weather fetch command** - Control manual útil
4. **Night mode switch** - Alta demanda, cambio moderado
5. **Night mode command** - Complementa el switch
6. Resto según necesidad

---

## Notas Técnicas

### Límites
- Máximo 26 entidades HA (hardcoded en `HAMqtt`)
- Actualmente: 25 (con battery) → espacio para 1 más
- Para 5 sensores weather: aumentar límite o hacer opcional

### Publicación Weather
- Solo publicar si `weatherData.valid == true`
- Deduplicar como `setCurrentApp()` para evitar spam
- Incluir en `sendStats()` pero en topic separado

### Entity IDs
```cpp
// Formato existente: "%s_<suffix>"
// Ejemplos weather:
"outdoor_temp_%s"     → "outdoor_temp_abc123"
"outdoor_hum_%s"
"pressure_%s"
"aqi_%s"
"weather_cond_%s"
```

### Device Classes HA
```cpp
// Para auto-configuración en HA:
outdoorTemp:  deviceClass = "temperature", unit = "°C" o "°F"
outdoorHum:   deviceClass = "humidity", unit = "%"
pressure:     deviceClass = "pressure", unit = "hPa"
aqi:          deviceClass = nullptr, unit = nullptr  // No hay clase estándar
weatherCond:  deviceClass = nullptr  // Texto libre
```

---

## Referencias

- Código MQTT: `src/MQTTManager/`
- Documentación: `src/MQTTManager/CLAUDE.md`
- Weather data: `src/DataFetcher/DataFetcher.cpp` (fetchWeather)
- Config structs: `lib/config/src/ConfigTypes.h` (WeatherConfig, WeatherData)
- API HTTP weather: `src/ServerManager/ServerManager.cpp` líneas 219-264
