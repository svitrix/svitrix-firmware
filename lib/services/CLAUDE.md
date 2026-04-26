# Services Library — AI Reference

15 pure-logic utility libraries extracted from managers for testability. All stateless (except TextUtils), no hardware dependencies, 100% test coverage.

## Service Map

| Service | Purpose | Stateless | Key Methods |
|---------|---------|-----------|-------------|
| **ColorUtils** | Color space: hex↔uint32, Kelvin→RGB, HSV→RGB, lerp | Yes | `hexToUint32()`, `kelvinToRGB()`, `interpolateColor()`, `CRGBtoHex()` |
| **GammaUtils** | LED gamma exponent (brightness-dependent log curve) | Yes | `calculateGamma(brightness)` |
| **MathUtils** | Decimal rounding, two-segment log mapping | Yes | `roundToDecimalPlaces()`, `logMap()` |
| **TextUtils** | Text width measurement (UTF-8 aware, bitmap font) | **No** — global `activeFont` | `setTextFont()`, `getTextWidth()` |
| **UnicodeFont** | Glyph lookup, UTF-8 decode, bitmap rendering | Yes | `findGlyph()`, `utf8NextCodepoint()`, `renderGlyph()`, `getUnicodeTextWidth()` |
| **TimeEffects** | Sine fade, square blink (millis passed as param) | Yes | `fadeColorAt()`, `textEffectAt()` |
| **SensorCalc** | Battery %, brightness, LDR inversion, calibration | Yes | `calculateBatteryPercent()`, `calculateBrightness()`, `applySensorOffset()` |
| **StatsBuilder** | Device telemetry → JSON string (no ArduinoJson) | Yes | `buildStatsJson(StatsData&)` |
| **AppRegistry** | Native app names, app list serialization | Yes | `getNativeAppNames()`, `isNativeApp()`, `serializeAppList()` |
| **MessageRouter** | MQTT topic → command enum routing | Yes | `routeTopic()`, `extractCustomTopicName()`, `isJsonPayload()` |
| **HADiscovery** | HA entity descriptors and ID generation | Yes | `get*Descriptors()`, `buildEntityId()`, `getTotalEntityCount()` |
| **OverlayMapping** | Weather overlay enum ↔ string | Yes | `overlayToString()`, `overlayFromString()` |
| **PlaceholderUtils** | `{{key}}` template substitution via callback | Yes | `replacePlaceholdersWith(input, getValue)` |
| **LayoutEngine** | Icon/text position: left, right, none layouts | Yes | `computeLayout()`, `layoutToString()`, `layoutFromString()` |
| **FormatStringValidator** | Whitelist for user-supplied printf format strings (CWE-134 defense) | Yes | `isSafeSingleArgFormat(fmt)` |

## Dependency Graph

```
MathUtils ← GammaUtils
          ← SensorCalc

UnicodeFont ← TextUtils

ColorUtils (standalone, depends only on FastLED CRGB)
TimeEffects (standalone, uses cmath sin)
StatsBuilder (standalone, manual String building)
AppRegistry (standalone)
MessageRouter (standalone)
HADiscovery (standalone)
OverlayMapping (standalone)
PlaceholderUtils (standalone)
LayoutEngine (standalone)
FormatStringValidator (standalone, header-only)
```

No inter-service circular dependencies. Most services are fully standalone.

## Who Uses What

| Consumer | Services Used |
|----------|--------------|
| DisplayManager | ColorUtils, TimeEffects, GammaUtils, TextUtils, StatsBuilder, OverlayMapping |
| DisplayRenderer | TextUtils, UnicodeFont, ColorUtils |
| PeripheryManager | SensorCalc |
| MQTTManager | MessageRouter, HADiscovery, AppRegistry, StatsBuilder, PlaceholderUtils |
| Apps | ColorUtils, TimeEffects, TextUtils |
| DisplayManager, Apps, Overlays | LayoutEngine |
| DataFetcher | FormatStringValidator |
| main.cpp | TextUtils (`setTextFont(SvitrixFont)` — must be called at startup) |

## Test Coverage

Every service has dedicated tests in `test/test_native/`:

| Service | Test Suite(s) |
|---------|--------------|
| ColorUtils | `test_color/`, `test_crgb_hex/` |
| GammaUtils | `test_gamma/` |
| MathUtils | `test_math/`, `test_logmap/` |
| TextUtils | `test_text_metrics/` |
| UnicodeFont | `test_unicode_font/`, `test_glyph_render/`, `test_utf8/` |
| TimeEffects | `test_time_effects/`, `test_effects/` |
| SensorCalc | `test_sensor_calc/` |
| StatsBuilder | `test_stats_builder/` |
| AppRegistry | `test_app_registry/` |
| MessageRouter | `test_message_router/` |
| HADiscovery | `test_ha_discovery/` |
| OverlayMapping | `test_overlay/` |
| PlaceholderUtils | `test_placeholder_utils/` |
| LayoutEngine | `test_layout_engine/` |
| FormatStringValidator | `test_format_validator/` |

Run all: `pio test -e native_test`

## Design Patterns

- **Pure functions** — deterministic, no side effects (TimeEffects takes `currentMillis` as param instead of calling `millis()`)
- **Callback injection** — PlaceholderUtils and UnicodeFont use `std::function` / function pointers for flexibility
- **Manual JSON** — StatsBuilder, AppRegistry build JSON strings manually (no ArduinoJson dependency — keeps native tests simple)
- **Bit-packed structs** — UnicodeFont `UniGlyph` is 5 bytes (height/xAdvance/yOffset packed into single byte)
- **Binary search** — `findGlyph()` uses binary search over sorted codepoint array
- **Config structs** — Services consume types from `lib/config/src/ConfigTypes.h` (BatteryConfig, SensorConfig, MqttConfig, etc.)

## Rules for Adding New Services

1. Keep stateless — pass time/state as parameters, don't call `millis()` or read globals
2. Add tests in `test/test_native/test_<name>/` — maintain 100% coverage
3. Use Arduino `String` for compatibility but prefer `const char*` params where possible
4. No ArduinoJson — build JSON manually for native test compatibility
5. No hardware includes — services must compile in `native_test` environment
6. Add the service to this table when created
