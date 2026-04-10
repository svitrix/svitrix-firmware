# Home Assistant Integration — AI Reference

Local fork of [arduino-home-assistant](https://github.com/dawidchyrzynski/arduino-home-assistant) v2.0.0 by Dawid Chyrzynski, trimmed for Svitrix.

## What This Library Does

MQTT-based Home Assistant auto-discovery. The device publishes config messages → HA auto-creates entities in its UI. Two-way: state reports (device → HA) + command callbacks (HA → device).

## Enabled Entity Types (7 of 17)

| Class | HA Type | Used For |
|-------|---------|----------|
| `HALight` | light | Matrix backlight, 3 indicator LEDs (brightness, RGB, color temp) |
| `HASwitch` | switch | Auto-transition toggle |
| `HAButton` | button | Dismiss, next/prev app, doUpdate |
| `HASensor` | sensor | Battery, temp, humidity, lux, uptime, signal, version, RAM, current app, device ID, IP |
| `HANumber` | number | Numeric settings with min/max/step |
| `HASelect` | select | Brightness mode, transition effect dropdowns |
| `HABinarySensor` | binary_sensor | 3 hardware buttons (left/mid/right) |

10 types **excluded** via `EX_ARDUINOHA_*` defines in `ArduinoHADefines.h`: Camera, Cover, DeviceTracker, DeviceTrigger, Fan, HVAC, Lock, Scene, SensorNumber, TagScanner.

## Class Hierarchy

```
HAMqtt (singleton, wraps PubSubClient)
  └── manages → HADevice (MAC, hostname, version, availability)
  └── manages → HABaseDeviceType* array (max 26 entities)
                  ├── HALight
                  ├── HASwitch
                  ├── HAButton
                  ├── HASensor
                  ├── HANumber
                  ├── HASelect
                  └── HABinarySensor
```

**Utilities:** `HASerializer` (JSON config builder), `HANumeric` (fixed-precision numbers), `HADictionary` (string key-value store).

## How Svitrix Uses It

All integration lives in `src/MQTTManager/` (5 files):

| File | Role |
|------|------|
| `MQTTManager.cpp` | Global `HADevice`, `HAMqtt`, entity pointer declarations |
| `MQTTManager_Discovery.cpp` | Creates 25+ entities with `new HALight(...)` etc. |
| `MQTTManager_Callbacks.cpp` | Inbound command handlers (HA → device) |
| `MQTTManager_StateUpdates.cpp` | Outbound state publishing (device → HA) |
| `MQTTManager_internal.h` | Shared `extern` declarations |

**Global objects** (in MQTTManager.cpp):
```cpp
WiFiClient espClient;
HADevice device;
HAMqtt mqtt(espClient, device, 26); // max 26 entities
```

## Message Flow

```
HA UI → MQTT Broker → PubSubClient → HAMqtt → entity.onCommand(callback)
                                                       ↓
                                              MQTTManager_Callbacks.cpp

Device state change → entity.setState() → HAMqtt.publish() → HA UI
```

## Key Patterns

- **Compile-time exclusion** — `#define EX_ARDUINOHA_CAMERA` etc. in `ArduinoHADefines.h`
- **PROGMEM strings** — `const __FlashStringHelper*` for flash storage
- **Callbacks** — `entity->onStateCommand(fn)` for inbound commands
- **Availability** — `HADevice::enableSharedAvailability()` + LWT on disconnect
- **Lazy serialization** — `buildSerializer()` called only on MQTT connect

## Important Constraints

- **Max 26 entities** — hardcoded in `HAMqtt` constructor (3rd param)
- **PubSubClient dependency** — `MQTT_MAX_PACKET_SIZE=8192` set in platformio.ini
- **Minimal modifications to library source** — `ArduinoHADefines.h` exclusions + `HAMqtt::resetDevicesCount()` added to support safe entity re-creation
- **Memory safe** — entity pointers freed by `destroyHAEntities()` before re-allocation in `MQTTManager::setup()`

## Files You Should NOT Edit

Everything under `src/` in this library is upstream code. To customize behavior:
1. Toggle entity types via `EX_ARDUINOHA_*` defines in `ArduinoHADefines.h`
2. Change integration logic in `src/MQTTManager/` (firmware side), not here
