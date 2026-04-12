# Icon Layout System — Design Spec

**Date:** 2026-04-12
**Branch:** (to be created) `feature/icon-layout`
**Status:** Approved

## Goal

Add configurable icon positioning (left / right / none) across all rendering contexts: native apps, custom apps, and notifications. Replace hardcoded icon/text coordinates with a unified LayoutEngine service.

## Current State

- Native apps (Temp, Hum, Bat) hardcode icon at x=0 via `matrix->drawRGBBitmap()` and text at x=8/12/14
- Custom apps and notifications use `renderAppIcon()` + `calculateTextX()` — icon always left, text at x=9
- No way to position icon on the right or hide it
- Layout logic scattered across `AppContentRenderer.cpp`, `Apps_NativeApps.cpp`, `Apps_CustomApp.cpp`, `Overlays.cpp`

## Design

### Approach

Approach B: `IconLayout` enum in `AppContentBase` + stateless `LayoutEngine` service in `lib/services/`.

### New Types

```cpp
// lib/services/src/LayoutEngine.h

enum class IconLayout : uint8_t {
    Left  = 0,   // default, backward compatible
    Right = 1,
    None  = 2
};

struct LayoutMetrics {
    int16_t  iconX;              // -1 if no icon
    int16_t  iconY;
    int16_t  textStartX;         // initial text x (for scroll start position)
    int16_t  textCenterX;        // centered text x (for non-scrolling short text)
    uint16_t textAvailableWidth; // 24 (with icon) or 32 (without)
    bool     hasIcon;
    bool     iconOnRight;
};
```

### LayoutEngine Service

Stateless utility in `lib/services/`. 14th service, follows existing patterns.

```cpp
class LayoutEngine {
public:
    static LayoutMetrics computeLayout(
        IconLayout layout,
        uint16_t textWidth,
        int16_t iconWidth = 8,
        int16_t matrixWidth = 32
    );

    static int16_t computePushIconPosition(
        IconLayout layout,
        float scrollPosition,
        int16_t textOffset,
        bool iconWasPushed,
        float movementFactor
    );
};
```

#### computeLayout() Output

| Layout | iconX | textStartX | textAvailableWidth | iconOnRight |
|--------|-------|------------|--------------------|-------------|
| Left   | 0     | 9          | 24                 | false       |
| Right  | 24    | 0          | 24                 | true        |
| None   | -1    | 0          | 32                 | false       |

`textCenterX` computed dynamically:
- Left: `((24 - textWidth) / 2) + 9`
- Right: `(24 - textWidth) / 2`
- None: `(32 - textWidth) / 2`

#### computePushIconPosition()

Push animation for Right layout mirrors Left: icon moves right (positive direction) instead of left (negative). Left layout preserves current behavior exactly.

### AppContentBase Changes

```cpp
// src/AppContent.h
#include "LayoutEngine.h"

struct AppContentBase {
    // existing fields unchanged...
    IconLayout layout = IconLayout::Left;  // +1 byte
};
```

Fields `icon`, `iconOffset`, `iconPosition`, `pushIcon`, `iconWasPushed` remain unchanged. Layout only determines geometry (where icon, where text); animation stays in AppContentRenderer.

### JSON API

New `"layout"` field in custom apps and notifications:

```json
POST /api/custom?name=crypto
{ "text": "$94,521", "icon": "btc", "layout": "right" }

POST /api/notify
{ "text": "Door open!", "icon": "door", "layout": "left" }
```

Values: `"left"` (default), `"right"`, `"none"`. Missing field = `"left"`.

MQTT custom apps and notifications use the same JSON format.

Parsed in `parseCommonAppFields()` in `DisplayManager_ParseHelpers.cpp`.

### Rendering Changes

**`AppContentRenderer.cpp`:**

- `calculateTextX()` — removed, replaced by `LayoutEngine::computeLayout()`
- `renderAppIcon()` — uses `LayoutMetrics.iconX` instead of hardcoded x=0; separator line draws on correct side based on `iconOnRight`
- `renderAppText()` — uses `LayoutMetrics.textCenterX` / `textStartX` instead of hardcoded 9/0
- `updateScrollAnimation()` — initial scroll position from `LayoutMetrics.textStartX` instead of hardcoded `hasIcon ? 9 : 0`
- Push animation mirrors for `iconOnRight=true`

**`Apps_NativeApps.cpp`:**

- TempApp, HumApp, BatApp — remove hardcoded `matrix->drawRGBBitmap()` + `setCursor()`. Instead, populate `AppContentBase` fields (`text`, `icon`, `layout`) and call the shared rendering pipeline.
- TimeApp, DateApp — unchanged (no icons, specialized rendering: big digits, binary clock).

**`Apps_CustomApp.cpp` and `Overlays.cpp`:**

- Pass `app.layout` through to rendering functions
- No structural changes — the rendering functions handle layout internally

### Settings & Persistence

**Native apps:**
- New field in `AppConfig`: `IconLayout nativeIconLayout = IconLayout::Left`
- Global setting for all native apps with icons (Temp, Hum, Bat)
- Settings JSON API: `{ "nativeIconLayout": "right" }`
- Persisted in NVS via `saveSettings()` / `loadSettings()`
- No MenuManager item (configured via SPA/API)

**Custom apps:**
- Layout saved in `/CUSTOMAPPS/*.json` as part of the app JSON

**Notifications:**
- Per-notification via JSON field, not persisted

### Backward Compatibility

- `IconLayout::Left` is default (value 0) — all existing apps/notifications render identically
- Missing `"layout"` field in JSON = Left
- Existing custom apps without layout field continue to work
- NVS: missing `nativeIconLayout` key = Left

### Test Plan

New test suite `test_layout_engine` in `test/test_native/`:

| Test | Verifies |
|------|----------|
| `test_left_layout_metrics` | Left: iconX=0, textStartX=9, avail=24 |
| `test_right_layout_metrics` | Right: iconX=24, textStartX=0, avail=24 |
| `test_none_layout_metrics` | None: hasIcon=false, textStartX=0, avail=32 |
| `test_text_center_left` | Text centering with icon left |
| `test_text_center_right` | Text centering with icon right |
| `test_text_center_none` | Text centering full width |
| `test_custom_icon_width` | Non-standard icon width (GIF) |
| `test_push_icon_right` | Push animation mirrors for Right |
| `test_default_is_left` | Enum default = Left (backward compat) |

100% coverage, consistent with other `lib/services/` test suites.

### Files Changed

| File | Change |
|------|--------|
| `lib/services/src/LayoutEngine.h` | **NEW** — enum, struct, class |
| `lib/services/src/LayoutEngine.cpp` | **NEW** — implementation |
| `test/test_native/test_layout_engine/` | **NEW** — test suite |
| `src/AppContent.h` | Add `IconLayout layout` field |
| `src/DisplayManager/AppContentRenderer.cpp` | Refactor: use LayoutEngine, remove `calculateTextX()` |
| `src/Apps/Apps_NativeApps.cpp` | Refactor Temp/Hum/Bat to use shared pipeline |
| `src/Apps/Apps_CustomApp.cpp` | Pass layout to rendering |
| `src/Overlays/Overlays.cpp` | Pass layout to rendering |
| `src/DisplayManager/DisplayManager_ParseHelpers.cpp` | Parse `"layout"` field |
| `lib/config/src/ConfigTypes.h` | Add `nativeIconLayout` to `AppConfig` |
| `src/Globals.cpp` | NVS load/save for `nativeIconLayout` |
| `src/DisplayManager/DisplayManager_Settings.cpp` | Expose in settings JSON |

### Size Impact

- Flash: ~400 bytes (LayoutEngine code + enum/struct)
- RAM: +1 byte per AppContentBase object
- Negligible on 650 KB free flash / 230 KB free RAM

### Visual Examples

```
Layout=Left (default):
  [ICON][  22.5*C   ]
   0--7  8---------31

Layout=Right:
  [   22.5*C  ][ICON]
   0---------23 24-31

Layout=None:
  [       22.5*C       ]
   0--------------------31
```
