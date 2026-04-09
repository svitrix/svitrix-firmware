# Svitrix Firmware — Claude Instructions

## Project

ESP32 firmware for Ulanzi TC001 Smart Pixel Clock (32x8 WS2812B LED matrix).
C++ / Arduino framework / PlatformIO.

## Build & Test

```bash
pio run -e ulanzi          # Build firmware
pio test -e native_test    # Run all tests (Unity, 21 suites, 368+ tests)
```

Flash is ~97% full — watch binary size when adding features.

## Project Structure

```
src/
  main.cpp                    # Composition root, dependency wiring
  Apps/                       # Native + custom app rendering (TimeApp, DateApp, etc.)
  DisplayManager/             # Display coordinator (3 classes), custom app lifecycle, settings
  MatrixDisplayUi/            # App framework: state machine, transitions, overlays, indicators
  MQTTManager/                # MQTT + Home Assistant auto-discovery (25 HA entities)
  MelodyPlayer/               # RTTTL parser + async PWM playback
  Games/                      # SlotMachine, SvitrixSays (disabled by default)
  effects/                    # 19 visual effects + weather overlays, IPixelCanvas abstraction
  AppContent.h                # AppContentBase struct (shared rendering fields)
  AppContentRenderer.cpp      # Shared rendering pipeline for apps + notifications
  Globals.cpp/h               # Config structs, global state
  ServerManager.cpp/h         # HTTP API (REST endpoints)
  PeripheryManager.cpp/h      # Hardware: sensors, buzzer, buttons, battery, LDR
  MenuManager.cpp/h           # On-device settings menu
  UpdateManager.cpp/h         # OTA firmware updates
  PowerManager.cpp/h          # Sleep/wake management
  SvitrixFont.h               # Custom bitmap font
lib/
  interfaces/                 # 13 interfaces (IDisplayControl, INotifier, IPixelCanvas, etc.)
  services/                   # 12 service libraries (100% test coverage)
  config/                     # Configuration defaults
  TJpg_Decoder/               # JPEG decoder (local fork)
  home-assistant-integration/ # ArduinoHA library (trimmed)
  webserver/                  # Async web server wrapper
test/
  test_native/                # 21 test suites (native C++ tests)
```

## Architecture Rules

- **13 interfaces** decouple all module-to-module communication — never add direct dependencies between modules
- **Singleton pattern** with `= delete` copy/move for managers
- **Setter injection** with `assert()` guards — all wiring in `main.cpp`
- **Effects** are decoupled from hardware via `IPixelCanvas` interface
- DisplayManager is split into 3 classes: `DisplayManager_` (coordinator) + `DisplayRenderer_` + `NotificationManager_`
- Each module directory has a `CLAUDE.md` or `README.md` with detailed AI reference — read it before modifying that module

## Module Dependency Graph

All inter-module communication goes through interfaces wired in `main.cpp`.

### Modules and their roles

```
┌─────────────────────────────────────────────────────────────────────┐
│ main.cpp — Composition Root                                         │
│ Creates singletons, wires all interfaces, runs setup() + loop()    │
└─────────────────────────────────────────────────────────────────────┘
         │ wires interfaces to:
         ▼
┌──────────────────────────────────────────────────────────────────────┐
│                        CORE MODULES                                  │
│                                                                      │
│  DisplayManager_ ─────────── "the brain"                            │
│  ├── DisplayRenderer_         draws text, shapes, charts, images    │
│  ├── NotificationManager_     notification queue + 3 indicators     │
│  └── MatrixDisplayUi          app framework, transitions, overlays  │
│       └── NeoMatrixCanvas     IPixelCanvas for effects              │
│                                                                      │
│  MQTTManager_ ────────────── MQTT broker + Home Assistant (25 HA)   │
│  ServerManager_ ──────────── HTTP REST API (33 endpoints)           │
│  PeripheryManager_ ───────── sensors, buzzer, buttons, LDR, battery │
│  MenuManager_ ────────────── on-device settings menu                │
│  PowerManager_ ───────────── deep sleep / wake                      │
│  UpdateManager_ ──────────── OTA firmware updates                   │
│  DataFetcher_ ────────────── external HTTP data sources             │
│  GameManager_ ────────────── games (disabled by default)            │
└──────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────┐
│                        LIBRARIES                                     │
│                                                                      │
│  lib/interfaces/     13 pure virtual interfaces (decoupling layer)  │
│  lib/services/       12 stateless utilities (100% test coverage)    │
│  lib/config/         ConfigTypes — all config structs               │
│  lib/home-assistant-integration/   ArduinoHA v2.0.0 (trimmed)      │
│  lib/webserver/      ESPAsyncWebServer wrapper + HTML pages         │
│  lib/TJpg_Decoder/   JPEG decoder (local fork)                     │
└──────────────────────────────────────────────────────────────────────┘
```

### Interface wiring (main.cpp)

Who provides what interface to whom:

```
                     PROVIDES                    CONSUMES
Module               Interface           →       Module
─────────────────────────────────────────────────────────────
DisplayManager_      IDisplayControl     →       MenuManager, ServerManager, MQTTManager
DisplayManager_      IDisplayNavigation  →       MenuManager, ServerManager, MQTTManager, DataFetcher
DisplayManager_      IMatrixHost         →       MatrixDisplayUi
DisplayManager_      IButtonHandler      →       PeripheryManager (dispatcher)
DisplayRenderer_     IDisplayRenderer    →       UpdateManager, GameManager, MenuManager, ServerManager
NotificationManager_ IDisplayNotifier   →       ServerManager, MQTTManager
MQTTManager_         INotifier           →       DisplayManager, NotificationManager
MQTTManager_         IButtonReporter     →       PeripheryManager (dispatcher)
ServerManager_       IButtonReporter     →       PeripheryManager (dispatcher)
PeripheryManager_    IPeripheryProvider  →       DisplayManager, NotificationManager, MenuManager, MQTTManager
PeripheryManager_    ISound              →       ServerManager, MQTTManager
PowerManager_        IPower              →       ServerManager, MQTTManager
UpdateManager_       IUpdater            →       ServerManager, MQTTManager, MenuManager
NeoMatrixCanvas      IPixelCanvas        →       Effect system (19 effects)
MenuManager_         IButtonHandler      →       PeripheryManager (dispatcher)
```

### Data flow diagram

```
     ┌──────────┐         ┌──────────────┐        ┌─────────────┐
     │  Buttons  │────────▶│PeripheryMgr  │───────▶│ DisplayMgr  │
     │  LDR     │ hardware│ (IButtonHandler│ iface │ (IDisplayCtrl│
     │  Sensors │ events  │  ISound       │───┐   │  IDisplayNav)│
     └──────────┘         │  IPeriphProv) │   │   └──────┬───────┘
                          └──────────────┘   │          │
                                │            │          │ owns
                                │            │   ┌──────▼───────┐
          ┌─────────────┐       │            │   │MatrixDisplayUi│
          │ MQTT Broker │◀─────▶│            │   │(state machine,│
          │ (Home Asst) │ iface │            │   │ transitions)  │
          └──────┬──────┘       │            │   └──────┬───────┘
                 │         ┌────▼──────┐     │          │ renders
                 └────────▶│MQTTManager│     │   ┌──────▼───────┐
                           │(INotifier │─────┘   │DisplayRenderer│
                           │ IButtonRep│         │(IDisplayRender)│
                           └───────────┘         └──────┬───────┘
                                                        │
          ┌─────────────┐                        ┌──────▼───────┐
          │  HTTP Client │◀─────────────────────▶│ ServerManager │
          │  (REST API)  │  33 endpoints          │(IButtonRep)  │
          └─────────────┘                        └──────────────┘

          ┌─────────────┐                        ┌──────────────┐
          │  LED Matrix  │◀──────────────────────│    FastLED    │
          │  32x8 WS2812│  256 pixels             │  NeoMatrix   │
          └─────────────┘                        └──────────────┘
```

### Main loop execution order

```cpp
void loop() {
    timer_tick();              // Background timers
    ServerManager.tick();      // Process HTTP requests
    DisplayManager.tick();     // Render current app/notification/effect
    PeripheryManager.tick();   // Read sensors, buttons, update brightness
    if (ServerManager.isConnected) {
        MQTTManager.tick();    // Process MQTT messages, publish stats
        DataFetcher.tick();    // Fetch external data sources
    }
}
```

### Service consumption map

Which `lib/services/` each module uses:

```
DisplayManager  ← ColorUtils, TimeEffects, GammaUtils, TextUtils, StatsBuilder, OverlayMapping
DisplayRenderer ← TextUtils, UnicodeFont, ColorUtils
PeripheryManager← SensorCalc
MQTTManager     ← MessageRouter, HADiscovery, AppRegistry, StatsBuilder, PlaceholderUtils
Apps            ← ColorUtils, TimeEffects, TextUtils
main.cpp        ← TextUtils (setTextFont at startup)
```

### CLAUDE.md reference map

Each module has detailed AI documentation:

```
CLAUDE.md (root)                            ← you are here
├── lib/interfaces/CLAUDE.md                ← 13 interfaces, methods, implementors, consumers
├── lib/services/CLAUDE.md                  ← 12 services, API, deps, test mapping
├── lib/home-assistant-integration/CLAUDE.md← ArduinoHA fork, 7/17 entity types enabled
├── src/DisplayManager/CLAUDE.md            ← 3 classes, 9 files, rendering, custom apps
├── src/MQTTManager/CLAUDE.md               ← 25 HA entities, 20 topics, 7 callbacks
└── src/MatrixDisplayUi/CLAUDE.md           ← state machine, 10 transitions, indicators
```

## Coding Conventions

- Build flags defined in `platformio.ini` — `-DULANZI` for hardware-specific code
- `#ifdef ULANZI` guards for battery, LDR, and hardware-specific features
- Custom font rendering via `matrixPrint()` in DisplayRenderer (not Adafruit GFX)
- Colors: hex strings `"#RRGGBB"` or `[r,g,b]` arrays in JSON APIs
- MQTT topics prefixed with configurable `mqttConfig.prefix`
- LittleFS for persistent storage: `/ICONS/`, `/CUSTOMAPPS/`, `/PALETTES/`
- JSON parsing uses ArduinoJson v6 (StaticJsonDocument/DynamicJsonDocument)

## Testing

- Tests run natively (not on device) using `native_test` environment
- Test mocks in `test/mocks/` — mock Arduino, FastLED, WiFi, etc.
- Service libraries in `lib/services/` have 100% test coverage
- Always run `pio test -e native_test` after changes to verify nothing breaks

## API Endpoints

- `GET /api/stats` — device stats JSON
- `POST /api/notify` — push notification (JSON: text, icon, duration, color, rainbow, rtttl, sound)
- MQTT: 23 incoming command topics, periodic stats publishing

## User Preferences

- Language: Ukrainian (prompts often in Ukrainian, respond accordingly)
- Concise communication preferred
- Always verify changes with build + tests

## Git

- Do NOT add `Co-Authored-By` lines to commit messages
