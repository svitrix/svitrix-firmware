# Services — AI Reference

> 12 pure-logic libraries extracted from managers for testability.
> No hardware dependencies. 100% test coverage. Run on host via `native_test`.

## Service Table

| Service | Tests | Purpose |
|---------|-------|---------|
| `ColorUtils` | test_color, test_crgb_hex | Color conversion & interpolation |
| `GammaUtils` | test_gamma | Brightness-dependent gamma calculation |
| `TextUtils` | test_text_metrics, test_utf8 | UTF-8 text width, char mapping |
| `MathUtils` | test_math | Map, constrain, log-scale |
| `TimeEffects` | test_time_effects | Fade, blink animations |
| `SensorCalc` | test_sensor_calc | LDR brightness, battery % |
| `StatsBuilder` | test_stats_builder | Device stats JSON |
| `AppRegistry` | test_app_registry | App name lookups |
| `MessageRouter` | test_message_router | MQTT topic → command routing |
| `HADiscovery` | test_ha_discovery | HA MQTT discovery payloads |
| `OverlayMapping` | test_overlay | Overlay name ↔ enum |
| `PlaceholderUtils` | test_placeholder | `{{topic}}` substitution |

## ColorUtils

```cpp
uint32_t hexToUint32(const char *hexString);
// "FF00AA" → 0xFF00AA via strtol(base 16)

CRGB kelvinToRGB(int kelvin);
// Tanner Helland blackbody radiation, 1000-10000K
// <6600K: R=255, G=log, B=computed
// >6600K: R=power fn, G=drops, B=255

uint32_t hsvToRgb(uint8_t h, uint8_t s, uint8_t v);
// h: 0-255 (0=red, 85=green, 170=blue)

uint32_t interpolateColor(uint32_t c1, uint32_t c2, float t);
// Per-channel linear lerp, t clamped [0..1]

String CRGBtoHex(CRGB color);
// → "#RRGGBB" (7 chars)
```

## GammaUtils

```cpp
float calculateGamma(float brightness);
// Two-segment log curve: logMap(bri, 2, 180, 0.535, 2.3, 1.9)
// bri<2 → 0.535, bri≈90 → 1.9, bri>180 → 2.3
```

## TextUtils

```cpp
float getTextWidth(const char *text, byte textCase, bool uppercaseLetters);
// Sum per-character widths from CharMap (180+ entries)
// Most chars: 4px, narrow (space, I, i): 2px, wide (M, W): 6px
// Cyrillic fallback widths: Ш/Щ/Ж/Ы=6px, Ф=7px, Д/Л=5px

byte utf8ascii(byte ascii);
// Stateful UTF-8 → single-byte decoder
// Supports: Latin supplement/extended, Polish, Cyrillic, Euro sign
// Multi-byte: first byte stored, continuation triggers mapping

String utf8ascii(String s);
// Batch-convert UTF-8 String
```

### Cyrillic Mapping (D0/D1 prefix)
- Uppercase (D0): А-П → subtract 17, Р-Я → subtract 49
- Lowercase (D1): а-п → add 15
- Special: Ё→0x84, Є→0xA0, І→0xA1, Ї→0xEF, Ґ→0x9F

## MathUtils

```cpp
double roundToDecimalPlaces(double value, int places);
// round(value × 10^places) / 10^places

float logMap(float x, float in_min, float in_max, float out_min, float out_max, float mid_point_out);
// Two-segment logarithmic curve
// Lower half: ln-scale from out_min to mid_point_out
// Upper half: ln-scale from mid_point_out to out_max
```

## TimeEffects

```cpp
uint32_t fadeColorAt(uint32_t color, uint32_t interval, uint32_t currentMillis);
// Sine-wave brightness: (sin(2π × t/interval) + 1) / 2
// interval=0 → returns color unchanged

uint32_t textEffectAt(uint32_t color, uint32_t fade, uint32_t blink, uint32_t currentMillis);
// Priority: fade > blink > static
// Fade: sine-wave modulation (period=fade ms)
// Blink: square-wave toggle (period=blink ms, blink<2 always OFF)
```

## SensorCalc

```cpp
uint8_t calculateBatteryPercent(uint16_t adc, uint16_t minBat, uint16_t maxBat);
// Linear map to [0..100], defaults: min=475, max=665

uint8_t calculateBrightness(uint16_t ldrRaw, float ldrFactor, float ldrGamma,
                            uint8_t minBri, uint8_t maxBri);
// 1. Normalize: raw × factor / 1023 × 100
// 2. Gamma: pow(pct, gamma) / pow(100, gamma-1)
// 3. Map to [minBri..maxBri]

uint16_t applyLdrInversion(uint16_t ldrRaw, bool ldrOnGround);
// Ground-wired LDR: 1023 - raw

float applySensorOffset(float rawValue, float offset);
// rawValue + offset (temperature/humidity calibration)

uint8_t scaleVolume(uint8_t vol, uint8_t inputMax, uint8_t outputMax);
// Linear: (vol × outputMax) / inputMax
```

## StatsBuilder

```cpp
struct StatsData {
  uint8_t batteryPercent; uint16_t batteryRaw;
  int lux; uint16_t ldrRaw; uint32_t freeRam;
  uint8_t brightness; bool hasSensor;
  float temperature; uint8_t tempDecimalPlaces;
  uint8_t humidity; String uptime;
  int8_t wifiSignal; uint32_t receivedMessages;
  String version; bool indicator1/2/3;
  String currentApp, uid; bool matrixOn;
  String ipAddress;
};

String buildStatsJson(const StatsData& data);
// Manual String building (no ArduinoJson), fields: bat, bat_raw, lux, ldr_raw,
// ram, bri, temp, hum, uptime, wifi_signal, messages, version, indicator1-3,
// app, uid, matrix, ip_address
```

## AppRegistry

```cpp
std::vector<String> getNativeAppNames();
// → {"Time", "Date", "Temperature", "Humidity", "Battery"}

bool isNativeApp(const String& name);
// Case-sensitive match against 5 native app names

String serializeAppList(const std::vector<String>& names);
// Manual JSON: {"Time":0,"Date":1,...}
```

## MessageRouter

```cpp
enum MqttCommandType {
  CMD_UNKNOWN, CMD_NOTIFY, CMD_NOTIFY_DISMISS, CMD_DO_UPDATE,
  CMD_APPS, CMD_SWITCH, CMD_SEND_SCREEN, CMD_SETTINGS,
  CMD_R2D2, CMD_NEXT_APP, CMD_PREVIOUS_APP, CMD_RTTTL,
  CMD_POWER, CMD_SLEEP, CMD_INDICATOR1/2/3, CMD_MOODLIGHT,
  CMD_REBOOT, CMD_SOUND, CMD_CUSTOM
};

MqttCommandType routeTopic(const String& topic, const String& prefix);
// Topic suffix matching, /custom/# wildcard

String extractCustomTopicName(const String& topic, const String& prefix);
// "prefix/custom/MyApp" → "MyApp"

bool isJsonPayload(const String& payload);
// Starts with '{', ends with '}'

std::vector<String> getSubscriptionTopics();
// 23 canonical subscription suffixes
```

## HADiscovery

```cpp
struct HAEntityDescriptor {
  const char *idTemplate;   // "%s_mat"
  const char *name;         // "Matrix"
  const char *icon;         // "mdi:clock-digital"
  const char *deviceClass;  // "temperature"
  const char *unit;         // "°C"
};

// Descriptors for 25 HA entities:
// 4 lights (Matrix + 3 indicators)
// 2 selects (brightness mode, transition effect)
// 4 buttons (dismiss, update, next, prev)
// 1 switch (auto-transition)
// 10-11 sensors (app, id, temp, hum, bat?, lux, version, wifi, uptime, ram, ip)
// 3 binary sensors (buttons)

void buildEntityId(idTemplate, macStr, outBuf, bufSize);
void buildConfigUrl(ip[4], outBuf, bufSize);
const char *getDeviceManufacturer(); // "Blueforcer"
const char *getDeviceModel();        // "SVITRIX"
size_t getTotalEntityCount(bool includeBattery); // 24 or 25
```

## OverlayMapping

```cpp
enum OverlayEffect { NONE, DRIZZLE, RAIN, SNOW, STORM, THUNDER, FROST };

String overlayToString(OverlayEffect);    // NONE→"clear", RAIN→"rain", etc.
OverlayEffect overlayFromString(String);  // Case-insensitive parse
```

## PlaceholderUtils

```cpp
String replacePlaceholdersWith(const String &input,
                               const std::function<String(const String&)> &getValue);
// Find "{{key}}", call getValue(key), replace token
// Callback injected for testability
```

## ConfigTypes (lib/config/src/ConfigTypes.h)

13 config structs: AuthConfig, NetworkConfig, BatteryConfig, HaConfig, MqttConfig,
SensorConfig (+ TempSensorType enum), DisplayConfig, BrightnessConfig, ColorConfig,
TimeConfig, AppConfig, AudioConfig, SystemConfig.

See `src/MODULES.md` → Globals section for full field list.
