# PowerManager

Deep sleep controller for the ESP32. Puts the device into `esp_deep_sleep`
with configurable timer and GPIO wake sources.

## Key Files

| File | Purpose |
|------|---------|
| `PowerManager.h` | Singleton, implements `IPower` |
| `PowerManager.cpp` | Sleep logic, JSON parser, wake source configuration |

## Wake Sources

| Source | Details |
|--------|---------|
| **Timer** | `esp_sleep_enable_timer_wakeup(seconds * 1e6)` |
| **GPIO 27** | `esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, LOW)` -- middle button |

## Interfaces

- **Provides:** `IPower` to ServerManager, MQTTManager
- **Consumes:** none -- uses ESP-IDF sleep APIs directly

## Detailed Reference

See the PowerManager section in [src/CLAUDE.md](../CLAUDE.md) for the
`sleepParser()` JSON format and interface wiring.
