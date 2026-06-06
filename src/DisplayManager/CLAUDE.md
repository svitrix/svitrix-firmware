# DisplayManager — AI Reference

Coordinator for all visual output on the 32x8 WS2812B LED matrix. Split into 3 classes (Phase 11 refactoring).

## Architecture

```
DisplayManager_ (singleton, coordinator)
  ├── implements: IButtonHandler, IMatrixHost, IDisplayControl, IDisplayNavigation
  ├── owns: DisplayRenderer_ (drawing engine)
  └── owns: NotificationManager_ (notification queue + indicators)
```

## File Map

| File | LOC | Purpose |
|------|-----|---------|
| `DisplayManager.h` | 306 | Public API: 4 interfaces + delegation wrappers |
| `DisplayManager.cpp` | 450 | Lifecycle, tick loop, power, brightness, buttons, matrix layout |
| `DisplayRenderer.h/cpp` | 747 | Text (UnicodeFont + UTF-8), primitives, charts, JPEG/GIF, draw commands |
| `NotificationManager.h/cpp` | 280 | Notification parsing/display, 3 RGB indicators, dismissal |
| `DisplayManager_internal.h` | — | Shared globals (LED buffer, matrix, UI framework) |
| `DisplayManager_CustomApps.cpp` | 745 | Custom app lifecycle: parse, persist, load, lifetime |
| `DisplayManager_ParseHelpers.cpp` | 295 | Shared JSON parsers for custom apps and notifications |
| `DisplayManager_Settings.cpp` | 270 | Settings get/set, device stats, LED export |
| `DisplayManager_Artnet.cpp` | 138 | Art-Net DMX receiver, moodlight mode |
| `AppContentRenderer.cpp` | 213 | Shared rendering: icon, text, scroll, overlay (uses LayoutEngine) |

## Shared Globals (DisplayManager_internal.h)

```cpp
extern CRGB leds[256];              // Primary LED buffer
extern CRGB ledsCopy[256];          // Pre-gamma copy for color readback
extern FastLED_NeoMatrix *matrix;   // NeoMatrix driver
extern MatrixDisplayUi *ui;         // App framework
extern int16_t cursor_x, cursor_y;  // Text cursor
extern uint32_t textColor;          // Current draw color
extern float actualBri;             // Current brightness
extern bool artnetMode, moodlightMode;
```

## DisplayManager_ — Key API

### Lifecycle
- `setup()` — Init FastLED, NeoMatrix, UI framework, load settings
- `tick()` — Main loop: dispatch to artnet/moodlight/UI
- `gammaCorrection()` — Apply gamma + optional mirror to LED buffer

### tick() Dispatch Order
1. AP mode → Show "AP MODE" text
2. artnetMode → handled by DMX callback
3. moodlightMode → handled by moodlight()
4. Normal → resolve active `IDisplayPolicy` (edge-trigger) → `ui->update()`
5. Always: poll artnet, checkNewYear()

### Power & Brightness
- `setBrightness(int)` — respects matrixOff, wakeup notifications, and any active `IDisplayPolicy` brightness override
- `setPower(bool)` — toggle display with sleep animation
- `showSleepAnimation()` — bouncing "Z" (700ms)

### Display Policies
Scheduled/event-driven override rules registered via `registerPolicy(IDisplayPolicy*)`
in `main.cpp`. First-registered active policy wins. DisplayManager caches the
active policy on each tick and applies overrides only on edge transitions
(or when `markPolicyConfigDirty()` forces a re-apply).

- `registerPolicy(IDisplayPolicy *)` — add a policy; pointer must outlive DisplayManager
- `resolveTextColor(uint32_t preferred) const` — `[[nodiscard]]` hot-path helper for
  app renderers (e.g. `Apps_Helpers::applyNativeAppColor`); returns the active
  policy's color override or the fallback
- `markPolicyConfigDirty()` — one-shot flag; call after mutating a config field
  a policy reads (e.g. `appConfig.nightColor`) so the next tick re-syncs the
  global brightness and text-color defaults. Wired from `setNewSettings()`.

Current implementors: `NightModePolicy` (src/policies/).

### Custom Apps
- `parseCustomPage(name, json, preventSave)` — create/update/delete custom app
- `loadCustomApps()` — load from `/CUSTOMAPPS/*.json` at boot (runs **before** `loadNativeApps()` in `main.cpp` so the merge can position custom apps)
- `loadNativeApps()` — rebuild the **unified app loop** (native + weather + custom) following the persisted order. Builds the enabled/available set, then `orderApps(parseAppOrder(appConfig.appOrder), desired)` (from `lib/services/AppOrderUtils`) positions them and appends new apps at the end. Replaces the old fixed-position insertion.
- `persistAppOrder()` — the single writer of `appConfig.appOrder`: serializes the live `Apps` order to a JSON array and `saveSettings()`. Called from `reorderApps`, `updateAppVector`, and custom-app add/remove (skipped during boot via `preventSave`).
- `updateAppVector(json)` — add/remove/reposition apps; persists order
- `reorderApps(json)` — reorder to match a JSON name array; unlisted live apps are appended (not dropped), then persisted
- `switchToApp(json)` — switch to app by name

> **App order = single source of truth.** `appConfig.appOrder` (NVS key `APPORDER`) holds the canonical order as a JSON array of names. Disabling an app removes it from the live loop but its name lingers in `appOrder` until the next persist, so re-enabling often restores its slot.

### Unified Rotation (DisplayManager_Artnet.cpp)
Single drag-and-drop list controlling all apps and standalone effects. Replaces the old separate app order + playlist system.

**Runtime struct:**
```cpp
struct RotationItemRuntime {
    String name;       // app or effect name
    int type;          // 0=native, 1=custom, 2=effect
    int duration;      // per-item override (0 = use global)
    uint32_t color;    // per-item text color override (0 = use default)
    String icon;       // reserved for future
    int offset;        // reserved for future
};
```

**Key components:**
- `rotationItems` — vector of `RotationItemRuntime` parsed from `rotationConfig.items`
- `rotationIndex` — current position (-1 initially)
- `currentRotationItem` — pointer to active item (for per-item override access during render)
- `resolveNextApp(currentApp, direction)` — called by MatrixDisplayUi on each transition:
  - Returns app index for apps
  - Returns `-2` for standalone effects (no app UI)
  - Returns `-1` for default sequential behavior (empty rotation)
- `parseRotationConfig()` — parses JSON items, validates apps/effects exist, resets state; preserves `currentRotationItem` pointer across reloads

**Per-item overrides:**
- **Duration:** `tick()` uses `currentRotationItem->duration` if > 0
- **Color:** `applyNativeAppColor(color, appName)` checks `currentRotationItem->color` and verifies name match before applying

**Persistence:** `rotationConfig` stored in NVS key `ROT_ITEMS`; survives reboots.

**HA Integration:** When a standalone effect is displayed, HA receives `Effect: EffectName` via `setCurrentApp()`.

### Settings (DisplayManager_Settings.cpp)
- `getSettings()` → JSON with 50+ config fields
- `setNewSettings(json)` — partial update, applies + persists
- `getStats()` → device status JSON (battery, sensors, uptime, WiFi, RAM)
- `ledsAsJson()` → LED buffer export

### Art-Net (DisplayManager_Artnet.cpp)
- 768 channels (256 LEDs × 3), universes 0-1 + universe 10 (global brightness)
- `moodlight(json)` — `{"color":"#RRGGBB"}` or `{"kelvin":N}`, empty string disables

## DisplayRenderer_ — Drawing API

Implements `IDisplayRenderer`. All methods are stateless, draw to shared globals.

### Text Rendering (UnicodeFont + UTF-8)
- `printText(x, y, text, centered, textCase)` — UTF-8 text, decodes with `utf8NextCodepoint()`, renders each glyph via UnicodeFont
- `HSVtext(x, y, text, clear, textCase)` — per-character rainbow (hue cycles)
- `GradientText(x, y, text, c1, c2, clear, textCase)` — linear color interpolation
- `renderColoredText(...)` — dispatcher: rainbow → HSV, gradient → Gradient, else solid+effects
- `matrixPrintGlyph(uint16_t codepoint)` — core glyph renderer, calls `renderGlyph()` from UnicodeFont service (binary search lookup in SvitrixFont glyph table)

### Primitives
- `drawPixel`, `drawLine` (Bresenham), `drawRect`, `drawFilledRect`
- `drawCircle`, `fillCircle`, `drawFastVLine`

### Images
- `drawBMP(x, y, bitmap, w, h)` — 565 RGB
- `drawRGBBitmap(x, y, bitmap, w, h)` — 888 RGB
- `drawJPG(x, y, file)` / `drawJPG(x, y, data, size)` — JPEG decode

### Charts
- `drawBarChart(x, y, data, size, withIcon, color, barBG)` — vertical bars
- `drawLineChart(x, y, data, size, withIcon, color)` — connected line segments

### JSON Draw Instructions
`processDrawInstructions(xOffset, yOffset, json)` — parses array of commands:
```json
[["dp",[x,y,color]], ["dl",[x0,y0,x1,y1,color]], ["dr",[x,y,w,h,color]],
 ["df",[x,y,w,h,color]], ["dc",[x,y,r,color]], ["dfc",[x,y,r,color]],
 ["dt",[x,y,"text",color]], ["db",[x,y,w,h,[rgb...]]]]
```

## NotificationManager_ — Notification Queue

Implements `IDisplayNotifier`. Stores notifications in `deque<Notification>`.

### generateNotification(source, json)
- `source` — 0=MQTT, 1=HTTP
- Parses: text, icon (file or base64), duration, sound/rtttl, hold, wakeup, stack, clients
- Shared fields with custom apps: progress bar, charts, gradients, effects, overlays, draw instructions
- Client forwarding: `"clients": ["mqtt://device1", "http://192.168.1.1"]`

### dismissNotify()
- Shifts to next notification or clears queue
- Stops sound, restores brightness if wakeup was active

### Indicators (1-3)
- `indicatorParser(indicator, json)` — `{"color":"#RRGGBB","blink":ms,"fade":ms}`
- `setIndicator{1,2,3}Color(uint32_t)` / `setIndicator{1,2,3}State(bool)`
- Publishes state changes via `INotifier`

## Shared JSON Parsers (ParseHelpers)

Used by both custom apps and notifications:
- `parseProgressBar()` — progress (0-100), colors
- `parseChartData()` — bar/line arrays (max 16 points, auto-scaled to 0-8)
- `parseGradient()` — [color1, color2]
- `parseTextOrFragments()` — plain text or `[{t:"...",c:"..."}]` colored fragments
- `parseCommonAppFields()` — 16 display/effect fields (incl. `layout`)
- `readColorField()` — hex `"#RRGGBB"` or `[r,g,b]`

## Dependency Injection

```cpp
void setNotifier(INotifier *n);              // MQTT publisher
void setPeriphery(IPeripheryProvider *p);    // Audio, uptime
void setMenuActiveQuery(bool (*cb)());       // Menu state callback
void registerPolicy(IDisplayPolicy *p);      // Scheduled display-override rules
```

## Wiring in main.cpp

```cpp
PeripheryManager.addButtonHandler(&DisplayManager);      // IButtonHandler
DisplayManager.setNotifier(&MQTTManager);                 // INotifier
DisplayManager.setPeriphery(&PeripheryManager);           // IPeripheryProvider
// Display policies (order = priority):
static RealTimeProvider realTimeProvider;
static NightModePolicy nightModePolicy(appConfig, realTimeProvider);
DisplayManager.registerPolicy(&nightModePolicy);
// Other modules receive interfaces:
MenuManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager);
ServerManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
MQTTManager.setDisplay(&DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
```

## Backward Compatibility

DisplayManager_ delegates ~79 methods to renderer_ and notifMgr_. Apps calling `DisplayManager.drawPixel()` still work — forwarded internally.

## Debugging Hints

Things grep won't tell you — reach for these before hypothesizing:

### Brightness flicker at low values

- LED output passes through `calculateGamma(brightness)` from [lib/services/src/GammaUtils.cpp](../../lib/services/src/GammaUtils.cpp) — a two-segment log curve: `logMap(bri, 2, 180, 0.535, 2.3, 1.9)`.
- At `brightness < 2` the curve floors at `0.535` — near zero output → perceived flicker / dropout.
- Auto-brightness path ([calculateBrightness in SensorCalc.cpp](../../lib/services/src/SensorCalc.cpp)) then applies `pow(pct, ldrGamma) / pow(100, ldrGamma-1)` and clamps to `[minBri, maxBri]`.
- **Common fixes:**
  - Raise `brightnessConfig.minBri` (default low; set to 10–15 to kill dropouts).
  - Lower `brightnessConfig.ldrGamma` if auto-brightness overshoots dark too fast.
  - Disable auto-brightness entirely (`autoBrightness = false`) to isolate whether the issue is sensor-driven or inherent.
- Curve samples live in [test/test_native/test_gamma/](../../test/test_native/test_gamma/).

### Text appears corrupted / truncated

- Width measurement is in `TextUtils::getTextWidth()` — uses `activeFont` set once at startup (`setTextFont(SvitrixFont)` in `main.cpp`). If the font pointer is null, all widths return 0 → cursor never advances.
- Cyrillic chars use a two-byte UTF-8 decode in [TextUtils](../../lib/services/src/TextUtils.cpp); missing codepoints fall back to width 4.
- For unicode glyph rendering see [UnicodeFont](../../lib/services/src/UnicodeFont.cpp) — binary search over sorted codepoints.

### Notifications queue stuck / not advancing

- NotificationManager owns a vector of `Notification` (FIFO). `duration=0` means indefinite — stuck notification usually = someone pushed `duration=0` and didn't dismiss.
- Check `/api/notify/dismiss` or MQTT `notify/dismiss` to clear.
- Indicator state (3 RGB dots) is independent — indicators persist across notifications.

### Custom app doesn't render or loops forever

- `lifetime=0` = never expires (indefinite). `repeat=0` = loop forever. Both together → app never releases the loop slot.
- Parsed by `parseCustomPage()` in `DisplayManager_CustomApps.cpp`; renderer is `AppContentRenderer.cpp` (shared with notifications — bugs here affect both).

### Matrix freezes but no crash

- Likely `tick()` blocked inside a long-running operation. Check:
  - `MQTTManager.tick()` publishing while WiFi is dropping — wrap in `isConnected` guard.
  - `UpdateManager.updateFirmware()` runs synchronously and hijacks the loop until reboot.
  - `DataFetcher.tick()` HTTP fetch — if `timeout_ms` too high, stalls loop.
