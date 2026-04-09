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
| `DisplayManager.h` | 307 | Public API: 4 interfaces + 79 delegation wrappers |
| `DisplayManager.cpp` | 450 | Lifecycle, tick loop, power, brightness, buttons, matrix layout |
| `DisplayRenderer.h/cpp` | 768 | Text (UnicodeFont + UTF-8), primitives, charts, JPEG/GIF, draw commands |
| `NotificationManager.h/cpp` | 280 | Notification parsing/display, 3 RGB indicators, dismissal |
| `DisplayManager_internal.h` | — | Shared globals (LED buffer, matrix, UI framework) |
| `DisplayManager_CustomApps.cpp` | 745 | Custom app lifecycle: parse, persist, load, lifetime |
| `DisplayManager_ParseHelpers.cpp` | 295 | Shared JSON parsers for custom apps and notifications |
| `DisplayManager_Settings.cpp` | 270 | Settings get/set, device stats, LED export |
| `DisplayManager_Artnet.cpp` | 138 | Art-Net DMX receiver, moodlight mode |

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
- `tick()` — Main loop: dispatch to game/artnet/moodlight/UI
- `gammaCorrection()` — Apply gamma + optional mirror to LED buffer

### tick() Dispatch Order
1. GameManager active → `GameManager.tick()`
2. AP mode → Show "AP MODE" text
3. artnetMode → handled by DMX callback
4. moodlightMode → handled by moodlight()
5. Normal → `ui->update()` (app framework)
6. Always: poll artnet, checkNewYear()

### Power & Brightness
- `setBrightness(int)` — respects matrixOff + wakeup notifications
- `setPower(bool)` — toggle display with sleep animation
- `showSleepAnimation()` — bouncing "Z" (700ms)

### Custom Apps
- `parseCustomPage(name, json, preventSave)` — create/update/delete custom app
- `loadCustomApps()` — load from `/CUSTOMAPPS/*.json` at boot
- `loadNativeApps()` — register Time/Date/Temp/Hum/Bat based on config
- `updateAppVector(json)` — add/remove/reorder apps
- `reorderApps(json)` — reorder app vector
- `switchToApp(json)` — switch to app by name

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
- `matrixPrint(codepoint)` — core glyph renderer, calls `renderGlyph()` from UnicodeFont service (binary search lookup in SvitrixFont glyph table)

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
- `parseCommonAppFields()` — 15 display/effect fields
- `readColorField()` — hex `"#RRGGBB"` or `[r,g,b]`

## Dependency Injection

```cpp
void setNotifier(INotifier *n);           // MQTT publisher
void setPeriphery(IPeripheryProvider *p);  // Audio, uptime
void setMenuActiveQuery(bool (*cb)());    // Menu state callback
```

## Wiring in main.cpp

```cpp
PeripheryManager.addButtonHandler(&DisplayManager);      // IButtonHandler
DisplayManager.setNotifier(&MQTTManager);                 // INotifier
DisplayManager.setPeriphery(&PeripheryManager);           // IPeripheryProvider
// Other modules receive interfaces:
MenuManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager);
ServerManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
MQTTManager.setDisplay(&DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
```

## Backward Compatibility

DisplayManager_ delegates ~79 methods to renderer_ and notifMgr_. Apps calling `DisplayManager.drawPixel()` still work — forwarded internally.
