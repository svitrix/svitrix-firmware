# Svitrix Firmware — AI Reference

ESP32 firmware for Ulanzi TC001 Smart Pixel Clock (32×8 WS2812B-Mini LED matrix).
C++17 / Arduino / PlatformIO.

## TL;DR

- **Stack:** ESP32-WROOM-32D · Arduino-ESP32 · FastLED/NeoMatrix · LittleFS · AsyncWebServer · ArduinoHA
- **Entry point:** [src/main.cpp](src/main.cpp) — composition root, wires interfaces, owns `setup()`/`loop()`
- **Decoupling:** 13 pure-virtual interfaces in [lib/interfaces/](lib/interfaces/); all inter-module calls go through them
- **Per-module docs:** see [Module Doc Map](#module-doc-map) — each dir auto-loads on Read
- **Deep dives:** [Git workflow](.claude/git-workflow.md) · [Hardware pinout](src/PeripheryManager/CLAUDE.md) · [Config structs](lib/config/CLAUDE.md)

## Build & Test

```bash
pio run -e ulanzi          # Build firmware (auto-builds SPA via tools/build_web.py)
pio test -e native_test    # Native unit tests (Unity)
cd web && npm run dev      # SPA dev server (hot reload, proxies to device)
cd web && npm run upload   # Build SPA + upload to LittleFS
```

## Hardware (Ulanzi TC001)

| Component     | Spec / Pin                                                           |
| ------------- | -------------------------------------------------------------------- |
| MCU           | ESP32-WROOM-32D (dual Xtensa LX6, 240 MHz)                           |
| Flash / RAM   | 8 MB (partition uses 4 MB, expandable) · 520 KB SRAM                 |
| App partition | 1.875 MB (OTA slot × 2), SPIFFS 256 KB                               |
| LED matrix    | 8×32 WS2812B-Mini, GPIO32, serpentine                                |
| Buttons       | GPIO26 (L), GPIO27 (M/wake), GPIO14 (R), GPIO13 (reset) — active LOW |
| Buzzer        | GPIO15 (passive piezo, LEDC PWM)                                     |
| LDR           | GPIO35 (GL5516, ADC1_CH7)                                            |
| Battery       | GPIO34 (ADC1_CH6, voltage divider), 4400 mAh                         |
| I²C           | GPIO21 SDA, GPIO22 SCL — SHT3x (0x44), DS1307 (0x68)                 |
| USB-Serial    | CH340                                                                |

## Project Structure

Physical layout only — for "what does X do?" see [Module Doc Map](#module-doc-map).

```
src/
├── main.cpp                    composition root
├── DisplayManager/
├── MatrixDisplayUi/
├── MQTTManager/
├── ServerManager/
├── PeripheryManager/
├── MenuManager/
├── UpdateManager/
├── PowerManager/
├── DataFetcher/
├── Apps/
├── MelodyPlayer/
├── effects/
├── Overlays/
├── data/                       SvitrixFont, cert, icons, Dictionary
├── contrib/                    GifPlayer, ArtnetWifi (3rd-party)
├── AppContent.h
├── Globals.{cpp,h}
├── Functions.{cpp,h}
└── timer.{cpp,h}
lib/
├── interfaces/                 pure-virtual decoupling layer
├── services/                   stateless utility libs
├── config/
├── home-assistant-integration/ ArduinoHA fork
├── webserver/                  ESPAsyncWebServer wrapper
└── TJpg_Decoder/
web/                            Preact + Vite + TypeScript SPA
data/web/                       SPA build output → LittleFS
tools/build_web.py              PlatformIO pre-build hook
test/test_native/               native C++ unit tests
```

## Architecture Rules

- **Interfaces everywhere** — never add direct `#include` of one module into another; wire via [lib/interfaces/](lib/interfaces/)
- **Singletons** with `= delete` copy/move; setter injection with `assert()` guards; all wiring in [main.cpp](src/main.cpp)
- **Effects decoupled from hardware** via `IPixelCanvas`; tests use `MockPixelCanvas`
- **DisplayManager is 3 classes:** `DisplayManager_` (coordinator) + `DisplayRenderer_` + `NotificationManager_`
- **Before modifying a module — read its CLAUDE.md / README.md** (listed below)

## Interface Wiring

| Provider             | Interface            | Consumers                                                     |
| -------------------- | -------------------- | ------------------------------------------------------------- |
| DisplayManager_      | `IDisplayControl`    | MenuManager, ServerManager, MQTTManager                       |
| DisplayManager_      | `IDisplayNavigation` | MenuManager, ServerManager, MQTTManager, DataFetcher          |
| DisplayManager_      | `IMatrixHost`        | MatrixDisplayUi                                               |
| DisplayManager_      | `IButtonHandler`     | PeripheryManager                                              |
| DisplayRenderer_     | `IDisplayRenderer`   | UpdateManager, MenuManager, ServerManager                     |
| NotificationManager_ | `IDisplayNotifier`   | ServerManager, MQTTManager                                    |
| MQTTManager_         | `INotifier`          | DisplayManager, NotificationManager                           |
| MQTTManager_         | `IButtonReporter`    | PeripheryManager                                              |
| ServerManager_       | `IButtonReporter`    | PeripheryManager                                              |
| PeripheryManager_    | `IPeripheryProvider` | DisplayManager, NotificationManager, MenuManager, MQTTManager |
| PeripheryManager_    | `ISound`             | ServerManager, MQTTManager                                    |
| PowerManager_        | `IPower`             | ServerManager, MQTTManager                                    |
| UpdateManager_       | `IUpdater`           | ServerManager, MQTTManager, MenuManager                       |
| NeoMatrixCanvas      | `IPixelCanvas`       | Effect system                                                 |
| MenuManager_         | `IButtonHandler`     | PeripheryManager                                              |

## Main loop

```cpp
void loop() {
    timer_tick();
    ServerManager.tick();      // HTTP requests
    DisplayManager.tick();     // Render app/notification/effect
    PeripheryManager.tick();   // Sensors, buttons, auto-brightness
    if (ServerManager.isConnected) {
        MQTTManager.tick();    // MQTT messages, stats publishing
        DataFetcher.tick();    // External HTTP data sources
    }
}
```

## Service Consumption

Which [lib/services/](lib/services/) each module uses:

| Consumer | Services |
|----------|----------|
| DisplayManager | ColorUtils, TimeEffects, GammaUtils, TextUtils, StatsBuilder, OverlayMapping |
| DisplayRenderer | TextUtils, UnicodeFont, ColorUtils |
| PeripheryManager | SensorCalc |
| MQTTManager | MessageRouter, HADiscovery, AppRegistry, StatsBuilder, PlaceholderUtils |
| Apps | ColorUtils, TimeEffects, TextUtils, LayoutEngine |
| main.cpp | TextUtils (`setTextFont(SvitrixFont)` at startup) |

## Module Doc Map

Per-module docs auto-load when you Read files in their directory.

| Module | Doc | Role |
|--------|-----|------|
| Interfaces | [lib/interfaces/CLAUDE.md](lib/interfaces/CLAUDE.md) | 13 pure-virtual interfaces (decoupling layer) |
| Services | [lib/services/CLAUDE.md](lib/services/CLAUDE.md) | 14 stateless libs, 100% test coverage |
| Config | [lib/config/CLAUDE.md](lib/config/CLAUDE.md) | Config structs, defaults, persistence |
| HA integration | [lib/home-assistant-integration/CLAUDE.md](lib/home-assistant-integration/CLAUDE.md) | ArduinoHA fork, entity types enabled |
| Webserver | [lib/webserver/CLAUDE.md](lib/webserver/CLAUDE.md) | Async server wrapper, WiFi, SPA fallback |
| SPA | [web/README.md](web/README.md) | Preact + Vite, pages & components |
| src root | [src/CLAUDE.md](src/CLAUDE.md) | Globals, AppContent, cross-cutting files |
| ServerManager | [src/ServerManager/CLAUDE.md](src/ServerManager/CLAUDE.md) | HTTP REST API, WiFi, mDNS, UDP/TCP |
| PeripheryManager | [src/PeripheryManager/CLAUDE.md](src/PeripheryManager/CLAUDE.md) | Buttons, I2C sensors, LDR, battery, buzzer |
| MenuManager | [src/MenuManager/CLAUDE.md](src/MenuManager/CLAUDE.md) | On-device settings menu |
| UpdateManager | [src/UpdateManager/CLAUDE.md](src/UpdateManager/CLAUDE.md) | OTA firmware updates (HTTPS, pinned CA) |
| PowerManager | [src/PowerManager/CLAUDE.md](src/PowerManager/CLAUDE.md) | Deep sleep, timer + GPIO wake |
| DisplayManager | [src/DisplayManager/CLAUDE.md](src/DisplayManager/CLAUDE.md) | 3 classes, rendering, custom apps |
| MQTTManager | [src/MQTTManager/CLAUDE.md](src/MQTTManager/CLAUDE.md) | MQTT + HA auto-discovery |
| MatrixDisplayUi | [src/MatrixDisplayUi/CLAUDE.md](src/MatrixDisplayUi/CLAUDE.md) | App framework, state machine, transitions |
| DataFetcher | [src/DataFetcher/CLAUDE.md](src/DataFetcher/CLAUDE.md) | External HTTP sources, round-robin polling |
| Apps | [src/Apps/README.md](src/Apps/README.md) | Native + custom app rendering |
| MelodyPlayer | [src/MelodyPlayer/README.md](src/MelodyPlayer/README.md) | RTTTL parser, async PWM |
| Effects | [src/effects/README.md](src/effects/README.md) | Visual effects, weather overlays, IPixelCanvas |

## Common Change Patterns

| Task | Touches |
|------|---------|
| New visual effect | `src/effects/<Cat>Effects.*`, `EffectRegistry.cpp`, `test/test_native/test_effects/`, `src/effects/README.md` |
| New config field | `lib/config/src/ConfigTypes.h`, `src/Globals.cpp`, `src/ServerManager/*`, `web/src/api/types.ts`, `web/src/context/SettingsContext.tsx`, settings UI section, `lib/config/CLAUDE.md` |
| New MQTT / HA entity | Use `/add-ha-entity` skill (covers all steps) |
| New service library | Use `/add-service` skill (covers `lib/services/` pattern) |
| New native app | `src/Apps/Apps_NativeApps.cpp`, `src/Apps/Apps.h`, register in `DisplayManager.cpp`, `src/Apps/README.md` |
| New TimeMode (TMODE) | `src/Apps/Apps_NativeApps.cpp`, `web/src/pages/settings-sections/TimeDateSection.tsx` |
| New UI page/section | `web/src/pages/`, `web/src/api/types.ts`, `SettingsContext.tsx`, `api/client.ts`, `web/README.md` |

## Cross-module Impact Map

| When you change… | Also update… |
|---|---|
| `lib/config/ConfigTypes.h` fields | `Globals.cpp` (NVS), ServerManager API, web UI, MQTTManager (HA entities), `lib/config/CLAUDE.md` |
| `lib/interfaces/I*.h` methods | All implementors + consumers (see Interface Wiring), `lib/interfaces/CLAUDE.md` |
| `IPixelCanvas` API | All effects, `NeoMatrixCanvas.h`, `MockPixelCanvas.h`, effect tests |
| `src/effects/EffectRegistry` | `EffectRegistry.h` (kNumEffects), `src/effects/README.md`, web UI effect list |
| `src/MQTTManager/` HA entities | `src/MQTTManager/CLAUDE.md` entity table |
| `web/src/api/types.ts` Settings | `SettingsContext.tsx`, all settings-sections using changed fields |

## Coding Conventions

- Build flags in `platformio.ini` — `-DULANZI` for hardware-specific code
- `#ifdef ULANZI` guards for battery, LDR, hardware features
- Custom font via `matrixPrint()` in DisplayRenderer (not Adafruit GFX)
- Colors: `"#RRGGBB"` or `[r,g,b]` in JSON APIs
- MQTT topics prefixed with configurable `mqttConfig.prefix`
- LittleFS storage: `/ICONS/`, `/CUSTOMAPPS/`, `/PALETTES/`
- JSON: ArduinoJson v6 (`StaticJsonDocument`/`DynamicJsonDocument`)

## Testing

Native-only (not on device). Mocks in `test/mocks/`. [lib/services/](lib/services/) maintains 100% coverage. Run `pio test -e native_test` after every change.

## User Preferences

- Concise communication
- Always verify changes with build + tests

## Available Skills

Prefer these over manual multi-step workflows — each is a tested, opinionated recipe.

| Skill | Use when |
|-------|----------|
| `/add-ha-entity` | Adding a MQTT sensor/switch/light/button with HA auto-discovery |
| `/add-effect` | Adding a visual effect following the `IPixelCanvas` pattern |
| `/add-service` | Extracting reusable stateless logic into `lib/services/` with 100% test coverage |
| `/parallel` | Splitting a feature into 2+ independent worktrees (no shared files) |
| `/delegate` | Handing off a full feature (branch + code + tests + PR) to a background agent |
| `/build` | Full local CI: tests + firmware build + flash-size check |
| `/flash-size` | Detailed binary size + partition layout analysis |
| `/sync-docs` | Re-sync all CLAUDE.md files with current branch changes before a PR |
| `/update-docs` | Update docs after a specific code change (narrower than sync-docs) |
| `/sync-public-docs` | Refresh public VitePress docs (`docs/`) before release |
| `/pr` | Create a structured GitHub PR from the current feature branch |
| `/release` | Interactive beta/rc/stable release workflow |
| `/changelog` | Generate changelog from conventional commits (since last tag or range) |

## Git & Release

See [.claude/git-workflow.md](.claude/git-workflow.md) — Conventional Commits, SemVer with pre-release tags, squash-merge only, no `Co-Authored-By` lines.

<!-- Counts verified 2026-04-25: 13 interfaces · 14 services · 20 effects · 25 HA entities · 26 test suites -->
