# Test Infrastructure — AI Reference

> Unity framework, native C++ tests via PlatformIO. 21 suites, 368 tests.
> Run: `pio test -e native_test`

## Mock Infrastructure (test/mocks/)

### Arduino.h Mock (~174 LOC)
- Controllable `millis()` via `setMockMillis(value)` — for time-dependent tests
- Full Arduino `String` class (API-compatible)
- Math macros: `constrain`, `min`, `max`, `map`, `PI`
- `random()` wrapper around `rand()`
- Minimal `Serial` stub

### FastLED.h Mock (~200 LOC)
- `CRGB` struct: 8-bit RGB, operators, `nscale8()`, `lerp8()`
- `CHSV` with `hsv2rgb_spectrum()` conversion
- `CRGBPalette16` + predefined palettes (Rainbow, Ocean, Forest, Party, etc.)
- Math: `sin8()`, `cos8()`, `sin16()`, `inoise8()` (pseudo-noise)
- Random: `random8()`, `random16()`

### MockDisplay.h — 4 display interface mocks

| Mock Class | Interface | Tracks |
|------------|-----------|--------|
| `MockDisplayRenderer` | IDisplayRenderer | clear, show, setTextColor, printText, drawFilledRect, drawProgressBar, drawMenuIndicator + last params |
| `MockDisplayControl` | IDisplayControl | brightness, power, settings, auto-transition, moodlight + configurable returns |
| `MockDisplayNavigation` | IDisplayNavigation | app navigation, custom page parsing, effect/transition names |
| `MockDisplayNotifier` | IDisplayNotifier | notification dismiss, indicator states and colors |

### MockServices.h — 4 service interface mocks

| Mock Class | Interface | Tracks |
|------------|-----------|--------|
| `MockSound` | ISound | RTTTL playback, sound JSON parsing, R2D2 |
| `MockPower` | IPower | sleep commands, sleep JSON parsing |
| `MockUpdater` | IUpdater | update checks, firmware updates |
| `MockPeripheryProvider` | IPeripheryProvider | volume, uptime, sound stop |

## Test Suites (21 total)

### Pure Logic & Math (5 suites, 62 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_functions | test_functions.cpp | Functions.cpp | 11 | hexToUint32 color conversion (6), roundToDecimalPlaces (5) |
| test_math_utils | test_math_utils.cpp | MathUtils | 9 | roundToDecimalPlaces edge cases, half-up, large numbers |
| test_logmap | test_logmap.cpp | MathUtils | 10 | logMap boundaries, monotonicity, clamping, degenerate ranges |
| test_color_utils | test_color_utils.cpp | ColorUtils | 25 | hexToUint32 (7), kelvinToRGB (4), hsvToRgb (3), interpolateColor (11) |
| test_crgb_hex | test_crgb_hex.cpp | ColorUtils | 6 | CRGBtoHex primary colors, arbitrary RGB |

### Text & UTF-8 (2 suites, 41 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_text_metrics | test_text_metrics.cpp | TextUtils | 13 | Character widths, uppercase forcing, special chars |
| test_utf8 | test_utf8.cpp | TextUtils (utf8ascii) | 28 | ASCII passthrough (4), Latin-1 (4), Polish (2), Cyrillic (4), Ukrainian (4), interrupted sequences (2) |

### Sensors & Brightness (3 suites, 49 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_sensor_calc | test_sensor_calc.cpp | SensorCalc | 29 | Battery % (6), brightness calc (8), LDR inversion (4), offset (4), volume scaling (4), div-by-zero |
| test_gamma | test_gamma.cpp | GammaUtils | 7 | Gamma boundaries, out-of-range, monotonicity |
| test_time_effects | test_time_effects.cpp | TimeEffects | 13 | fadeColorAt (5), textEffectAt (8): fade/blink priorities, zero intervals |

### MQTT & Routing (2 suites, 53 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_message_router | test_message_router.cpp | MessageRouter | 30 | routeTopic exact matches (7), all 20 commands, extractCustomTopicName (5), isJsonPayload (5), getSubscriptionTopics (3) |
| test_placeholder_utils | test_placeholder_utils.cpp | PlaceholderUtils | 23 | Basic substitution (5), multiple placeholders (3), longer replacement regression (5), edge cases (8) |

### App System (2 suites, 48 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_app_registry | test_app_registry.cpp | AppRegistry | 13 | getNativeAppNames (3), isNativeApp (5), serializeAppList (4) |
| test_apps_logic | test_apps_logic.cpp | Apps_Registry.cpp | 30 | getCustomAppByName (6), getAppNameAtIndex (5), findAppIndexByName (6), getTimeFormat (9) |

### JSON & Stats (2 suites, 67 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_stats_builder | test_stats_builder.cpp | StatsBuilder | 13 | JSON structure (2), all keys present (1), sensor conditional (2), value formatting (8) |
| test_ha_discovery | test_ha_discovery.cpp | HADiscovery | 54 | buildEntityId (4), buildConfigUrl (3), all entity descriptors (25+), uniqueness (1), total count (3) |

### Config & Overlay (2 suites, 47 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_config | test_config.cpp | ConfigTypes | 31 | All 13 config structs: defaults, assignments, type validation |
| test_overlay | test_overlay_mapping.cpp | OverlayMapping | 16 | overlayToString (7), overlayFromString (7), case insensitivity, round-trip |

### Interface Validation (2 suites, 25 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_idisplay | test_idisplay.cpp | MockDisplay.h | 13 | IDisplayRenderer (4), IDisplayControl (4), IDisplayNavigation (4), IDisplayNotifier (3) |
| test_iservices | test_iservices.cpp | MockServices.h | 12 | ISound (3), IPower (2), IUpdater (3), IPeripheryProvider (3) |

### Effects (1 suite, 12 tests)

| Suite | File | Module | Tests | Key Scenarios |
|-------|------|--------|-------|---------------|
| test_effects | test_effects.cpp | PatternEffects, WaveEffects, GameEffects | 12 | Checkerboard (2), fade (1), moving line (1), theater chase (2), plasma (1), pacifica (1), ping-pong (1), brick breaker (1), eyes (2) |

## Test Patterns

### 1. Pure Function Testing
Extracted pure logic without hardware. Boundary values, overflow, adversarial inputs (NaN, div-by-zero).

### 2. Time-Dependent Testing
`setMockMillis(value)` controls time for fade/blink/animation tests.

### 3. Interface Mock Validation
Call tracking + parameter capture on mock implementations. Configurable return values.

### 4. Adversarial Coverage
- Degenerate math: equal ranges, inverted ranges, overflow
- Malformed input: empty strings, unclosed `{{`, invalid JSON, out-of-range enums
- State corruption: interrupted UTF-8 sequences

## File Structure

```
test/
├── mocks/
│   ├── Arduino.h            Mock Arduino String + Serial + millis()
│   ├── FastLED.h            Mock CRGB/CHSV/palettes/math
│   ├── MockDisplay.h        4 IDisplay* mock classes
│   └── MockServices.h       4 IService mock classes
└── test_native/
    ├── test_functions.cpp
    ├── test_math_utils.cpp
    ├── test_logmap.cpp
    ├── test_color_utils.cpp
    ├── test_crgb_hex.cpp
    ├── test_text_metrics.cpp
    ├── test_utf8.cpp
    ├── test_sensor_calc.cpp
    ├── test_gamma.cpp
    ├── test_time_effects.cpp
    ├── test_message_router.cpp
    ├── test_placeholder_utils.cpp
    ├── test_app_registry.cpp
    ├── test_apps_logic.cpp
    ├── test_stats_builder.cpp
    ├── test_ha_discovery.cpp
    ├── test_config.cpp
    ├── test_overlay_mapping.cpp
    ├── test_idisplay.cpp
    ├── test_iservices.cpp
    └── test_effects.cpp
```
