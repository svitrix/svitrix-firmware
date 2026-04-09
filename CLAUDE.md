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
- Each module directory has a `README.md` with detailed AI reference — read it before modifying that module

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
