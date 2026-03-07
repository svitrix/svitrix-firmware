# MQTTManager Block — AI Reference

> MQTT broker communication + Home Assistant auto-discovery. Uses ArduinoHA library.

## Files

| File | Purpose |
|------|---------|
| `MQTTManager.h` | Public API, INotifier + IButtonReporter interfaces |
| `MQTTManager_internal.h` | Shared state, HA entity pointers, extern refs |
| `MQTTManager.cpp` | Singleton, connection lifecycle, tick, publish |
| `MQTTManager_Messages.cpp` | Incoming message reception + command dispatch |
| `MQTTManager_Discovery.cpp` | HA entity creation (25 entities) |
| `MQTTManager_Callbacks.cpp` | 7 HA callback handlers |
| `MQTTManager_StateUpdates.cpp` | Stats/button/indicator publishing |

## Class: MQTTManager_

**Pattern**: Singleton
**Implements**: `INotifier`, `IButtonReporter`
**Injected**: `IDisplayControl*`, `IDisplayNavigation*`, `IDisplayNotifier*`, `ISound*`, `IPower*`, `IUpdater*`, `IPeripheryProvider*`

## MQTT Topics

### Incoming (23 subscriptions)

All prefixed with `<mqttConfig.prefix>/`:

| Suffix | Command Type | Handler |
|--------|-------------|---------|
| `/notify` | CMD_NOTIFY | `dmNotify_->generateNotification(0, json)` |
| `/notify/dismiss` | CMD_NOTIFY_DISMISS | `dmNotify_->dismissNotify()` |
| `/custom/#` | CMD_CUSTOM | `dmNav_->parseCustomPage(name, json)` |
| `/switch` | CMD_SWITCH | `dmNav_->switchToApp(json)` |
| `/apps` | CMD_APPS | `dmNav_->updateAppVector(json)` |
| `/nextapp` | CMD_NEXT_APP | `dmNav_->nextApp()` |
| `/previousapp` | CMD_PREVIOUS_APP | `dmNav_->previousApp()` |
| `/settings` | CMD_SETTINGS | `dmControl_->setNewSettings(json)` |
| `/power` | CMD_POWER | `dmControl_->setPower(doc["power"])` |
| `/sleep` | CMD_SLEEP | `dmControl_->setPower(false)` + `mqttPower_->sleep(sec)` |
| `/indicator1-3` | CMD_INDICATOR* | `dmNotify_->indicatorParser(id, json)` |
| `/moodlight` | CMD_MOODLIGHT | `dmControl_->moodlight(json)` |
| `/rtttl` | CMD_RTTTL | `mqttSound_->playRTTLString(payload)` |
| `/sound` | CMD_SOUND | `mqttSound_->parseSound(json)` |
| `/r2d2` | CMD_R2D2 | `mqttSound_->r2d2(payload)` |
| `/doupdate` | CMD_DO_UPDATE | `mqttUpdater_->checkUpdate()` + `updateFirmware()` |
| `/sendscreen` | CMD_SEND_SCREEN | publish `control->ledsAsJson()` |
| `/reboot` | CMD_REBOOT | `ESP.restart()` |
| `/brightness` | — | Brightness command |
| `/timeformat` | — | Time format |
| `/dateformat` | — | Date format |

### Outgoing (published)

| Topic | Payload | Trigger |
|-------|---------|---------|
| `<prefix>/stats` | Aggregated JSON | Periodic (statsInterval) |
| `<prefix>/stats/currentApp` | App name | On app change |
| `<prefix>/stats/effects` | JSON array | On connect |
| `<prefix>/stats/transitions` | JSON array | On connect |
| `<prefix>/button/left\|select\|right` | "1"/"0" | On button press/release |
| `<prefix>/screen` | LED JSON | On sendscreen request |

### External Topic Caching

- `subscribe(topic)` → subscribes to external MQTT topic, stores value in `mqttValues` map
- `getValueForTopic(topic)` → returns cached value ("N/A" if unknown)
- Used by `{{topic}}` placeholders in custom apps

## Home Assistant Entities (25 total)

### Lights (4)
| Entity | ID | Features |
|--------|-----|----------|
| Matrix | `%s_mat` | Brightness (0-255) + RGB |
| Indicator 1 | `%s_ind1` | RGB only |
| Indicator 2 | `%s_ind2` | RGB only |
| Indicator 3 | `%s_ind3` | RGB only |

### Selects (2)
| Entity | Options |
|--------|---------|
| Brightness mode | Manual; Auto |
| Transition effect | Random; Slide; Dim; Zoom; Rotate; Pixelate; Curtain; Ripple; Blink; Reload; Fade |

### Buttons (4)
Dismiss notification, Start Update, Next app, Previous app

### Switch (1)
Auto-transition toggle

### Sensors (10-11)
Current app, Device topic, Temperature, Humidity, Battery (optional), Illuminance, Version, WiFi signal, Uptime, Free RAM, IP Address

### Binary Sensors (3)
Button left, Button select, Button right

## HA Callbacks (MQTTManager_Callbacks.cpp)

| Callback | Handles |
|----------|---------|
| `onButtonCommand` | dismiss, nextApp, prevApp, doUpdate |
| `onSwitchCommand` | Auto-transition toggle + saveSettings |
| `onSelectCommand` | Brightness mode (Manual/Auto), Transition effect |
| `onRGBColorCommand` | Matrix text color + custom app colors, Indicator colors |
| `onStateCommand` | Matrix power on/off, Indicator on/off |
| `onBrightnessCommand` | Matrix brightness (skip if auto-brightness) |
| `onNumberCommand` | Scroll speed number input |

## Message Routing

```
onMqttMessage(topic, payload, length)
  → processMqttMessage(strTopic, payloadCopy)
    → routeTopic(topic, prefix) → MqttCommandType enum
    → switch(commandType) → dispatch to manager
    → If CMD_UNKNOWN and topic in mqttValues → cache value
```

## Stats Publishing (periodic)

Updated HA sensors: battery, temperature, humidity, illuminance, brightness mode, matrix state/brightness/color, WiFi RSSI, free RAM, uptime, auto-transition, IP address.
