# MQTTManager — AI Reference

Central MQTT communication and Home Assistant integration singleton. Manages broker connection, message dispatch, HA auto-discovery (25 entities), and state synchronization.

## File Map

| File                           | LOC | Purpose                                                           |
| ------------------------------ | --- | ----------------------------------------------------------------- |
| `MQTTManager.h`                | 135 | Public API, singleton, INotifier + IButtonReporter                |
| `MQTTManager.cpp`              | 289 | Instance, connection lifecycle, tick, publish methods             |
| `MQTTManager_internal.h`       | 127 | Shared extern declarations for 25 HA entity pointers + ID buffers |
| `MQTTManager_Messages.cpp`     | 188 | Incoming message reception + command dispatch                     |
| `MQTTManager_Callbacks.cpp`    | 179 | 7 ArduinoHA callback handlers (HA → device)                       |
| `MQTTManager_Discovery.cpp`    | 245 | HA entity creation + metadata                                     |
| `MQTTManager_StateUpdates.cpp` | 165 | Stats publishing, button/indicator sync (device → HA)             |

## Interfaces

**Implements:**
- `INotifier` — publish/subscribe, setCurrentApp, setIndicatorState
- `IButtonReporter` — sendButton(btn, state)

**Injected Dependencies (7):**
```cpp
void setDisplay(IDisplayControl*, IDisplayNavigation*, IDisplayNotifier*);
void setServices(ISound*, IPower*, IUpdater*, IPeripheryProvider*);
```

## HA Entities (25 total)

| Type | Count | Entities |
|------|-------|----------|
| **HALight** | 4 | Matrix (brightness+RGB), Indicator 1/2/3 (RGB) |
| **HASelect** | 2 | BriMode (Manual/Auto), transEffect (11 transitions) |
| **HAButton** | 4 | dismiss, nextApp, prevApp, doUpdate |
| **HASwitch** | 1 | transition (auto-transition toggle) |
| **HASensor** | 10-11 | curApp, myOwnID, temp, hum, lux, signal, version, ram, uptime, ipAddr, battery* |
| **HABinarySensor** | 3 | btnleft, btnmid, btnright |

*battery only on ULANZI build

All allocated with `new` in `setup()` when `haConfig.discovery == true`. Freed by `destroyHAEntities()` at the start of each `setup()` call to prevent heap leaks on repeated invocations.

## MQTT Topics

### Incoming (20 command suffixes)
All prefixed with `<mqttConfig.prefix>/`. **HTTP equivalent** column shows the matching REST endpoint in [src/ServerManager/CLAUDE.md](../ServerManager/CLAUDE.md) — useful for cross-referencing behavior or porting commands.

| Suffix                      | Action                                      | HTTP equivalent                             |
| --------------------------- | ------------------------------------------- | ------------------------------------------- |
| `/notify`                   | `generateNotification(0, json)`             | `POST /api/notify`                          |
| `/notify/dismiss`           | `dismissNotify()`                           | `ANY /api/notify/dismiss`                   |
| `/custom/#`                 | `parseCustomPage(appName, json)` — wildcard | `POST /api/custom?name=X`                   |
| `/switch`                   | `switchToApp(json)`                         | `POST /api/switch`                          |
| `/apps`                     | `updateAppVector(json)`                     | `POST /api/apps`                            |
| `/nextapp`, `/previousapp`  | Navigation                                  | `ANY /api/nextapp`, `POST /api/previousapp` |
| `/settings`                 | `setNewSettings(json)`                      | `POST /api/settings`                        |
| `/power`                    | `setPower(doc["power"])`                    | `POST /api/power`                           |
| `/sleep`                    | `setPower(false)` + `sleep(seconds)`        | `POST /api/sleep`                           |
| `/indicator1-3`             | `indicatorParser(N, json)`                  | `POST /api/indicator1..3`                   |
| `/moodlight`                | `moodlight(json)`                           | `POST /api/moodlight`                       |
| `/rtttl`, `/sound`, `/r2d2` | Audio playback                              | `POST /api/rtttl`, `/sound`, `/r2d2`        |
| `/doupdate`                 | `checkUpdate()` + `updateFirmware()`        | `POST /api/doupdate`                        |
| `/sendscreen`               | Publish `ledsAsJson()`                      | `GET /api/screen`                           |
| `/reboot`                   | `ESP.restart()`                             | `ANY /api/reboot`                           |

**HTTP-only (no MQTT binding):** `/api/erase` (factory reset), `/api/resetSettings`, `/api/reorder`, `/api/loop`, `/api/stats`, `/api/effects`, `/api/transitions`, `/api/datafetcher*`, `/version`, `/save`. If MQTT access is needed for these, add a new `CMD_*` enum in `MessageRouter` + handler in `onMqttMessage`.

Routing via `MessageRouter::routeTopic()` → `MqttCommandType` enum → switch dispatch.

### Outgoing
| Topic | Trigger |
|-------|---------|
| `<prefix>/stats` | Periodic (every `statsInterval` ms) |
| `<prefix>/stats/currentApp` | On app change (deduplicated) |
| `<prefix>/stats/effects` | On connect |
| `<prefix>/stats/transitions` | On connect |
| `<prefix>/button/left\|select\|right` | On hardware press (debounced) |

### External Topic Caching
- `subscribe(topic)` — registers in `mqttValues` map, subscribes on connect
- `getValueForTopic(topic)` → cached value or "N/A"
- Used by custom apps: `{{topic}}` placeholders resolved via PlaceholderUtils

## 7 Callback Handlers (MQTTManager_Callbacks.cpp)

| Callback | Entities | Action |
|----------|----------|--------|
| `onButtonCommand` | dismiss, nextApp, prevApp, doUpdate | Dispatch to managers |
| `onSwitchCommand` | transition | Toggle autoTransition + save |
| `onSelectCommand` | BriMode, transEffect | Set mode/effect + save |
| `onRGBColorCommand` | Matrix, Indicator1-3 | Set color + save |
| `onStateCommand` | Matrix, Indicator1-3 | Set power/state |
| `onBrightnessCommand` | Matrix | Set brightness (skip if auto) + save |
| `onNumberCommand` | scrollSpeed | Set scroll speed + save |

All callbacks call `saveSettings()` after modifying config structs.

## Connection Lifecycle

```
setup() → destroyHAEntities() + resetDevicesCount() → create 25 HA entities (if discovery enabled) → register callbacks
  └→ connect() → mqtt.begin(host, port, user, pass)
       └→ onMqttConnected()
            ├─ Subscribe to ~20 command topics (30ms delay each)
            ├─ Subscribe to deferred external topics
            ├─ Publish effects, transitions, version, deviceID
            └─ connected = true
```

Reconnection handled internally by ArduinoHA. `onMqttConnected()` re-runs on each reconnect.

## Services Used (from lib/services/)

- `MessageRouter` — topic → command type routing
- `HADiscovery` — entity descriptors + ID generation
- `StatsBuilder` — telemetry → JSON
- `PlaceholderUtils` — `{{topic}}` substitution

## Tests

| Test Suite | What It Covers |
|------------|---------------|
| `test_ha_memory/` | destroy-and-reset pattern: `destroyHAEntities()` + `resetDevicesCount()` |
| `test_ha_discovery/` | HADiscovery service: entity descriptors, ID generation |

## Key Design Patterns

- **Singleton** with setter injection + assert guards
- **Split implementation** across 7 files by domain
- **Extern globals** for 25+ HA entity pointers (MQTTManager_internal.h)
- **Debounced button reporting** via static state tracking
- **Deferred subscriptions** — topics queued before connect, applied in onMqttConnected()
- **Config persistence** — all callback handlers call `saveSettings()` immediately
- **Stability gate** — `sendStats()` only runs if `sensorConfig.sensorsStable == true`

## Wiring in main.cpp

```cpp
PeripheryManager.addButtonReporter(&MQTTManager);         // IButtonReporter
MQTTManager.setDisplay(&DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
MQTTManager.setServices(&PeripheryManager, &PowerManager, &UpdateManager, &PeripheryManager);
// Only initialized if MQTT configured:
if (mqttConfig.host != "") { MQTTManager.setup(); MQTTManager.tick(); }
```

## Important Constraints

- Max 26 entities (hardcoded in `HAMqtt` constructor)
- `MQTT_MAX_PACKET_SIZE=8192` (platformio.ini build flag)
- Firmware runs fully functional without MQTT if `mqttConfig.host == ""`
- Entity pointers remain `nullptr` if HA discovery disabled
