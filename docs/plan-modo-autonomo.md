# Plan: Modo Autónomo SVITRIX

## Contexto

El Ulanzi TC001 tiene un RTC DS1307 en el bus I2C (0x68) que actualmente no se utiliza. El objetivo es habilitar un **Modo Autónomo** donde el dispositivo funcione sin WiFi ni Home Assistant: alarmas locales, temporizador, cronómetro, y sincronización de hora con RTC para mantener la hora durante reinicios o pérdida de conexión.

Cuando hay WiFi disponible, el sistema sincroniza con NTP y actualiza el RTC. Sin WiFi, el RTC mantiene la hora.

## Arquitectura

```
┌─────────────────────────────────────────────────────────────────┐
│                         main.cpp                                │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────────┐  │
│  │ DS1307Provider│  │RtcTimeProvider│ │   AlarmManager_     │  │
│  │ (IRtcProvider)│─▶│(ITimeProvider)│ │(IAlarmProvider)     │  │
│  └──────────────┘  └──────────────┘  └──────────────────────┘  │
│         │                  │                    │               │
│         ▼                  ▼                    ▼               │
│  PeripheryManager    NightModePolicy      Native Apps          │
│  (sync NTP→RTC)      (hora local)      (Timer,Stopwatch,Alarm) │
└─────────────────────────────────────────────────────────────────┘
```

## Fases de Implementación

### Fase 1: Interfaz IRtcProvider ✅
**Archivos:** `lib/interfaces/src/IRtcProvider.h`

```cpp
class IRtcProvider {
public:
    virtual ~IRtcProvider() = default;
    virtual bool begin() = 0;
    virtual bool isRunning() = 0;
    virtual time_t getTime() = 0;
    virtual bool setTime(time_t epoch) = 0;
};
```

### Fase 2: DS1307Provider ✅
**Archivos:** `src/PeripheryManager/DS1307Provider.h`, `src/PeripheryManager/DS1307Provider.cpp`

- Comunicación I2C directa (sin librería externa)
- Conversión BCD ↔ binario
- Detección de batería RTC agotada (bit CH)
- Registro en PeripheryManager con setter injection

### Fase 3: RtcTimeProvider ✅
**Archivos:** `src/RtcTimeProvider.h`

- Implementa `ITimeProvider` usando `IRtcProvider`
- Fallback a `RealTimeProvider` si RTC no disponible
- Reemplaza `RealTimeProvider` como proveedor principal

### Fase 4: Sincronización NTP → RTC ✅
**Archivos:** `src/main.cpp`

- En loop(): después de NTP sync, escribir epoch al RTC
- Solo si RTC presente y WiFi conectado
- Log de sincronización exitosa

### Fase 5: Interfaz IAlarmProvider ✅
**Archivos:** `lib/interfaces/src/IAlarmProvider.h`

```cpp
struct Alarm {
    uint8_t id;
    uint8_t hour, minute;
    uint8_t days;        // Bitmask: Dom=0x01, Lun=0x02, ..., Sáb=0x40
    bool enabled;
    String label;
    String melody;       // RTTTL o nombre de archivo
};

class IAlarmProvider {
public:
    virtual void tick(time_t now) = 0;
    virtual bool addAlarm(const Alarm& a) = 0;
    virtual bool removeAlarm(uint8_t id) = 0;
    virtual bool updateAlarm(const Alarm& a) = 0;
    virtual std::vector<Alarm> getAlarms() = 0;
    virtual void snooze(uint8_t minutes = 5) = 0;
    virtual void dismiss() = 0;
};
```

### Fase 6: AlarmManager ✅
**Archivos:** `src/AlarmManager/AlarmManager.h`, `src/AlarmManager/AlarmManager.cpp`

- Hasta 10 alarmas configurables
- Persistencia en `/alarms.json` (LittleFS)
- Usa `ISound` para reproducir melodías
- Usa `IDisplayNotifier` para mostrar notificación de alarma
- Snooze configurable (1-30 min)
- Llamado desde `loop()` cada segundo

### Fase 7: App Timer ✅
**Archivos:** `src/Apps/Apps_NativeApps.cpp`, `src/Apps/Apps.h`

- Nuevo `TMODE` para timer (ej: `TMODE_TIMER = 7`)
- Countdown desde valor configurado
- Reproducir melodía al terminar
- Controles: botones L/M/R para iniciar/pausar/reset
- Estado persistido si se cambia de app

### Fase 8: App Stopwatch (Cronómetro) ✅
**Archivos:** `src/Apps/Apps_NativeApps.cpp`, `src/Apps/Apps.h`

- Nuevo `TMODE` para cronómetro (ej: `TMODE_STOPWATCH = 8`)
- Cuenta ascendente MM:SS.cs (centésimas)
- Controles: L=lap, M=start/stop, R=reset
- Máximo 99:59.99

### Fase 9: App Alarms ✅
**Archivos:** `src/Apps/Apps_NativeApps.cpp`, `src/Apps/Apps.h`

- Muestra próxima alarma activa o "No alarm"
- Indicación visual cuando suena (rojo)
- Color cyan para próxima alarma

### Fase 10: API HTTP ✅
**Archivos:** `src/ServerManager/ServerManager.cpp`

HTTP:
- `GET/POST /api/timer` - estado y control (start/pause/reset/setTime)
- `GET/POST /api/stopwatch` - estado y control (start/pause/reset)
- `GET/POST/PUT/DELETE /api/alarms` - CRUD alarmas + snooze/dismiss

### Fase 11: Web UI ✅
**Archivos:** `web/src/pages/autonomous/Autonomous.tsx`, `web/src/api/types.ts`, `web/src/api/client.ts`

- Página "Timer" con tres secciones: Timer, Stopwatch, Alarms
- Timer: display, controles start/pause/reset, configuración de tiempo
- Stopwatch: display con centésimas, controles start/pause/reset  
- Alarms: lista con toggle on/off, selector de días, agregar/eliminar
- Alert visual cuando suena alarma con botones snooze/dismiss

## Archivos Críticos a Modificar

| Archivo | Cambios | Estado |
|---------|---------|--------|
| `lib/interfaces/src/IRtcProvider.h` | Nueva interfaz | ✅ |
| `src/PeripheryManager/DS1307Provider.*` | Implementación RTC | ✅ |
| `src/RtcTimeProvider.h` | Wrapper ITimeProvider | ✅ |
| `src/main.cpp` | Wiring + NTP→RTC sync | ✅ |
| `lib/interfaces/src/IAlarmProvider.h` | Nueva interfaz | ✅ |
| `src/AlarmManager/AlarmManager.*` | Gestor de alarmas | ✅ |
| `src/Apps/Apps_NativeApps.cpp` | Apps timer/stopwatch/alarms | ✅ |
| `src/Apps/Apps.h` | Control namespaces | ✅ |
| `src/ServerManager/ServerManager.cpp` | Endpoints API | ✅ |
| `web/src/pages/autonomous/*` | UI Timer/Stopwatch/Alarms | ✅ |

## Recursos Existentes a Reutilizar

- `ITimeProvider` (`lib/interfaces/src/ITimeProvider.h`) - patrón de abstracción de tiempo
- `ISound` + `MelodyPlayer` - reproducción RTTTL existente
- `IDisplayNotifier` - notificaciones en pantalla
- `nativeAppGuard()` - patrón para apps nativas
- `saveFile()`/`openFile()` - persistencia LittleFS existente
- `NightModePolicy` - referencia para usar ITimeProvider

## Verificación

1. **Build:** `pio run -e ulanzi` sin errores
2. **Tests:** Crear tests nativos para DS1307Provider y AlarmManager
3. **Hardware:**
   - Desconectar WiFi → verificar que RTC mantiene hora
   - Crear alarma → verificar que suena a la hora correcta
   - Timer/Stopwatch → verificar controles con botones
4. **API:** Probar endpoints con curl/Postman
5. **Web UI:** Verificar página de alarmas funcional

## Orden de Implementación

1. ✅ IRtcProvider + DS1307Provider (base de todo)
2. ✅ RtcTimeProvider + sync NTP→RTC
3. ⏳ Tests nativos para RTC
4. ✅ IAlarmProvider + AlarmManager + persistencia
5. ✅ App Timer (más simple)
6. ✅ App Stopwatch
7. ✅ App Alarms
8. ✅ API HTTP
9. ✅ Web UI
10. ⏳ Tests de integración
