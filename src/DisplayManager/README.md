# DisplayManager Block — AI Reference

> Central display coordinator for 32x8 WS2812B LED matrix (GPIO 32).
> Split into 3 classes: coordinator + DisplayRenderer_ + NotificationManager_.

## Files

| File | LOC | Purpose |
|------|-----|---------|
| `DisplayManager.h` | 182 | Public API header + 79 delegation wrappers |
| `DisplayManager.cpp` | 417 | Core: setup, tick, power, brightness, navigation |
| `DisplayManager_internal.h` | — | Shared globals (LED buffer, matrix, UI) — private to block |
| `DisplayRenderer.h` / `.cpp` | 768 | Drawing engine: text, shapes, images, charts |
| `NotificationManager.h` / `.cpp` | 318 | Notification queue + 3 RGB indicators |
| `DisplayManager_CustomApps.cpp` | 745 | Custom app lifecycle (parse, persist, load) |
| `DisplayManager_Settings.cpp` | 270 | Settings get/set, stats JSON, LED export |
| `DisplayManager_Artnet.cpp` | 138 | Art-Net DMX receiver + moodlight mode |
| `DisplayManager_ParseHelpers.cpp` | 295 | JSON parsing utilities shared by CustomApps + Notifications |

## Class: DisplayManager_

**Pattern**: Singleton (`getInstance()`, deleted copy/move)
**Implements**: `IDisplayControl`, `IDisplayNavigation`, `IMatrixHost`, `IButtonHandler`
**Owns**: `DisplayRenderer_ renderer_`, `NotificationManager_ notifMgr_`
**Injected**: `INotifier*` (MQTT), `IPeripheryProvider*` (audio/uptime), `bool(*isMenuActive_)()`

### tick() — Main Loop Dispatch

```
tick()
  ├─ IF game active → GameManager.tick()
  ├─ IF AP mode → HSVtext("AP MODE")
  ├─ IF artnet → (handled by DMX callback)
  ├─ IF moodlight → (handled by moodlight function)
  └─ ELSE → ui->update()
       On transition end:
         resetAllEffectState()
         notify currentApp via MQTT
         checkLifetime(nextApp)
         ResetCustomApps()
```

### setup() — Initialization

1. TJpg_Decoder callback + scale 1:1
2. FastLED NEOPIXEL on GPIO 32
3. `setMatrixLayout()` → creates FastLED_NeoMatrix + MatrixDisplayUi
4. Rotation, gamma (1.9), color correction/temperature
5. GIF player init
6. UI animation = SLIDE_DOWN
7. Apply settings, set overlays (3), set background effect
8. `ui->init()`

### Key Methods

| Method | Signature | Notes |
|--------|-----------|-------|
| `setBrightness` | `(int bri)` | Respects matrixOff, wakeup notifications override |
| `setPower` | `(bool state)` | Sleep animation on off, restore brightness on on |
| `setAutoTransition` | `(bool) → bool` | Fails if <2 apps |
| `setMatrixLayout` | `(int layout)` | 0=32x8 zigzag rows, 1=4×(8x8) tiles, 2=32x8 zigzag columns |
| `gammaCorrection` | `()` | calculateGamma(actualBri), copy to ledsCopy, optional mirror |
| `powerStateParse` | `(const char* json)` | Accepts `"true"/"1"` or `{"power":bool}` |
| `checkNewYear` | `()` | Fireworks + Auld Lang Syne at midnight Jan 1 |

### Button Handlers

- `leftButton()` → `ui->previousApp()` (guard: menu not active)
- `rightButton()` → `ui->nextApp()` (guard: menu not active)
- `selectButton()` → `dismissNotify()` (guard: menu not active)
- `selectButtonLong()` → empty

## Class: DisplayRenderer_

**Implements**: `IDisplayRenderer`
**Accesses**: shared globals via `DisplayManager_internal.h`

### Text Rendering

| Method | Algorithm |
|--------|-----------|
| `printText(x, y, text, centered, textCase)` | Single-color, optional centering (32px width) |
| `HSVtext(x, y, text, clear, textCase)` | Per-character hue cycling (static hueOffset) |
| `GradientText(x, y, text, color1, color2, clear, textCase)` | Per-character interpolateColor(t) |
| `renderColoredText(...)` | Dispatcher: rainbow → HSVtext, gradient → GradientText, else solid |
| `matrixPrint(char c)` | Core glyph renderer: reads SvitrixFont bitmap, pixel-by-pixel |

### Drawing Primitives

| Method | Algorithm |
|--------|-----------|
| `drawLine` | Bresenham's line algorithm |
| `drawRect` | 4 edge loops |
| `drawFilledRect` | Vertical scanline fill |
| `drawCircle` | Midpoint circle algorithm (8 octants) |
| `fillCircle` | Midpoint + horizontal scanlines |
| `drawProgressBar` | Single-pixel height, pColor fill + pbColor background |
| `drawMenuIndicator` | Centered dot pagination |
| `drawBarChart` | Vertical bars with autoscale, optional icon offset (9px) |
| `drawLineChart` | Connected data points with interpolated X spacing |

### processDrawInstructions(xOffset, yOffset, jsonString)

JSON array of draw commands:
- `"dp"` [x, y, color] — drawPixel
- `"dl"` [x0, y0, x1, y1, color] — drawLine
- `"dr"` [x, y, w, h, color] — drawRect (outline)
- `"df"` [x, y, w, h, color] — drawFilledRect
- `"dc"` [x, y, r, color] — drawCircle
- `"dfc"` [x, y, r, color] — fillCircle
- `"dt"` [x, y, text, color] — drawText (cursor y+5)
- `"db"` [x, y, w, h, [colors...]] — drawBitmap pixel-by-pixel

## Class: NotificationManager_

**Implements**: `IDisplayNotifier`
**Injected**: `INotifier*`, `IPeripheryProvider*` via `setDependencies()`

### generateNotification(source, json)

1. Parse JSON (6144 bytes) → Notification struct
2. Shared field parsing (same helpers as custom apps)
3. Notification-specific: loopSound, sound, rtttl, duration, hold, wakeup
4. Icon: base64 decode if >64 chars, else LittleFS `/ICONS/{name}.jpg|gif`
5. Client forwarding: "clients" array → MQTT rawPublish or HTTP POST
6. Stack: push_back (default) or replace first (stack=false)
7. Set currentApp = "Notification"

### dismissNotify()

1. Shift timer to next notification
2. Close icon file handle
3. `pop_front()`
4. Stop sound
5. If wakeup was active and matrix is off → setBrightness(0)

### indicatorParser(indicator, json)

- Empty `""` or `"{}"` → clear indicator (state=false, blink=0, fade=0)
- `{"color": "#RRGGBB", "blink": ms, "fade": ms}` → set state/color/effects
- Publish state change via `notifier_->setIndicatorState()`

## Shared Globals (DisplayManager_internal.h)

```cpp
extern CRGB leds[256];           // Primary LED buffer
extern CRGB ledsCopy[256];       // Pre-gamma copy
extern FastLED_NeoMatrix *matrix;
extern MatrixDisplayUi *ui;
extern float actualBri;           // 0-255
extern int16_t cursor_x, cursor_y;
extern uint32_t textColor;
extern fs::File gifFile;
extern GifPlayer gif;
extern bool artnetMode, moodlightMode;
extern float displayGamma;
extern CRGB colorCorrection, colorTemperature;
extern String currentApp;
```

## Custom Apps Lifecycle (DisplayManager_CustomApps.cpp)

### parseCustomPage(name, json, preventSave)

- Empty payload `""` / `"{}"` → remove app entirely
- JsonObject → `generateCustomPage(name, doc, preventSave)`
- JsonArray → loop, call `generateCustomPage(name + index, ...)`

### generateCustomPage(name, doc, preventSave)

1. `pushCustomApp(name, pos)` — allocate trampoline slot (0-19)
2. Persist to `/CUSTOMAPPS/{name}.json` if doc["save"]=true
3. Parse all common fields (shared with notifications)
4. CustomApp-specific: duration, lifetime, lifetimeMode, bounce
5. Icon handling: base64 or LittleFS name (change detection)
6. MQTT placeholder subscription for `{{topic}}` patterns

### Key Free Functions

| Function | Purpose |
|----------|---------|
| `pushCustomApp(name, pos)` | Allocate callback slot (0-19), insert into Apps vector |
| `removeCustomAppFromApps(name, setApps)` | Remove from Apps + customApps + LittleFS |
| `ResetCustomApps()` | Reset scroll state on all inactive custom apps |
| `checkLifetime(pos)` | Remove or mark expired apps (lifetimeMode 0=remove, 1=red border) |
| `subscribeToPlaceholders(text)` | Scan for `{{topic}}` and subscribe via MQTT |

## JSON Parse Helpers (DisplayManager_ParseHelpers.cpp)

| Function | Parses |
|----------|--------|
| `parseCRGBFromJson(doc, key, &target)` | Hex string or [r,g,b] array → CRGB |
| `parseProgressBar(doc, ...)` | progress (-1..100), pColor, pbColor |
| `parseChartData(doc, ...)` | bar[16], line[16] with autoscale to 0-8 |
| `parseGradient(doc, gradient[2])` | 2-color gradient endpoints |
| `parseTextOrFragments(doc, ...)` | Plain text or [{t:"text", c:"#hex"}] fragments |
| `parseCommonAppFields(doc, ...)` | 15 fields: effect, overlay, rainbow, pushIcon, textCase, offsets, scrollSpeed, topText, fade, blink, center, noScrolling, repeat, drawInstructions |
| `parseAppColor(doc)` | "color" field or default textColor |
| `parseBackground(doc)` | "background" field or 0 (transparent) |
| `decodeBase64Icon(data, &buffer)` | Base64 → uint8_t vector |

## Settings & Stats (DisplayManager_Settings.cpp)

### getSettings() — 50+ config fields

Display: MATP, BRI, ABRI, MAT, GAMMA, UPPERCASE
Apps: ATRANS, ATIME, TIM, DAT, TEMP, HUM, BAT
Transitions: TEFF, TSPEED
Time: TMODE, TFORMAT, DFORMAT, SOM, CEL
Colors: TCOL, TIME_COL, DATE_COL, TEMP_COL, HUM_COL, BAT_COL, CHCOL, CTCOL, CBCOL, WDCA, WDCI
Audio: SOUND, VOL
Other: CCORRECTION, CTEMP, WD, BLOCKN, SSPEED, OVERLAY

### setNewSettings(json) — partial update

Parses only present fields, applies to config, calls `applyAllSettings()` + `saveSettings()`.

## Art-Net / Moodlight (DisplayManager_Artnet.cpp)

- **Artnet**: 768 channels (256 LEDs × 3), 2 universes, universe 10 = brightness override
- **Moodlight**: `{"color":"#RRGGBB"}` or `{"kelvin":N, "brightness":N}`, empty string = disable
