# Config Library — AI Reference

Shared configuration structs consumed by all modules. Single header, no logic, no dependencies beyond Arduino `String`.

## Files

| File | Purpose |
|------|---------|
| `src/ConfigTypes.h` | 13 config structs + 1 enum, all POD-style with public fields |

## Config Structs

### AuthConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `user` | String | `""` | HTTP username |
| `pass` | String | `"svitrix"` | HTTP password |

### NetworkConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `isStatic` | bool | `false` | Use static IP |
| `ip` | String | `"192.168.178.10"` | Static IP address |
| `gateway` | String | `"192.168.178.1"` | Gateway |
| `subnet` | String | `"255.255.255.0"` | Subnet mask |
| `primaryDns` | String | `"8.8.8.8"` | Primary DNS |
| `secondaryDns` | String | `"1.1.1.1"` | Secondary DNS |

### BatteryConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `percent` | uint8_t | `0` | Calculated battery % |
| `raw` | uint16_t | `0` | Current ADC reading |
| `minRaw` | uint16_t | `475` | ADC value = 0% |
| `maxRaw` | uint16_t | `665` | ADC value = 100% |

### HaConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `discovery` | bool | `false` | Enable HA discovery |
| `prefix` | String | `"homeassistant"` | HA discovery topic prefix |

### MqttConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `host` | String | `""` | Broker hostname/IP |
| `port` | uint16_t | `1883` | Broker port |
| `user` | String | `""` | MQTT username |
| `pass` | String | `""` | MQTT password |
| `prefix` | String | `""` | Topic prefix (set to deviceId at runtime) |

### SensorConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `currentTemp` | float | `0` | Current temperature reading |
| `currentHum` | float | `0` | Current humidity reading |
| `currentLux` | float | `0` | Current lux reading |
| `ldrRaw` | uint16_t | `0` | Raw LDR ADC value |
| `tempSensorType` | uint8_t | `TEMP_SENSOR_TYPE_NONE` | Detected sensor type |
| `sensorReading` | bool | `true` | Enable sensor polling |
| `sensorsStable` | bool | `false` | Sensors have stabilized |
| `tempOffset` | float | `-9` | Temperature calibration offset |
| `humOffset` | float | `0` | Humidity calibration offset |

### TempSensorType (enum)

| Value | Constant |
|-------|----------|
| 0 | `TEMP_SENSOR_TYPE_NONE` |
| 1 | `TEMP_SENSOR_TYPE_BME280` |
| 2 | `TEMP_SENSOR_TYPE_HTU21DF` |
| 3 | `TEMP_SENSOR_TYPE_BMP280` |
| 4 | `TEMP_SENSOR_TYPE_SHT31` |

### DisplayConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `matrixLayout` | int | `0` | NeoMatrix layout flag |
| `matrixFps` | uint8_t | `42` | Target FPS |
| `matrixOff` | bool | `false` | Display powered off |
| `mirrorDisplay` | bool | `false` | Mirror horizontally |
| `rotateScreen` | bool | `false` | Rotate 180 degrees |
| `uppercaseLetters` | bool | `true` | Force uppercase text |
| `backgroundEffect` | int | `-1` | Active background effect index (-1 = none) |

### BrightnessConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `brightness` | int | `120` | Current brightness (0-255) |
| `brightnessPercent` | int | `0` | Brightness as percentage |
| `autoBrightness` | bool | `true` | Enable LDR auto-brightness |
| `minBrightness` | uint8_t | `2` | Auto-brightness floor |
| `maxBrightness` | uint8_t | `160` | Auto-brightness ceiling |
| `ldrGamma` | float | `3.0` | LDR gamma curve exponent |
| `ldrFactor` | float | `1.0` | LDR scaling factor |
| `ldrOnGround` | bool | `false` | LDR mounted on ground side |

### ColorConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `textColor` | uint32_t | `0xFFFFFF` | Default text color (white) |
| `timeColor` | uint32_t | `0` | Time app color (0 = use textColor) |
| `dateColor` | uint32_t | `0` | Date app color |
| `batColor` | uint32_t | `0` | Battery app color |
| `tempColor` | uint32_t | `0` | Temperature app color |
| `humColor` | uint32_t | `0` | Humidity app color |
| `wdcActive` | uint32_t | `0xFFFFFF` | Weekday indicator active |
| `wdcInactive` | uint32_t | `0x666666` | Weekday indicator inactive |
| `calendarHeaderColor` | uint32_t | `0xFF0000` | Calendar header (red) |
| `calendarTextColor` | uint32_t | `0x000000` | Calendar text (black) |
| `calendarBodyColor` | uint32_t | `0xFFFFFF` | Calendar body (white) |

### TimeConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `timeFormat` | String | `"%H:%M:%S"` | strftime format for time |
| `dateFormat` | String | `"%d.%m.%y"` | strftime format for date |
| `timeMode` | uint8_t | `1` | 0 = 12h, 1 = 24h |
| `startOnMonday` | bool | `false` | Week starts on Monday |
| `ntpServer` | String | `"de.pool.ntp.org"` | NTP server |
| `ntpTz` | String | `"CET-1CEST,M3.5.0,M10.5.0/3"` | POSIX timezone |
| `isCelsius` | bool | `false` | Temperature unit |
| `tempDecimalPlaces` | int | `0` | Decimal places for temp display |

### AppConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `showTime` | bool | `true` | Show time app |
| `showDate` | bool | `true` | Show date app |
| `showBat` | bool | `true` | Show battery app (Ulanzi only) |
| `showTemp` | bool | `true` | Show temperature app |
| `showHum` | bool | `true` | Show humidity app |
| `showWeekday` | bool | `true` | Show weekday indicator |
| `autoTransition` | bool | `false` | Auto-cycle through apps |
| `transEffect` | int8_t | `1` | Transition effect index |
| `timePerTransition` | int | `400` | Transition duration (ms) |
| `timePerApp` | long | `7000` | Time per app before auto-transition (ms) |
| `scrollSpeed` | uint8_t | `100` | Text scroll speed (ms per pixel) |
| `nativeIconLayout` | IconLayout | `Left` | Icon position for native apps |
| `blockNavigation` | bool | `false` | Block button navigation |

### AudioConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `soundActive` | bool | `false` | Enable sound |
| `soundVolume` | uint8_t | `30` | Volume level |
| `bootSound` | String | `""` | RTTTL melody on boot |

### SystemConfig
| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `debugMode` | bool | `true` | Enable debug serial output |
| `apTimeout` | uint32_t | `15` | AP mode timeout (seconds) |
| `webPort` | int | `80` | HTTP server port |
| `hostname` | String | `""` | mDNS hostname (set to deviceId) |
| `updateCheck` | bool | `false` | Check for firmware updates |
| `statsInterval` | long | `10000` | MQTT stats publish interval (ms) |
| `newyear` | bool | `false` | New Year countdown mode |
| `swapButtons` | bool | `false` | Swap left/right buttons |
| `buttonCallback` | String | `""` | HTTP URL for button press callback |
| `deviceId` | String | `""` | Unique device ID (from MAC) |
| `updateAvailable` | bool | `false` | Runtime: OTA update available |
| `apMode` | bool | `false` | Runtime: in AP mode |
| `updateVersionUrl` | String | `""` | Custom update version check URL |
| `updateFirmwareUrl` | String | `""` | Custom firmware download URL |

## Persistence

Config values come from **three layers** (later overrides earlier):

1. **Compile-time defaults** — initializer lists in `Globals.cpp`
2. **NVS (Preferences)** — `loadSettings()` / `saveSettings()` (namespace `"svitrix"`)
3. **`/dev.json` (LittleFS)** — `loadDevSettings()` overrides (highest priority)

## Who Uses What

| Config Struct | Consumers |
|---------------|-----------|
| `appConfig` | DisplayManager, MQTTManager, ServerManager, MenuManager, Apps, MatrixDisplayUi |
| `displayConfig` | DisplayManager, MQTTManager, ServerManager, MenuManager, DisplayRenderer |
| `brightnessConfig` | DisplayManager, PeripheryManager, MQTTManager, ServerManager, MenuManager |
| `colorConfig` | DisplayManager, DisplayRenderer, Apps, MQTTManager, ServerManager, MenuManager |
| `timeConfig` | Apps, MQTTManager, ServerManager, MenuManager, Overlays |
| `sensorConfig` | PeripheryManager, Apps, MQTTManager, ServerManager |
| `systemConfig` | Nearly all modules |
| `mqttConfig` | MQTTManager, ServerManager |
| `audioConfig` | ServerManager, MQTTManager, MenuManager, PeripheryManager |
| `batteryConfig` | PeripheryManager, Apps, MQTTManager |
| `haConfig` | MQTTManager |
| `networkConfig` | main.cpp (WiFi setup) |
| `authConfig` | ServerManager |

## Design Notes

- Structs are plain C++ aggregates (no constructors, no methods)
- All 13 instances declared `extern` in `Globals.h`, defined in `Globals.cpp`
- Color fields use `uint32_t` (`0xRRGGBB`), not `CRGB` — avoids FastLED dependency
- Some fields serve double duty as runtime state (e.g., `sensorConfig.currentTemp`)
- Only dependency: `Arduino.h` (for `String` type)
- Compiles in both `ulanzi` (ESP32) and `native_test` environments
