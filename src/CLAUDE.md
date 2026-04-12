# src/ Standalone Modules — AI Reference

Documentation for ServerManager, PeripheryManager, MenuManager, UpdateManager, and PowerManager modules.

> **Note (2026-04):** These modules were moved from loose `src/*.cpp/h` files into their own subdirectories (`src/ServerManager/`, `src/PeripheryManager/`, etc.) during the src/ reorganization. The detailed docs below remain here since these modules don't have their own `CLAUDE.md` files yet. Other restructuring: `AppContentRenderer.cpp` moved into `src/DisplayManager/`, `Overlays.cpp/h` moved to `src/Overlays/`, static data files moved to `src/data/`, third-party headers moved to `src/contrib/`.

---

## ServerManager

HTTP REST API server, WiFi connectivity, mDNS discovery, UDP device discovery, and TCP server. Primary external control interface alongside MQTT.

### Files

| File | LOC | Purpose |
|------|-----|---------|
| `ServerManager/ServerManager.h` | 39 | Public API, singleton, IButtonReporter |
| `ServerManager/ServerManager.cpp` | 536 | WiFi setup, HTTP endpoints, UDP/TCP, settings loader |

### Interfaces

**Implements:** `IButtonReporter` — forwards button presses to external HTTP callback.

**Consumes (7):**

| Interface | Used for |
|-----------|----------|
| `IDisplayRenderer` | `HSVtext()` during erase/reset |
| `IDisplayControl` | Power, moodlight, settings, stats, LED export |
| `IDisplayNavigation` | App list, switching, reordering, custom apps, effects, transitions |
| `IDisplayNotifier` | Notifications, indicators, dismiss |
| `ISound` | RTTTL playback, sound files, R2D2 |
| `IPower` | Sleep/wake |
| `IUpdater` | OTA firmware update |

### HTTP Endpoints (35)

#### Device Control
| Method | Path | Description |
|--------|------|-------------|
| `POST` | `/api/power` | Set display power state |
| `POST` | `/api/sleep` | Deep sleep (JSON: seconds) |
| `ANY` | `/api/reboot` | Restart ESP32 |
| `ANY` | `/api/erase` | Factory reset: wipe WiFi + LittleFS + settings |
| `ANY` | `/api/resetSettings` | Reset settings to defaults |
| `POST` | `/api/doupdate` | Check + apply OTA firmware update |

#### Display & Apps
| Method | Path | Description |
|--------|------|-------------|
| `GET` | `/api/loop` | Get current app loop as JSON |
| `GET` | `/api/apps` | Get apps with icon data |
| `POST` | `/api/apps` | Update app vector |
| `POST` | `/api/switch` | Switch to app by name |
| `ANY` | `/api/nextapp` | Navigate to next app |
| `POST` | `/api/previousapp` | Navigate to previous app |
| `POST` | `/api/reorder` | Reorder app vector |
| `POST` | `/api/custom?name=X` | Create/update/delete custom app |
| `POST` | `/api/moodlight` | Set moodlight mode |

#### Notifications & Indicators
| Method | Path | Description |
|--------|------|-------------|
| `POST` | `/api/notify` | Push notification |
| `ANY` | `/api/notify/dismiss` | Dismiss current notification |
| `POST` | `/api/indicator1` | Set indicator 1 |
| `POST` | `/api/indicator2` | Set indicator 2 |
| `POST` | `/api/indicator3` | Set indicator 3 |

#### Audio
| Method | Path | Description |
|--------|------|-------------|
| `POST` | `/api/rtttl` | Play RTTTL melody |
| `POST` | `/api/sound` | Play sound file |
| `POST` | `/api/r2d2` | Play R2D2-style sounds |

#### Settings & Stats
| Method | Path | Description |
|--------|------|-------------|
| `GET` | `/api/settings` | Get all settings as JSON |
| `POST` | `/api/settings` | Update settings (partial JSON) |
| `GET` | `/api/stats` | Device stats |
| `GET` | `/api/screen` | LED buffer as JSON (256 pixels) |
| `GET` | `/api/effects` | List available visual effects |
| `GET` | `/api/transitions` | List available transition effects |

#### DataFetcher
| Method | Path | Description |
|--------|------|-------------|
| `GET` | `/api/datafetcher` | List configured data sources |
| `POST` | `/api/datafetcher` | Add new data source |
| `DELETE` | `/api/datafetcher?name=X` | Remove data source |
| `POST` | `/api/datafetcher/fetch?name=X` | Force-fetch a data source |

#### System
| Method | Path | Description |
|--------|------|-------------|
| `GET` | `/version` | Firmware version string |
| `POST` | `/save` | Apply `/DoNotTouch.json` config to running system |

### Config Initialization

`initConfigDefaults()` — writes default values to `/DoNotTouch.json` on first boot (if file doesn't exist). Keys: `Static IP`, `Local IP`, `Gateway`, `Subnet`, `Primary DNS`, `Secondary DNS`, `Broker`, `Port`, `Username`, `Password`, `Prefix`, `Homeassistant Discovery`, `NTP Server`, `Timezone`, `Auth Username`, `Auth Password`.

`loadSettings()` — reads `/DoNotTouch.json` and applies values to config structs, then calls `applyAllSettings()`.

### Communication Protocols

**UDP Discovery (port 4210):** listens for `"FIND_SVITRIX"`, responds on 4211 with hostname.

**TCP Server (port 8080):** single-client, newline-delimited messages.

**HTTP Button Callback:** POST to `systemConfig.buttonCallback` URL on button press/release.

### Important Patterns

- Route ordering matters — `/api/datafetcher/fetch` before `/api/datafetcher`
- AP mode fallback at `192.168.4.1` — only `/version` endpoint registered
- All HTML pages served from SPA in LittleFS root (not PROGMEM)
- Auth via `mws.setAuth(user, pass)`
- mDNS registers `http` and `svitrix` TCP services

---

## PeripheryManager

Hardware abstraction singleton. Manages physical buttons, I2C temperature/humidity sensors, LDR (light), battery ADC, and piezo buzzer.

### Files

| File | LOC | Purpose |
|------|-----|---------|
| `PeripheryManager/PeripheryManager.h` | 77 | Public API, singleton, IPeripheryProvider + ISound |
| `PeripheryManager/PeripheryManager.cpp` | 541 | Hardware init, sensor loops, button dispatch, sound |

### Interfaces

**Implements:** `IPeripheryProvider`, `ISound`

**Dispatches to (multi-consumer vectors):**
- `IButtonHandler` — `leftButton()`, `rightButton()`, `selectButton()`, `selectButtonLong()`
- `IButtonReporter` — `sendButton(btn, state)`

### Hardware Components

#### Buttons (4x EasyButton)

| Button | GPIO | Events |
|--------|------|--------|
| Left | 26 | `onPressed` → `dispatchLeftButton()` |
| Right | 14 | `onPressed` → `dispatchRightButton()` |
| Select | 27 | `onPressed`, `onPressedFor(1000ms)`, `onSequence(2, 300ms)` |
| Reset | 13 | `onPressedFor(5000ms)` → factory reset (ULANZI only) |

Button swap: left/right swapped when `rotateScreen XOR swapButtons`.

#### I2C Sensors (auto-detected, first match wins)

1. **BME280** (0x76/0x77) — temp + humidity
2. **BMP280** (0x76/0x77) — temp only
3. **HTU21DF** — temp + humidity
4. **SHT31** (0x44) — temp + humidity

Readings every **10 seconds**, filtered with calibration offsets.

#### LDR — GPIO 35, read every **100ms**, median+mean filtered

#### Battery (ULANZI only) — GPIO 34 ADC, read every **10 seconds**

#### Buzzer — GPIO 15, PWM via `MelodyPlayer`, async RTTTL playback

### Button Dispatch Architecture

```
EasyButton callbacks
  └→ check blockNavigation
      └→ dispatch*()
          ├→ IButtonHandler vector: DisplayManager_, MenuManager_
          └→ IButtonReporter vector: MQTTManager_, ServerManager_
```

### Callback Registration

| Method | Purpose |
|--------|---------|
| `addButtonHandler(IButtonHandler*)` | Left/right/select dispatch |
| `addButtonReporter(IButtonReporter*)` | Raw button state reporting |
| `setOnPowerToggle(fn)` | Double-press power toggle |
| `setOnBrightnessChange(fn)` | Auto-brightness updates |
| `setOnFactoryReset(fn)` | Reset button long press |
| `setIsMenuActive(fn)` | Suppress reports during menu |

### Tick Loop

```
tick()
  ├─ Read buttons (dispatch reports if menu not active)
  ├─ Every 10s: read battery + temperature/humidity
  └─ Every 100ms: read LDR, compute auto-brightness
```

---

## MenuManager

On-device settings menu rendered on the LED matrix. Adjusts display, time, audio, and app settings via physical buttons.

### Files

| File | LOC | Purpose |
|------|-----|---------|
| `MenuManager/MenuManager.h` | 47 | Singleton, `IButtonHandler` implementation |
| `MenuManager/MenuManager.cpp` | 491 | Menu state machine, 13 menu items, button handlers |

### Menu Structure (13 items)

| Menu | Controls |
|------|----------|
| BRIGHT | `brightnessPercent` (1-100%) or AUTO |
| COLOR | `textColor` (15 preset colors) |
| SWITCH | `autoTransition` (ON/OFF) |
| T-SPEED | `timePerTransition` (200-2000ms) |
| APPTIME | `timePerApp` (1-30s) |
| TIME | `timeFormat` (8 strftime patterns) |
| DATE | `dateFormat` (9 strftime patterns) |
| WEEKDAY | `startOnMonday` (MON/SUN) |
| TEMP | `isCelsius` (°C/°F) |
| APPS | Toggle 5 native apps |
| SOUND | `soundActive` (ON/OFF) |
| VOLUME | `soundVolume` (0-30) |
| UPDATE | Triggers OTA update |

### Navigation

| Button | MainMenu | Submenu |
|--------|----------|---------|
| Right | Next item | Increase value |
| Left | Previous item | Decrease value |
| Select (short) | Enter submenu | Toggle |
| Select (long) | Exit menu | Save + back |

### Interfaces

**Provides:** `IButtonHandler` → PeripheryManager

**Consumes:** `IDisplayRenderer`, `IDisplayControl`, `IDisplayNavigation`, `IPeripheryProvider`, `IUpdater`

---

## UpdateManager

OTA firmware update manager. Checks remote server for new versions and performs HTTPS firmware updates with progress display.

### Files

| File | LOC | Purpose |
|------|-----|---------|
| `UpdateManager/UpdateManager.h` | 23 | Singleton, `IUpdater` implementation |
| `UpdateManager/UpdateManager.cpp` | 36 | Version check, firmware download, progress callbacks |

### OTA Flow

1. `checkUpdate(withScreen)` — HTTPS GET version string, compare against `VERSION` constant
2. `updateFirmware()` — download via `httpUpdate.update()` with pinned CA cert (`cert.h`)
3. Progress bar rendered via `IDisplayRenderer::drawProgressBar()` (green fill, y=7)
4. On success: automatic reboot

### Interfaces

**Provides:** `IUpdater` → ServerManager, MQTTManager, MenuManager

**Consumes:** `IDisplayRenderer`

### Config Fields

| Field | Purpose |
|-------|---------|
| `systemConfig.updateVersionUrl` | URL returning plain-text version |
| `systemConfig.updateFirmwareUrl` | URL to firmware `.bin` file |
| `systemConfig.updateAvailable` | Flag set by `checkUpdate()` |

---

## PowerManager

Deep sleep controller. Puts the ESP32 into `esp_deep_sleep` with timer and GPIO wake sources.

### Files

| File | LOC | Purpose |
|------|-----|---------|
| `PowerManager/PowerManager.h` | 23 | Singleton, `IPower` implementation |
| `PowerManager/PowerManager.cpp` | 36 | Sleep logic, JSON parser, wake source config |

### Wake Sources

| Source | Config |
|--------|--------|
| **Timer** | `esp_sleep_enable_timer_wakeup(seconds * 1e6)` |
| **GPIO 27** | `esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, LOW)` — middle button |

### Interfaces

**Provides:** `IPower` → ServerManager, MQTTManager

**Consumes:** none — uses ESP-IDF sleep APIs directly.

---

## Globals & Config

Central configuration store and persistent settings. Defines all config structs as global externs, handles load/save to NVS (Preferences) and LittleFS.

### Files

| File | Purpose |
|------|---------|
| `Globals.h` | Extern declarations for 13 config structs, debug macros |
| `Globals.cpp` | Config struct definitions with defaults, NVS load/save, LittleFS init |
| `lib/config/src/ConfigTypes.h` | All 13 config struct definitions + `TempSensorType` enum |

### 13 Config Structs

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
| `AppConfig` | showTime/Date/Bat/Temp/Hum, autoTransition, transEffect, scrollSpeed, nativeIconLayout | Apps |
| `AudioConfig` | soundActive, soundVolume, bootSound | Audio |
| `SystemConfig` | debugMode, hostname, deviceId, updateUrls, buttonCallback | System |

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

### Debug Macros

`DEBUG_PRINTLN(x)` and `DEBUG_PRINTF(fmt, ...)` — print with timestamp and function name. Always enabled. Expensive calls gated by `systemConfig.debugMode`.

---

## AppContent & AppContentRenderer

Shared rendering data structure and pipeline used by both custom apps and notifications on the 32x8 LED matrix.

### Files

| File | Purpose |
|------|---------|
| `AppContent.h` | `AppContentBase` struct + free function declarations |
| `DisplayManager/AppContentRenderer.cpp` | Shared rendering functions (icon, text, scroll, overlay) |

### AppContentBase — Key Fields

| Field | Type | Purpose |
|-------|------|---------|
| `text` | String | Display text |
| `color` | uint32_t | Text color |
| `icon` | File | Icon file handle (8x8 JPEG/GIF) |
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

### Rendering Pipeline (4 free functions)

1. **`renderAppIcon()`** — icon, push animation, separator, draw instructions, progress bar, charts
2. **`updateScrollAnimation()`** — advance scroll position, handle delays and speed
3. **`renderAppText()`** — centered/scrolling text, fragments or solid (rainbow/gradient)
4. **`renderAppOverlay()`** — weather overlay via `EffectOverlay()`

### Rendering Order

```
1. renderAppIcon()          ← if topText
2. updateScrollAnimation()  ← advance scroll
3. renderAppText()          ← draw text
4. renderAppIcon()          ← if !topText
5. renderAppOverlay()       ← weather effects
```
