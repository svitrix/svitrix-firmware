# MenuManager — AI Reference

On-device settings menu rendered on the LED matrix. Adjusts display, time, audio, and app settings via physical buttons.

## TL;DR

- **Provides:** `IButtonHandler` → PeripheryManager
- **Consumes:** `IDisplayRenderer`, `IDisplayControl`, `IDisplayNavigation`, `IPeripheryProvider`, `IUpdater`
- **Entry point:** `MenuManager_::setup()`, `::tick()`, `::show(bool)`
- **UI:** ~13 menu items on 32×8 matrix

> 📌 Auto-loads when reading files in `src/MenuManager/`

## Files

| File | Purpose |
|------|---------|
| `MenuManager.h` | Singleton, `IButtonHandler` implementation |
| `MenuManager.cpp` | Menu state machine, menu items, button handlers |

## Menu Structure (~13 items)

| Menu | Controls |
|------|----------|
| BRIGHT | `brightnessPercent` (1-100%) or AUTO |
| COLOR | `textColor` (15 preset colors) |
| SWITCH | `autoTransition` (ON/OFF) |
| T-SPEED | `timePerTransition` (200-2000ms) |
| APPTIME | `timePerApp` (1-30s) |
| TIME | `timeFormat` (8 strftime patterns) |
| DATE | `dateFormat` (9 strftime patterns) |
| WEEKDAY | `startOnMonday` (MON/SUN) |
| TEMP | `isCelsius` (°C/°F) |
| APPS | Toggle native apps |
| SOUND | `soundActive` (ON/OFF) |
| VOLUME | `soundVolume` (0-30) |
| UPDATE | Triggers OTA update |

## Navigation

| Button | MainMenu | Submenu |
|--------|----------|---------|
| Right | Next item | Increase value |
| Left | Previous item | Decrease value |
| Select (short) | Enter submenu | Toggle |
| Select (long) | Exit menu | Save + back |

## Interfaces

**Provides:** `IButtonHandler` → PeripheryManager

**Consumes:** `IDisplayRenderer`, `IDisplayControl`, `IDisplayNavigation`, `IPeripheryProvider`, `IUpdater`

## Don't

- Don't render menu when notifications are active — defer to NotificationManager
- Don't persist every value change — only on submenu exit (`Select long`)
- Don't call `IUpdater::updateFirmware()` without user confirmation in UPDATE menu
