# PeripheryManager

Hardware abstraction layer for all physical peripherals on the Ulanzi TC001.
Manages buttons, I2C sensors, LDR, battery, and piezo buzzer.

## Key Files

| File | Purpose |
|------|---------|
| `PeripheryManager.h` | Singleton, implements `IPeripheryProvider` + `ISound` |
| `PeripheryManager.cpp` | Hardware init, sensor polling, button dispatch, sound playback |

## Hardware

- **Buttons**: 4x EasyButton (left, right, select, reset) with press/long-press/double-press
- **Sensors**: Auto-detected I2C (BME280, BMP280, HTU21DF, SHT31) for temp/humidity
- **LDR**: Ambient light sensor for auto-brightness (GPIO 35)
- **Battery**: ADC reading with filtering (ULANZI build only)
- **Buzzer**: Async RTTTL playback via MelodyPlayer (GPIO 15)

## Button Dispatch

Button events are dispatched to two consumer vectors:
- `IButtonHandler` (DisplayManager, MenuManager) -- navigation actions
- `IButtonReporter` (MQTTManager, ServerManager) -- raw state reporting

## Detailed Reference

See the PeripheryManager section in `src/CLAUDE.md` for GPIO mapping,
sensor details, and callback registration API.
