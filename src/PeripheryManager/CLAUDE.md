# PeripheryManager — AI Reference

Hardware abstraction singleton. Manages physical buttons, I2C temperature/humidity sensors, LDR (light), battery ADC, and piezo buzzer.

## TL;DR

- **Provides:** `IPeripheryProvider`, `ISound`
- **Consumes:** none (owns hardware directly)
- **Dispatches to:** `IButtonHandler` vector, `IButtonReporter` vector
- **Entry point:** `PeripheryManager_::setup()`, `::tick()`
- **Uses service:** `SensorCalc`

> 📌 Auto-loads when reading files in `src/PeripheryManager/`

## Files

| File | Purpose |
|------|---------|
| `PeripheryManager.h` | Public API, singleton, IPeripheryProvider + ISound |
| `PeripheryManager.cpp` | Hardware init, sensor loops, button dispatch, sound |

## Interfaces

**Implements:** `IPeripheryProvider`, `ISound`

**Dispatches to (multi-consumer vectors):**
- `IButtonHandler` — `leftButton()`, `rightButton()`, `selectButton()`, `selectButtonLong()`
- `IButtonReporter` — `sendButton(btn, state)`

## Hardware Components

### Buttons (4x EasyButton)

| Button | GPIO | Events |
|--------|------|--------|
| Left | 26 | `onPressed` → `dispatchLeftButton()` |
| Right | 14 | `onPressed` → `dispatchRightButton()` |
| Select | 27 | `onPressed`, `onPressedFor(1000ms)`, `onSequence(2, 300ms)` |
| Reset | 13 | `onPressedFor(5000ms)` → factory reset (ULANZI only) |

Button swap: left/right swapped when `rotateScreen XOR swapButtons`.

### I2C Sensors (auto-detected, first match wins)

1. **BME280** (0x76/0x77) — temp + humidity
2. **BMP280** (0x76/0x77) — temp only
3. **HTU21DF** — temp + humidity
4. **SHT31** (0x44) — temp + humidity

Readings every **10 seconds**, filtered with calibration offsets.

### Other Hardware

- **LDR** — GPIO 35, read every **100ms**, median+mean filtered
- **Battery** (ULANZI only) — GPIO 34 ADC, read every **10 seconds**
- **Buzzer** — GPIO 15, PWM via `MelodyPlayer`, async RTTTL playback

## Button Dispatch Architecture

```
EasyButton callbacks
  └→ check blockNavigation
      └→ dispatch*()
          ├→ IButtonHandler vector: DisplayManager_, MenuManager_
          └→ IButtonReporter vector: MQTTManager_, ServerManager_
```

## Callback Registration

| Method | Purpose |
|--------|---------|
| `addButtonHandler(IButtonHandler*)` | Left/right/select dispatch |
| `addButtonReporter(IButtonReporter*)` | Raw button state reporting |
| `setOnPowerToggle(fn)` | Double-press power toggle |
| `setOnBrightnessChange(fn)` | Auto-brightness updates |
| `setOnFactoryReset(fn)` | Reset button long press |
| `setIsMenuActive(fn)` | Suppress reports during menu |

## Tick Loop

```
tick()
  ├─ Read buttons (dispatch reports if menu not active)
  ├─ Every 10s: read battery + temperature/humidity
  └─ Every 100ms: read LDR, compute auto-brightness
```

## Don't

- Don't call `tick()` outside main loop — button timing depends on it
- Don't do heavy work in EasyButton callbacks — dispatch to main loop
- Don't access I2C from ISR
- Don't dispatch button events while `isMenuActive()` returns true (menu consumes them)
