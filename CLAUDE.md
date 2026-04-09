# Svitrix Firmware — Claude Instructions

## Project

ESP32 firmware for Ulanzi TC001 Smart Pixel Clock (32x8 WS2812B LED matrix).
C++ / Arduino framework / PlatformIO.

## Build & Test

```bash
pio run -e ulanzi          # Build firmware
pio test -e native_test    # Run all tests (Unity, 21 suites, 434 tests)
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
  SvitrixFont.h               # Unicode sparse glyph table (336 glyphs, UTF-8, binary search)
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
DataFetcher     ← (no lib/services dependencies)
main.cpp        ← TextUtils (setTextFont at startup)
```

### CLAUDE.md reference map

Each module has detailed AI documentation:

```
CLAUDE.md (root)                            ← you are here
│
├── lib/interfaces/CLAUDE.md                ← 13 interfaces, methods, implementors, consumers
├── lib/services/CLAUDE.md                  ← 12 services, API, deps, test mapping
├── lib/config/CLAUDE.md                    ← 13 config structs, all fields, defaults, persistence
├── lib/home-assistant-integration/CLAUDE.md← ArduinoHA fork, 7/17 entity types enabled
├── lib/webserver/CLAUDE.md                 ← FSWebServer, WiFi, routes, HTML pipeline
│
├── src/CLAUDE.md                           ← standalone modules: ServerManager (33 endpoints),
│                                              PeripheryManager (buttons, sensors, buzzer),
│                                              MenuManager (13 menu items), UpdateManager (OTA),
│                                              PowerManager (deep sleep), Globals (config store),
│                                              AppContentRenderer (shared rendering pipeline)
├── src/DisplayManager/CLAUDE.md            ← 3 classes, 9 files, rendering, custom apps
├── src/MQTTManager/CLAUDE.md               ← 25 HA entities, 20 topics, 7 callbacks
├── src/MatrixDisplayUi/CLAUDE.md           ← state machine, 10 transitions, indicators
├── src/DataFetcher/CLAUDE.md               ← external HTTP data sources, round-robin polling
│
├── src/Apps/README.md                      ← native + custom app rendering
├── src/Games/README.md                     ← GameManager, SlotMachine, SvitrixSays
├── src/MelodyPlayer/README.md              ← RTTTL parser, async PWM playback
└── src/effects/README.md                   ← 19 visual effects, weather overlays, IPixelCanvas
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

## Git & Release Workflow

### Commit rules

- Do NOT add `Co-Authored-By` lines to commit messages
- Use [Conventional Commits](https://www.conventionalcommits.org/) format:
  - `feat: add temperature overlay` — new feature
  - `fix: correct MQTT reconnect loop` — bug fix
  - `refactor: extract notification queue` — code restructuring
  - `docs: update MQTTManager CLAUDE.md` — documentation only
  - `test: add ColorUtils edge cases` — tests only
  - `chore: bump ArduinoHA to 2.0.1` — maintenance, deps, CI
  - `perf: reduce DisplayRenderer draw calls` — performance
- Scope is optional but encouraged: `feat(mqtt): add light entity support`
- Breaking changes: add `!` after type — `refactor!: rename INotifier methods`
- Keep subject line under 72 characters
- Body: explain **why**, not **what** (the diff shows what)

### Branching strategy

```
main              ← always stable, every commit is a tagged release
  └── feature/*   ← short-lived branches for features/fixes
```

- `main` is the single long-lived branch
- All work happens in `feature/*` branches, merged via PR
- No `develop` branch — keep it simple for solo/small team
- Add `develop` + `release/*` branches only when contributors appear

### Versioning — Semantic Versioning with pre-release tags

Format: `vMAJOR.MINOR.PATCH[-pre.N]`

```
v0.2.0-beta.1    ← first beta (from feature branch or main)
v0.2.0-beta.2    ← beta bug fixes
v0.2.0-rc.1      ← release candidate (feature freeze, only bug fixes)
v0.2.0-rc.2      ← critical fix in RC
v0.2.0           ← stable release (merged to main)
```

- **MAJOR** — breaking API/MQTT/config changes
- **MINOR** — new features, new HA entities, new effects
- **PATCH** — bug fixes, performance improvements

### Release process

1. Create `feature/*` branch from `main`
2. Develop and test (`pio run -e ulanzi && pio test -e native_test`)
3. When ready for beta testing: tag `v0.X.0-beta.1` on the branch
4. When feature-complete: tag `v0.X.0-rc.1` (only bug fixes after this)
5. Merge PR to `main`, tag `v0.X.0` on main
6. Create GitHub Release from tag, attach `firmware.bin`

### GitHub Releases

- **Stable** (`v0.2.0`) — full release, marked as "Latest"
- **Beta** (`v0.2.0-beta.1`) — mark as **pre-release** on GitHub
- **RC** (`v0.2.0-rc.1`) — mark as **pre-release** on GitHub
- Attach `.pio/build/ulanzi/firmware.bin` to each release
- Release notes: list changes grouped by type (Features, Fixes, Breaking)

### Tags

- Always use annotated tags: `git tag -a v0.2.0 -m "v0.2.0: short description"`
- Push tags explicitly: `git push origin v0.2.0`
- Never delete or move published tags
