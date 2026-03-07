# Apps Block — AI Reference

> Native and custom app implementations for the 32x8 LED matrix.

## Files

| File | Purpose |
|------|---------|
| `Apps.h` | CustomApp struct (inherits AppContentBase), callback declarations, externs |
| `Apps_internal.h` | Shared state for app modules |
| `Apps_NativeApps.cpp` | TimeApp, DateApp, TempApp, HumApp, BatApp |
| `Apps_CustomApp.cpp` | ShowCustomApp — uses shared rendering functions from AppContentRenderer |
| `Apps_Helpers.cpp` | Guard, color, weekday bar, placeholders |
| `Apps_Registry.cpp` | App name/index lookups, global vectors |

## App Callback Signature

```cpp
void AppCallback(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state,
                 int16_t x, int16_t y, GifPlayer *gifPlayer);
```

## Global Data Structures

```cpp
std::vector<std::pair<String, AppCallback>> Apps;  // Active app roster (ordered)
std::map<String, CustomApp> customApps;             // All custom apps by name
String currentCustomApp;                            // Name of currently rendering custom app
```

## CustomApp Struct

Inherits from `AppContentBase` (defined in `src/AppContent.h`). Rendering fields (text, color, icon, scroll, charts, effects, etc.) live in the base struct. CustomApp adds:

| Field | Type | Purpose |
|-------|------|---------|
| `iconName` | String | Filename for icon lookup in /ICONS/ |
| `iconNotFound` | bool | Negative cache for missing icons |
| `currentFrame` | uint8_t | GIF animation frame index |
| `duration` | long | Display time (ms), 0 = default |
| `repeat` | int16_t | Scroll repeat count before auto-advance |
| `currentRepeat` | int16_t | Current repeat counter |
| `name` | String | App identifier |
| `lifetime` | uint64_t | Lifetime in ms (0 = infinite) |
| `lifetimeMode` | byte | Lifetime behavior mode |
| `lifeTimeEnd` | bool | True when lifetime expired (red border) |
| `bounce` / `bounceDir` | bool/int | Bounce animation state |
| `hasCustomColor` | bool | Per-app color override flag |

See `AppContentBase` in `src/AppContent.h` for the full list of shared rendering fields (text, color, icon, scroll, fragments, charts, effects, overlay, etc.).

## Native Apps

### TimeApp (7 display modes)

| Mode | Display |
|------|---------|
| 0 | Text time only |
| 1 | Time + full calendar box |
| 2 | Time + calendar, weekday at top |
| 3 | Time + minimal calendar, weekday at top |
| 4 | Time + minimal calendar, weekday at bottom |
| 5 | Big-digit GIF clock (loads /bigtime.gif, 7-segment bitmasks) |
| 6 | Binary clock (3 rows × 6 bits: hour=red, minute=green, second=blue) |

### DateApp
Formatted date string + weekday bar at bottom.

### TempApp
8x8 thermometer icon (icon_234) + temperature value. Auto Fahrenheit conversion.

### HumApp
8x8 droplet icon (icon_2075) + humidity percentage.

### BatApp (ULANZI only)
8x8 battery icon (icon_1486) + battery percentage.

## ShowCustomApp Rendering Pipeline

Uses shared functions from `AppContentRenderer.cpp` (see `src/MODULES.md`):

```
Phase 1:  Guard (notifyFlag check) + duration setup
Phase 2:  Background fill + effect rendering
Phase 3:  Icon loading (lazy, /ICONS/{name}.jpg|gif, negative cache)
Phase 4:  Text width calculation (with placeholder resolution)
Phase 5:  renderAppIcon() — icon + charts + progress (if topText)
Phase 6:  Auto-transition control (disable if repeat > 0)
Phase 7:  Scroll reset (custom-app-specific repeat logic with currentRepeat)
Phase 8:  updateScrollAnimation() — scroll speed + delay
Phase 9:  renderAppText() — fragments + solid text (hsvOnZeroColor=false)
Phase 10: renderAppIcon() — icon + charts + progress (if !topText)
Phase 11: Lifetime red border (custom-app only)
Phase 12: renderAppOverlay() — weather overlay + text color reset
```

### Scroll Logic

- Reset when `scrollposition + textOffset <= -textWidth`
- Increment `currentRepeat`, if >= repeat → `nextApp()` + reset
- Scroll delay: wait for `scrollDelay > displayConfig.matrixFps` frames
- Speed: `scrollSpeed` per-app or `appConfig.scrollSpeed` global

### Icon Push Modes

- `pushIcon=0` — static icon at position 0
- `pushIcon=1` — push icon left as text scrolls, hold at edge
- `pushIcon=2` — push icon left, reset per repeat cycle

## Custom App Callbacks

Custom apps use `std::function<void(...)>` (AppCallback) with lambda captures.
When a custom app is registered, `pushCustomApp()` creates a lambda that captures
the app name and delegates to `ShowCustomApp()`:

```cpp
AppCallback callback = [name](FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state,
                              int16_t x, int16_t y, GifPlayer *gifPlayer) {
  ShowCustomApp(name, matrix, state, x, y, gifPlayer);
};
```

No hardcoded limit on the number of custom apps.

## Helper Functions

| Function | Purpose |
|----------|---------|
| `nativeAppGuard(appName)` | Skip if notification active, set currentApp |
| `applyNativeAppColor(color)` | Apply custom color or reset to default |
| `drawWeekdayBar(x, y, lineWidth, spacing, start)` | 7 horizontal segments, current day highlighted |
| `replacePlaceholders(input)` | `{{topic}}` → MQTT value substitution |

## Registry Functions

| Function | Purpose |
|----------|---------|
| `getCustomAppByName(name)` | O(1) map lookup |
| `getAppNameAtIndex(index)` | Bounds-checked vector access |
| `findAppIndexByName(name)` | Linear search, returns -1 if not found |
