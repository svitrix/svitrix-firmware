# Svitrix Firmware — Claude Instructions

## Project

ESP32 firmware for Ulanzi TC001 Smart Pixel Clock (32x8 WS2812B LED matrix).
C++17 / Arduino framework / PlatformIO.

## Build & Test

```bash
pio run -e ulanzi          # Build firmware (auto-builds SPA via pre-build script)
pio test -e native_test    # Run all tests (Unity, 25 suites, 445 tests)
cd web && npm run dev      # SPA dev server (hot reload, proxies to device)
cd web && npm run upload   # Build SPA + upload to device LittleFS
```

Flash is ~96% full — watch binary size when adding features.

## Project Structure

```
src/
  main.cpp                    # Composition root, dependency wiring
  Apps/                       # Native + custom app rendering (TimeApp, DateApp, etc.)
  DisplayManager/             # Display coordinator (3 classes), custom app lifecycle, settings
                              #   + AppContentRenderer.cpp (shared rendering pipeline)
  MatrixDisplayUi/            # App framework: state machine, transitions, overlays, indicators
  MQTTManager/                # MQTT + Home Assistant auto-discovery (25 HA entities)
  MelodyPlayer/               # RTTTL parser + async PWM playback
  ServerManager/              # HTTP API (35 REST endpoints)
  PeripheryManager/           # Hardware: sensors, buzzer, buttons, battery, LDR
  MenuManager/                # On-device settings menu
  UpdateManager/              # OTA firmware updates
  PowerManager/               # Sleep/wake management
  Overlays/                   # Clock/date overlay rendering
  Games/                      # SlotMachine, SvitrixSays (disabled by default)
  effects/                    # 19 visual effects + weather overlays, IPixelCanvas abstraction
  data/                       # Static data: SvitrixFont.h, cert.h, icons.h, Dictionary.cpp/h
  contrib/                    # Third-party headers: GifPlayer.h, ArtnetWifi.h
  AppContent.h                # AppContentBase struct (shared rendering fields)
  Globals.cpp/h               # Config structs, global state
  Functions.cpp/h             # Utility functions
  timer.cpp/h                 # Background timer helpers
lib/
  interfaces/                 # 13 interfaces (IDisplayControl, INotifier, IPixelCanvas, etc.)
  services/                   # 13 service libraries (100% test coverage)
  config/                     # Configuration defaults
  TJpg_Decoder/               # JPEG decoder (local fork)
  home-assistant-integration/ # ArduinoHA library (trimmed)
  webserver/                  # Async web server wrapper (API routing, WiFi, OTA)
web/                          # SPA (Preact + Vite + TypeScript)
  src/
    api/                      # Typed API client for all 38 REST endpoints
    pages/                    # 6 pages: Screen, Settings, DataFetcher, Backup, Update, Files
    components/               # Nav, Toast
    styles/                   # Global CSS (dark theme)
  vite.config.ts              # Build config: IIFE output, gzip, dev proxy
data/
  web/                        # SPA build output (gzipped, uploaded to LittleFS)
tools/
  build_web.py                # PlatformIO pre-build: auto-builds SPA if sources changed
test/
  test_native/                # 25 test suites (native C++ tests)
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
│  ServerManager_ ──────────── HTTP REST API (35 endpoints)           │
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
│  lib/services/       13 stateless utilities (100% test coverage)    │
│  lib/config/         ConfigTypes — all config structs               │
│  lib/home-assistant-integration/   ArduinoHA v2.0.0 (trimmed)      │
│  lib/webserver/      ESPAsyncWebServer wrapper (API routing, WiFi)  │
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
          │  (REST API)  │  35 endpoints          │(IButtonRep)  │
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
├── lib/services/CLAUDE.md                  ← 13 services, API, deps, test mapping
├── lib/config/CLAUDE.md                    ← 13 config structs, all fields, defaults, persistence
├── lib/home-assistant-integration/CLAUDE.md← ArduinoHA fork, 7/17 entity types enabled
├── lib/webserver/CLAUDE.md                 ← FSWebServer, WiFi, routes, SPA fallback
├── web/README.md                           ← SPA: Preact + Vite, 6 pages, dev workflow
│
├── src/CLAUDE.md                           ← module docs: ServerManager (35 endpoints),
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

- Concise communication preferred
- Always verify changes with build + tests

## Git & Release Workflow

### Commit rules — [Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0/)

- Do NOT add `Co-Authored-By` lines to commit messages

#### Format

```
<type>[(scope)][!]: <description>

[body]

[footer(s)]
```

- **Subject line** (`<type>[(scope)]: <description>`):
  - Under 72 characters
  - Lowercase type, imperative mood (`add`, not `added` or `adds`)
  - No period at the end
- **Body** (optional, blank line after subject):
  - Explain **why**, not **what** (the diff shows what)
  - Wrap at 72 characters
- **Footer** (optional):
  - `BREAKING CHANGE: <description>` — for detailed breaking change explanation
  - `Refs: #123` — reference related issues

#### Types

| Type | When to use | Changelog section |
|------|-------------|-------------------|
| `feat` | New user-facing feature | **Features** |
| `fix` | Bug fix | **Bug Fixes** |
| `refactor` | Code restructuring (no behavior change) | — |
| `perf` | Performance improvement | **Performance** |
| `docs` | Documentation only | — |
| `test` | Adding/fixing tests | — |
| `chore` | Maintenance, deps, CI, tooling | — |
| `build` | Build system, platformio.ini | — |
| `ci` | CI/CD pipeline changes | — |
| `style` | Formatting, whitespace (no logic) | — |

#### Scope

Optional but encouraged. Use module name in lowercase:

`feat(mqtt): add light entity support`
`fix(display): correct gamma on low brightness`
`refactor(effects): extract IPixelCanvas`
`docs(web): update SPA dev workflow`

Common scopes: `mqtt`, `display`, `effects`, `web`, `server`, `menu`, `periphery`, `power`, `datafetcher`, `apps`, `config`, `font`

#### Breaking changes

Two ways to indicate:

1. `!` after type/scope: `refactor(mqtt)!: rename INotifier methods`
2. `BREAKING CHANGE:` footer for details:

```
refactor(mqtt)!: rename INotifier methods

Renamed publishState() to syncState() across all callers.

BREAKING CHANGE: INotifier::publishState() renamed to syncState().
All MQTT topic consumers must update their integration code.
```

#### Examples

```
feat(datafetcher): add JSON path array index support

Allow data sources to use array indices in jsonPath (e.g., "data.0.price").
This enables fetching from APIs that return arrays instead of objects.

Refs: #25
```

```
fix(display): prevent crash on empty notification text

Null text pointer caused segfault in DisplayRenderer::printText().
Added nullptr check before UTF-8 decoding.
```

```
chore: upgrade C++ standard from C++11 to C++17
```

### Branching strategy

```
main              ← always stable, every commit is a tagged release
  └── feature/*   ← short-lived branches for features/fixes
```

- `main` is the single long-lived branch
- All work happens in `feature/*` branches, merged via PR
- **Squash merge only** — every PR becomes a single commit on main (`gh pr merge --squash`)
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
