# Plan: Weather API Integration

## Resumen

Integrar WeatherAPI.com para mostrar datos meteorológicos externos, diferenciando de los sensores internos (SHT3x/BME280).

## Fuentes de Datos

| Fuente | Datos | Icono |
|--------|-------|-------|
| **Sensor Interno** (SHT3x/BME280) | Temp interior, Humedad interior | Termómetro casa |
| **WeatherAPI.com** (externa) | Temp exterior, Humedad exterior, Presión (mb), Condición clima, ICA | Iconos clima LaMetric |

## Configuración Web (nueva sección "Weather")

```
Weather API
├── API Key: [_________________]
├── Location Method: [City ▼] / Coordinates / Station ID
│   ├── City: [_________________]
│   ├── Coordinates: Lat [____] Lon [____]
│   └── Station ID: [_________________]
├── Update Interval: [15 min ▼] (15/30/60 min)
└── Units: [Metric ▼] / Imperial

Display Options
├── ☑ Show Outdoor Temperature    [icon: sol/nube/lluvia según condición]
├── ☑ Show Outdoor Humidity       [icon: gota]
├── ☑ Show Pressure (mb)          [icon: barómetro]
├── ☑ Show Air Quality (AQI)      [icon: aire/pulmón]
├── ☑ Show Indoor Temperature     [icon: casa+termómetro]
└── ☑ Show Indoor Humidity        [icon: casa+gota]
```

## Apps Nativas Nuevas/Modificadas

| App | Contenido | Icono LaMetric |
|-----|-----------|----------------|
| **OutdoorTemp** | "23°C" o "73°F" | Condición actual (sunny, cloudy, rainy, snowy, etc.) |
| **OutdoorHumidity** | "65%" | Gota de agua |
| **Pressure** | "1013 mb" | Barómetro |
| **AirQuality** | "42 AQI" + categoría (Good/Moderate/...) | Icono aire |
| **IndoorTemp** | "21°C" | Casa con termómetro |
| **IndoorHumidity** | "55%" | Casa con gota |

## Iconos LaMetric a Seleccionar

Iconos 8x8 necesarios:

**Condiciones climáticas:**
- sunny (sol)
- partly_cloudy (sol con nube)
- cloudy (nube)
- rainy (lluvia)
- thunderstorm (tormenta)
- snowy (nieve)
- foggy (niebla)
- windy (viento)

**Sensores:**
- thermometer (termómetro exterior)
- humidity_drop (gota de agua)
- barometer (barómetro)
- air_quality (aire/pulmón)
- house_temp (casa con termómetro)
- house_humidity (casa con gota)

## Estructura de Datos

```cpp
// lib/config/src/ConfigTypes.h

enum WeatherLocationType {
    WEATHER_LOC_CITY = 0,
    WEATHER_LOC_COORDS = 1,
    WEATHER_LOC_STATION = 2
};

struct WeatherConfig {
    String apiKey;
    WeatherLocationType locationType;
    String city;              // "Mexico City" o "auto:ip"
    float latitude;
    float longitude;
    String stationId;         // PWS station ID
    uint16_t updateInterval;  // minutos (15, 30, 60)
    bool useCelsius;
    
    // Toggles de display
    bool showOutdoorTemp;
    bool showOutdoorHumidity;
    bool showPressure;
    bool showAirQuality;
    bool showIndoorTemp;
    bool showIndoorHumidity;
};

struct WeatherData {
    float outdoorTemp;
    float outdoorHumidity;
    float pressure;           // mb/hPa
    int aqi;                  // 0-500 (US EPA index)
    String condition;         // "sunny", "cloudy", "rainy", etc.
    int conditionCode;        // WeatherAPI condition code
    unsigned long lastUpdate; // millis() del último update
    bool valid;               // datos disponibles
};
```

## WeatherAPI.com Endpoints

**Current Weather + AQI:**
```
GET https://api.weatherapi.com/v1/current.json
    ?key={API_KEY}
    &q={LOCATION}        // city, lat,lon, o station:ID
    &aqi=yes
```

**Location formats:**
- City: `q=Mexico City` o `q=auto:ip`
- Coords: `q=19.4326,-99.1332`
- Station: `q=pws:KMAHANOV10`

**Response fields usados:**
```json
{
  "current": {
    "temp_c": 23.0,
    "temp_f": 73.4,
    "humidity": 65,
    "pressure_mb": 1013.0,
    "condition": {
      "text": "Partly cloudy",
      "code": 1003
    },
    "air_quality": {
      "us-epa-index": 2
    }
  }
}
```

## Mapeo Condition Code → Icono

| Code Range | Condición | Icono |
|------------|-----------|-------|
| 1000 | Sunny/Clear | sunny.gif |
| 1003 | Partly cloudy | partly_cloudy.gif |
| 1006, 1009 | Cloudy/Overcast | cloudy.gif |
| 1030, 1135, 1147 | Mist/Fog | foggy.gif |
| 1063, 1150-1201 | Rain/Drizzle | rainy.gif |
| 1087, 1273-1282 | Thunder | thunderstorm.gif |
| 1066, 1114-1225 | Snow/Sleet | snowy.gif |
| 1072, 1168, 1171 | Freezing | snowy.gif |

## Flujo de Datos

```
                                    ┌─────────────────┐
WeatherAPI.com ───HTTP GET───►      │  DataFetcher    │
(cada N minutos)                    │  .tick()        │
                                    └────────┬────────┘
                                             │
                                             ▼
                                    ┌─────────────────┐
                                    │  weatherData    │ (global)
                                    │  (exterior)     │
                                    └────────┬────────┘
                                             │
SHT3x/BME280 ───I2C───►             ┌────────┴────────┐
                        │           │                 │
                        ▼           ▼                 ▼
               ┌─────────────┐  ┌─────────┐  ┌──────────────┐
               │ sensorConfig│  │ Apps_   │  │ MQTTManager  │
               │ (interior)  │  │ Native  │  │ (HA entities)│
               └─────────────┘  └─────────┘  └──────────────┘
```

## Archivos a Modificar

### Fase 1: Infraestructura

| Archivo | Cambios |
|---------|---------|
| `lib/config/src/ConfigTypes.h` | Agregar `WeatherLocationType`, `WeatherConfig`, `WeatherData` |
| `src/Globals.h` | Extern declarations |
| `src/Globals.cpp` | Definir `weatherConfig`, `weatherData`, defaults, NVS load/save |
| `src/ServerManager/ServerManager_API.cpp` | Endpoints GET/SET weather settings |
| `web/src/api/types.ts` | Tipos TypeScript para WeatherConfig |
| `web/src/context/SettingsContext.tsx` | Agregar campos weather |
| `web/src/pages/settings/sections/WeatherSection.tsx` | Nueva sección UI |
| `web/src/pages/settings/SettingsPage.tsx` | Importar WeatherSection |

### Fase 2: Data Fetching

| Archivo | Cambios |
|---------|---------|
| `src/DataFetcher/DataFetcher.cpp` | Agregar weather fetch con timer |
| `src/DataFetcher/DataFetcher.h` | Declarar métodos weather |
| `lib/config/CLAUDE.md` | Documentar WeatherConfig |

### Fase 3: Apps Nativas

| Archivo | Cambios |
|---------|---------|
| `src/Apps/Apps.h` | Declarar 6 nuevas funciones app |
| `src/Apps/Apps_NativeApps.cpp` | Implementar OutdoorTemp, OutdoorHumidity, Pressure, AirQuality, IndoorTemp, IndoorHumidity |
| `src/DisplayManager/DisplayManager.cpp` | Registrar apps en `loadNativeApps()` basado en config toggles |
| `src/Apps/README.md` | Documentar nuevas apps |

### Fase 4: Iconos (Locales en LittleFS)

**Espacio disponible:** LittleFS = 256 KB, usado ~29 KB, libre ~227 KB
**Espacio necesario:** ~15 iconos × 500 bytes = ~8 KB

**Fuente:** https://developer.lametric.com/icons

| Archivo | Cambios |
|---------|---------|
| `data/ICONS/weather/` | Agregar iconos 8x8 GIF de LaMetric |
| `src/Apps/Apps_NativeApps.cpp` | Mapeo conditionCode → archivo icono |

**Iconos a descargar:**

| Nombre | Uso | URL búsqueda |
|--------|-----|--------------|
| `sunny.gif` | Condición soleado | weather sun |
| `partly_cloudy.gif` | Parcialmente nublado | weather cloud sun |
| `cloudy.gif` | Nublado | weather cloud |
| `rainy.gif` | Lluvia | weather rain |
| `thunderstorm.gif` | Tormenta | weather thunder |
| `snowy.gif` | Nieve | weather snow |
| `foggy.gif` | Niebla | weather fog |
| `windy.gif` | Viento | weather wind |
| `temp_out.gif` | Temp exterior | thermometer |
| `humidity.gif` | Humedad exterior | humidity drop |
| `pressure.gif` | Presión | barometer |
| `air_quality.gif` | Calidad aire | air lung |
| `temp_in.gif` | Temp interior | house thermometer |
| `humidity_in.gif` | Humedad interior | house drop |

### Fase 5: Home Assistant (opcional)

| Archivo | Cambios |
|---------|---------|
| `src/MQTTManager/MQTTManager.cpp` | Publicar weather entities |
| `src/MQTTManager/CLAUDE.md` | Documentar entities |

## NVS Keys (Preferences)

| Key | Tipo | Default |
|-----|------|---------|
| `WAPI_KEY` | String | "" |
| `WAPI_LOC_TYPE` | uint8 | 0 (city) |
| `WAPI_CITY` | String | "" |
| `WAPI_LAT` | float | 0.0 |
| `WAPI_LON` | float | 0.0 |
| `WAPI_STATION` | String | "" |
| `WAPI_INTERVAL` | uint16 | 30 |
| `WAPI_CELSIUS` | bool | true |
| `WAPI_SHOW_OTEMP` | bool | true |
| `WAPI_SHOW_OHUM` | bool | false |
| `WAPI_SHOW_PRES` | bool | false |
| `WAPI_SHOW_AQI` | bool | false |
| `WAPI_SHOW_ITEMP` | bool | false |
| `WAPI_SHOW_IHUM` | bool | false |

## API Rate Limiting

- WeatherAPI.com free tier: 1,000,000 calls/month (~1.4 calls/min)
- Con update cada 15 min: ~2,880 calls/mes (muy dentro del límite)
- Implementar cache: no fetch si `millis() - lastUpdate < interval * 60000`

## Manejo de Errores

```cpp
// En DataFetcher
if (httpCode != 200) {
    weatherData.valid = false;
    // Retry en próximo interval
    return;
}

// En Apps
if (!weatherData.valid) {
    // Mostrar "---" o icono de error
}
```

## Testing

- Test manual con API key real
- Mock responses para native tests (opcional)
- Verificar rate limiting funciona

---

## Siguiente Paso

Comenzar con **Fase 1: Infraestructura** - ConfigTypes, Globals, y WeatherSection.tsx
