# src/ — AI Reference Index

Top-level source directory. Module-specific docs live in their own subdirectories; this file covers only cross-cutting files at the `src/` root (Globals, AppContent).

> 📌 Auto-loads when reading files in `src/` (root level — not subdirectories).

## Module Doc Map

| Module | Doc | Role |
|--------|-----|------|
| DisplayManager | [DisplayManager/CLAUDE.md](DisplayManager/CLAUDE.md) | Display coordinator, custom apps, rendering |
| MatrixDisplayUi | [MatrixDisplayUi/CLAUDE.md](MatrixDisplayUi/CLAUDE.md) | App framework, state machine, transitions |
| MQTTManager | [MQTTManager/CLAUDE.md](MQTTManager/CLAUDE.md) | MQTT + Home Assistant auto-discovery |
| DataFetcher | [DataFetcher/CLAUDE.md](DataFetcher/CLAUDE.md) | External HTTP data sources |
| ServerManager | [ServerManager/CLAUDE.md](ServerManager/CLAUDE.md) | HTTP REST API, WiFi, mDNS, UDP/TCP |
| PeripheryManager | [PeripheryManager/CLAUDE.md](PeripheryManager/CLAUDE.md) | Buttons, sensors, LDR, battery, buzzer |
| MenuManager | [MenuManager/CLAUDE.md](MenuManager/CLAUDE.md) | On-device settings menu |
| UpdateManager | [UpdateManager/CLAUDE.md](UpdateManager/CLAUDE.md) | OTA firmware updates |
| PowerManager | [PowerManager/CLAUDE.md](PowerManager/CLAUDE.md) | Deep sleep / wake |
| Apps | [Apps/README.md](Apps/README.md) | Native + custom app rendering |
| effects | [effects/README.md](effects/README.md) | Visual effects, IPixelCanvas |
| MelodyPlayer | [MelodyPlayer/README.md](MelodyPlayer/README.md) | RTTTL parser, async PWM |

## Other Subdirectories (no dedicated docs)

- `Overlays/` — Clock/date/weather overlays (see effects)
- `data/` — Static data: `SvitrixFont.h`, `cert.h`, `icons.h`, `Dictionary.cpp/h`
- `contrib/` — Third-party headers: `GifPlayer.h`, `ArtnetWifi.h`

---

## Globals & Config

Central configuration store and persistent settings. Defines all config structs as global externs, handles load/save to NVS (Preferences) and LittleFS.

### Files

| File | Purpose |
|------|---------|
| `Globals.h` | Extern declarations for config structs, debug macros |
| `Globals.cpp` | Config struct definitions with defaults, NVS load/save, LittleFS init |
| `lib/config/src/ConfigTypes.h` | All config struct definitions + `TempSensorType` enum |

### Config Structs

| Struct | Key Fields | Purpose |
|--------|------------|---------|
| `MqttConfig` | host, port, user, pass, prefix | MQTT broker |
| `NetworkConfig` | isStatic, ip, gateway, subnet, dns | Static IP |
| `HaConfig` | discovery, prefix | Home Assistant discovery |
| `AuthConfig` | user, pass | HTTP auth |
| `SensorConfig` | currentTemp, currentHum, currentLux, tempSensorType, offsets | Sensor readings |
| `BatteryConfig` | percent, raw, minRaw, maxRaw | Battery (ULANZI) |
| `DisplayConfig` | matrixLayout, matrixFps, matrixOff, mirror, rotate, backgroundEffect | Matrix settings |
| `BrightnessConfig` | brightness, autoBrightness, min/max, ldrGamma/Factor | Brightness |
| `ColorConfig` | textColor, timeColor, dateColor, batColor, tempColor, humColor, weekday, calendar | Colors |
| `TimeConfig` | timeFormat, dateFormat, timeMode, startOnMonday, ntpServer, isCelsius | Time/date |
| `AppConfig` | showTime/Date/Bat/Temp/Hum, autoTransition, transEffect, scrollSpeed, nativeIconLayout, nightMode/Start/End/Brightness/Color/BlockTransition | Apps |
| `AudioConfig` | soundActive, soundVolume, bootSound | Audio |
| `SystemConfig` | debugMode, hostname, deviceId, updateUrls, buttonCallback | System |

See [lib/config/CLAUDE.md](../lib/config/CLAUDE.md) for full field list.

### Persistence — Three Layers (later overrides earlier)

1. **Compile-time defaults** — initializer lists in `Globals.cpp`
2. **NVS (Preferences)** — `loadSettings()` / `saveSettings()` (30+ keys, namespace `"svitrix"`)
3. **`/dev.json` (LittleFS)** — `loadDevSettings()` for advanced overrides (26 optional keys)

### Key Functions

| Function | Purpose |
|----------|---------|
| `loadSettings()` | Init LittleFS → load NVS → set deviceId → load dev.json |
| `saveSettings()` | Write user settings to NVS |
| `formatSettings()` | Wipe NVS namespace (factory reset) |
| `startLittleFS()` | Mount LittleFS, create dirs, handle corrupt FS |
| `loadDevSettings()` | Parse `/dev.json` for advanced overrides |
| `getID()` | Generate device ID from MAC: `svitrix_XXYYZZ` |
| `isNightModeActive()` | Check if current time is within night mode window |

### Debug Macros

`DEBUG_PRINTLN(x)` and `DEBUG_PRINTF(fmt, ...)` — print with timestamp and function name. Always enabled. Expensive calls gated by `systemConfig.debugMode`.

---

## AppContent

Shared rendering data structure used by both custom apps and notifications on the 32×8 LED matrix.

### Files

| File | Purpose |
|------|---------|
| `AppContent.h` | `AppContentBase` struct + free function declarations |
| `DisplayManager/AppContentRenderer.cpp` | Shared rendering functions (icon, text, scroll, overlay) |

> Detailed rendering pipeline docs live in [DisplayManager/CLAUDE.md](DisplayManager/CLAUDE.md).

### AppContentBase — Key Fields

| Field | Type | Purpose |
|-------|------|---------|
| `text` | String | Display text |
| `color` | uint32_t | Text color |
| `icon` | File | Icon file handle (8×8 JPEG/GIF) |
| `rainbow` | bool | Per-character HSV rainbow |
| `gradient[2]` | int | Two-color gradient |
| `fragments` / `colors` | vector | Per-segment colored text |
| `scrollposition` / `scrollSpeed` | float | Scroll animation state |
| `pushIcon` | byte | Icon push animation mode |
| `progress` / `pColor` / `pbColor` | int/uint32_t | Progress bar |
| `barData[16]` / `lineData[16]` | int | Bar/line chart data |
| `layout` | IconLayout | Icon/text positioning: Left, Right, None |
| `effect` | int | Background effect index |
| `overlay` | OverlayEffect | Weather overlay |
| `drawInstructions` | String | JSON draw commands |

### Inheritance

```
AppContentBase
├── CustomApp    adds: name, lifetime, duration, repeat, bounce, currentFrame, iconName
└── Notification adds: duration, repeat, hold, wakeup, sound, rtttl, startime
```
