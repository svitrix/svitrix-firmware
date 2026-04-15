# PowerManager — AI Reference

Deep sleep controller. Puts the ESP32 into `esp_deep_sleep` with timer and GPIO wake sources.

## TL;DR

- **Provides:** `IPower` → ServerManager, MQTTManager
- **Consumes:** none — uses ESP-IDF sleep APIs directly
- **Entry point:** `PowerManager_::sleep(seconds)`
- **Wake sources:** timer + GPIO 27 (middle button, LOW)

> 📌 Auto-loads when reading files in `src/PowerManager/`

## Files

| File | LOC | Purpose |
|------|-----|---------|
| `PowerManager.h` | 23 | Singleton, `IPower` implementation |
| `PowerManager.cpp` | 36 | Sleep logic, JSON parser, wake source config |

## Wake Sources

| Source | Config |
|--------|--------|
| **Timer** | `esp_sleep_enable_timer_wakeup(seconds * 1e6)` |
| **GPIO 27** | `esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, LOW)` — middle button |

## Interfaces

**Provides:** `IPower` → ServerManager, MQTTManager

**Consumes:** none.

## Don't

- Don't call `sleep()` before persisting critical state — deep sleep resets RAM
- Don't chain sleeps in `loop()` — exiting deep sleep restarts the firmware from setup
- Don't use other wake GPIOs without reconfiguring pull-ups; ESP32 ext0 is single-pin
