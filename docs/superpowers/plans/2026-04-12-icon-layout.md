# Icon Layout System — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add configurable icon positioning (left/right/none) via a LayoutEngine service, replacing hardcoded icon/text coordinates across all rendering contexts.

**Architecture:** New `IconLayout` enum + `LayoutMetrics` struct + stateless `LayoutEngine` service in `lib/services/`. `AppContentBase` gets a `layout` field. `AppContentRenderer.cpp` uses `LayoutEngine::computeLayout()` instead of hardcoded positions. Native apps (Temp/Hum/Bat) adopt the shared pipeline. JSON API gets a `"layout"` field.

**Tech Stack:** C++17, Unity test framework, PlatformIO native_test

**Spec:** `docs/superpowers/specs/2026-04-12-icon-layout-design.md`

---

### Task 1: Create LayoutEngine service with tests (TDD)

**Files:**
- Create: `lib/services/src/LayoutEngine.h`
- Create: `lib/services/src/LayoutEngine.cpp`
- Create: `test/test_native/test_layout_engine/test_layout_engine.cpp`

- [ ] **Step 1: Create test file with all test cases**

```cpp
// test/test_native/test_layout_engine/test_layout_engine.cpp
#include <unity.h>
#include "LayoutEngine.h"

// --- computeLayout: Left ---

void test_left_icon_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_EQUAL_INT16(0, m.iconX);
}

void test_left_text_start_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_EQUAL_INT16(9, m.textStartX);
}

void test_left_available_width(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_EQUAL_UINT16(24, m.textAvailableWidth);
}

void test_left_has_icon(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_TRUE(m.hasIcon);
    TEST_ASSERT_FALSE(m.iconOnRight);
}

void test_left_text_center(void)
{
    // text 10px wide in 24px space: (24-10)/2 + 9 = 16
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 10);
    TEST_ASSERT_EQUAL_INT16(16, m.textCenterX);
}

// --- computeLayout: Right ---

void test_right_icon_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_EQUAL_INT16(24, m.iconX);
}

void test_right_text_start_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_EQUAL_INT16(0, m.textStartX);
}

void test_right_available_width(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_EQUAL_UINT16(24, m.textAvailableWidth);
}

void test_right_has_icon(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_TRUE(m.hasIcon);
    TEST_ASSERT_TRUE(m.iconOnRight);
}

void test_right_text_center(void)
{
    // text 10px wide in 24px space: (24-10)/2 = 7
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 10);
    TEST_ASSERT_EQUAL_INT16(7, m.textCenterX);
}

// --- computeLayout: None ---

void test_none_has_no_icon(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 20);
    TEST_ASSERT_FALSE(m.hasIcon);
    TEST_ASSERT_EQUAL_INT16(-1, m.iconX);
}

void test_none_text_start_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 20);
    TEST_ASSERT_EQUAL_INT16(0, m.textStartX);
}

void test_none_available_width(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 20);
    TEST_ASSERT_EQUAL_UINT16(32, m.textAvailableWidth);
}

void test_none_text_center(void)
{
    // text 10px wide in 32px space: (32-10)/2 = 11
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 10);
    TEST_ASSERT_EQUAL_INT16(11, m.textCenterX);
}

// --- Custom icon width ---

void test_custom_icon_width_left(void)
{
    // GIF with 10px width: iconX=0, textStartX=11, avail=32-10-1=21
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 15, 10);
    TEST_ASSERT_EQUAL_INT16(0, m.iconX);
    TEST_ASSERT_EQUAL_INT16(11, m.textStartX);
    TEST_ASSERT_EQUAL_UINT16(21, m.textAvailableWidth);
}

void test_custom_icon_width_right(void)
{
    // GIF with 10px width: iconX=32-10=22, textStartX=0, avail=21
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 15, 10);
    TEST_ASSERT_EQUAL_INT16(22, m.iconX);
    TEST_ASSERT_EQUAL_INT16(0, m.textStartX);
    TEST_ASSERT_EQUAL_UINT16(21, m.textAvailableWidth);
}

// --- Default is Left ---

void test_default_enum_value(void)
{
    IconLayout layout = {};
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layout));
}

// --- layoutToString / layoutFromString ---

void test_layout_to_string_left(void)
{
    TEST_ASSERT_EQUAL_STRING("left", layoutToString(IconLayout::Left).c_str());
}

void test_layout_to_string_right(void)
{
    TEST_ASSERT_EQUAL_STRING("right", layoutToString(IconLayout::Right).c_str());
}

void test_layout_to_string_none(void)
{
    TEST_ASSERT_EQUAL_STRING("none", layoutToString(IconLayout::None).c_str());
}

void test_layout_from_string_left(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layoutFromString("left")));
}

void test_layout_from_string_right(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Right),
                            static_cast<uint8_t>(layoutFromString("right")));
}

void test_layout_from_string_none(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::None),
                            static_cast<uint8_t>(layoutFromString("none")));
}

void test_layout_from_string_unknown_defaults_left(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layoutFromString("garbage")));
}

void test_layout_from_string_empty_defaults_left(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layoutFromString("")));
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Left
    RUN_TEST(test_left_icon_x);
    RUN_TEST(test_left_text_start_x);
    RUN_TEST(test_left_available_width);
    RUN_TEST(test_left_has_icon);
    RUN_TEST(test_left_text_center);

    // Right
    RUN_TEST(test_right_icon_x);
    RUN_TEST(test_right_text_start_x);
    RUN_TEST(test_right_available_width);
    RUN_TEST(test_right_has_icon);
    RUN_TEST(test_right_text_center);

    // None
    RUN_TEST(test_none_has_no_icon);
    RUN_TEST(test_none_text_start_x);
    RUN_TEST(test_none_available_width);
    RUN_TEST(test_none_text_center);

    // Custom icon width
    RUN_TEST(test_custom_icon_width_left);
    RUN_TEST(test_custom_icon_width_right);

    // Default
    RUN_TEST(test_default_enum_value);

    // String conversion
    RUN_TEST(test_layout_to_string_left);
    RUN_TEST(test_layout_to_string_right);
    RUN_TEST(test_layout_to_string_none);
    RUN_TEST(test_layout_from_string_left);
    RUN_TEST(test_layout_from_string_right);
    RUN_TEST(test_layout_from_string_none);
    RUN_TEST(test_layout_from_string_unknown_defaults_left);
    RUN_TEST(test_layout_from_string_empty_defaults_left);

    return UNITY_END();
}
```

- [ ] **Step 2: Run tests — verify they fail**

Run: `pio test -e native_test -f test_layout_engine -v`
Expected: FAIL — `LayoutEngine.h` not found

- [ ] **Step 3: Create LayoutEngine header**

```cpp
// lib/services/src/LayoutEngine.h
#pragma once

#include <cstdint>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

enum class IconLayout : uint8_t {
    Left  = 0,
    Right = 1,
    None  = 2
};

struct LayoutMetrics {
    int16_t  iconX;
    int16_t  iconY;
    int16_t  textStartX;
    int16_t  textCenterX;
    uint16_t textAvailableWidth;
    bool     hasIcon;
    bool     iconOnRight;
};

class LayoutEngine {
public:
    static LayoutMetrics computeLayout(
        IconLayout layout,
        uint16_t textWidth,
        int16_t iconWidth = 8,
        int16_t matrixWidth = 32
    );
};

String layoutToString(IconLayout layout);
IconLayout layoutFromString(const String& str);
```

- [ ] **Step 4: Create LayoutEngine implementation**

```cpp
// lib/services/src/LayoutEngine.cpp
#include "LayoutEngine.h"

LayoutMetrics LayoutEngine::computeLayout(
    IconLayout layout,
    uint16_t textWidth,
    int16_t iconWidth,
    int16_t matrixWidth)
{
    LayoutMetrics m = {};
    m.iconY = 0;

    switch (layout)
    {
    case IconLayout::Right:
        m.hasIcon = true;
        m.iconOnRight = true;
        m.iconX = matrixWidth - iconWidth;
        m.textStartX = 0;
        m.textAvailableWidth = matrixWidth - iconWidth - 1;
        m.textCenterX = (m.textAvailableWidth - textWidth) / 2;
        break;

    case IconLayout::None:
        m.hasIcon = false;
        m.iconOnRight = false;
        m.iconX = -1;
        m.textStartX = 0;
        m.textAvailableWidth = matrixWidth;
        m.textCenterX = (matrixWidth - textWidth) / 2;
        break;

    case IconLayout::Left:
    default:
        m.hasIcon = true;
        m.iconOnRight = false;
        m.iconX = 0;
        m.textStartX = iconWidth + 1;
        m.textAvailableWidth = matrixWidth - iconWidth - 1;
        m.textCenterX = ((m.textAvailableWidth - textWidth) / 2) + iconWidth + 1;
        break;
    }

    return m;
}

String layoutToString(IconLayout layout)
{
    switch (layout)
    {
    case IconLayout::Right: return "right";
    case IconLayout::None:  return "none";
    case IconLayout::Left:
    default:                return "left";
    }
}

IconLayout layoutFromString(const String& str)
{
    if (str == "right") return IconLayout::Right;
    if (str == "none")  return IconLayout::None;
    return IconLayout::Left;
}
```

- [ ] **Step 5: Run tests — verify they pass**

Run: `pio test -e native_test -f test_layout_engine -v`
Expected: 25 tests PASS

- [ ] **Step 6: Run full test suite**

Run: `pio test -e native_test`
Expected: 26 suites, 470 tests PASS (25 new + 445 existing)

- [ ] **Step 7: Commit**

```bash
git add lib/services/src/LayoutEngine.h lib/services/src/LayoutEngine.cpp \
        test/test_native/test_layout_engine/test_layout_engine.cpp
git commit -m "feat(services): add LayoutEngine service with icon layout support

New stateless service for computing icon/text positioning on the 32x8
LED matrix. Supports Left, Right, and None icon layouts with string
conversion. 25 tests, 100% coverage."
```

---

### Task 2: Add `layout` field to AppContentBase and parse from JSON

**Files:**
- Modify: `src/AppContent.h` (add `layout` field)
- Modify: `src/DisplayManager/DisplayManager_ParseHelpers.cpp:193` (parse `"layout"`)

- [ ] **Step 1: Add layout field to AppContentBase**

In `src/AppContent.h`, add include and field:

```cpp
// After the existing #include "OverlayMapping.h"
#include "LayoutEngine.h"

// Inside struct AppContentBase, after the `OverlayEffect overlay` field (line 46):
    IconLayout layout = IconLayout::Left;
```

- [ ] **Step 2: Parse layout in parseCommonAppFields**

In `src/DisplayManager/DisplayManager_ParseHelpers.cpp`, add `layout` parameter to `parseCommonAppFields` signature and parse it.

Add to the function signature (line 173-177), adding `IconLayout& layout` parameter:

```cpp
void parseCommonAppFields(JsonObject doc, int& effect, OverlayEffect& overlay,
                          bool& rainbow, byte& pushIcon, byte& textCase,
                          int& iconOffset, int& textOffset, float& scrollSpeed,
                          bool& topText, int& fade, int& blink, bool& center,
                          bool& noScrolling, int16_t& repeat, String& drawInstructions,
                          IconLayout& layout)
```

Add after the `repeat` parsing (after line 203):

```cpp
    layout = doc.containsKey("layout") ? layoutFromString(doc["layout"].as<String>()) : IconLayout::Left;
```

- [ ] **Step 3: Update all callers of parseCommonAppFields**

Search for all call sites and add the `layout` argument. There are two callers:

1. `src/DisplayManager/DisplayManager_CustomApps.cpp` — search for `parseCommonAppFields(` and add `ca->layout` as last argument
2. `src/DisplayManager/NotificationManager.cpp` — search for `parseCommonAppFields(` and add `notif.layout` as last argument

Also update the forward declaration in the header if one exists.

- [ ] **Step 4: Build to verify compilation**

Run: `pio run -e ulanzi`
Expected: SUCCESS (no functional change yet — layout field added but not used in rendering)

- [ ] **Step 5: Run tests**

Run: `pio test -e native_test`
Expected: All pass (no test changes needed — layout defaults to Left)

- [ ] **Step 6: Commit**

```bash
git add src/AppContent.h src/DisplayManager/DisplayManager_ParseHelpers.cpp \
        src/DisplayManager/DisplayManager_CustomApps.cpp \
        src/DisplayManager/NotificationManager.cpp
git commit -m "feat: add IconLayout field to AppContentBase and JSON parsing

New 'layout' field parsed from JSON API ('left'/'right'/'none').
Defaults to Left for backward compatibility. Not yet wired to rendering."
```

---

### Task 3: Refactor AppContentRenderer to use LayoutEngine

**Files:**
- Modify: `src/DisplayManager/AppContentRenderer.cpp` (replace hardcoded positions)
- Modify: `src/AppContent.h` (update `calculateTextX` signature or remove)

- [ ] **Step 1: Replace calculateTextX with LayoutEngine**

In `src/DisplayManager/AppContentRenderer.cpp`, add include at top:

```cpp
#include "LayoutEngine.h"
```

Replace `calculateTextX` function (lines 128-136) with:

```cpp
int16_t calculateTextX(const AppContentBase& app,
                       uint16_t textWidth, bool hasIcon)
{
    LayoutMetrics m = LayoutEngine::computeLayout(app.layout, textWidth);
    if (app.center)
    {
        return m.textCenterX;
    }
    return m.textStartX;
}
```

- [ ] **Step 2: Update renderAppIcon for layout-aware icon positioning**

In `renderAppIcon()` (lines 9-88), replace the hardcoded icon X with layout-aware positioning.

Replace the push animation block (lines 17-31) with:

```cpp
        if (app.pushIcon > 0 && !noScrolling)
        {
            if (app.layout == IconLayout::Right)
            {
                // Mirror: icon pushes right off-screen
                if (app.iconPosition > 0 && !app.iconWasPushed && app.scrollposition < -8)
                {
                    app.iconPosition -= movementFactor;
                }
                if (app.scrollposition > (-9 + app.textOffset) && !app.iconWasPushed)
                {
                    app.iconPosition = -(app.scrollposition + 9 - app.textOffset);
                    if (app.iconPosition >= 9 + app.textOffset)
                    {
                        app.iconWasPushed = true;
                    }
                }
            }
            else
            {
                // Original Left behavior
                if (app.iconPosition < 0 && app.iconWasPushed == false && app.scrollposition > 8)
                {
                    app.iconPosition += movementFactor;
                }
                if (app.scrollposition < (9 - app.textOffset) && !app.iconWasPushed)
                {
                    app.iconPosition = app.scrollposition - 9 + app.textOffset;
                    if (app.iconPosition <= -9 - app.textOffset)
                    {
                        app.iconWasPushed = true;
                    }
                }
            }
        }
```

Replace icon drawing calls (lines 34-57) to use layout-aware X:

```cpp
        LayoutMetrics m = LayoutEngine::computeLayout(app.layout, 0);
        int16_t drawX = x + m.iconX + app.iconPosition + app.iconOffset;

        if (app.isGif)
        {
            if (currentFrame)
            {
                iconWidth = gifPlayer->playGif(drawX, y, &app.icon, *currentFrame);
                *currentFrame = gifPlayer->getFrame();
            }
            else
            {
                iconWidth = gifPlayer->playGif(drawX, y, &app.icon);
            }
        }
        else
        {
            iconWidth = 8;
            if (!app.jpegDataBuffer.empty())
            {
                DisplayManager.drawJPG(drawX, y, app.jpegDataBuffer.data(), app.jpegDataBuffer.size());
            }
            else
            {
                DisplayManager.drawJPG(drawX, y, app.icon);
            }
        }
```

Replace separator line (lines 59-64) with layout-aware side:

```cpp
        if (!noScrolling)
        {
            int16_t lineY1 = (app.progress > -1) ? (6 + y) : (7 + y);
            if (m.iconOnRight)
            {
                // Separator on left side of icon
                DisplayManager.drawLine(m.iconX + x + app.iconPosition - 1, 0 + y,
                                        m.iconX + x + app.iconPosition - 1, lineY1, app.background);
            }
            else
            {
                // Separator on right side of icon (original)
                DisplayManager.drawLine(iconWidth + x + app.iconPosition + app.iconOffset, 0 + y,
                                        iconWidth + x + app.iconPosition, lineY1, app.background);
            }
        }
```

Update progress bar (line 74) to be layout-aware:

```cpp
    if (app.progress > -1)
    {
        LayoutMetrics pm = LayoutEngine::computeLayout(app.layout, 0);
        int16_t barStart = pm.hasIcon && !pm.iconOnRight ? 8 : 0;
        int16_t barEnd = pm.hasIcon && pm.iconOnRight ? 23 : 31;
        DisplayManager.drawProgressBar(barStart, 7 + y, app.progress, app.pColor, app.pbColor);
    }
```

- [ ] **Step 3: Update updateScrollAnimation for layout-aware start positions**

Replace the initial position block (lines 110-124) with:

```cpp
        ++app.scrollDelay;
        LayoutMetrics m = LayoutEngine::computeLayout(app.layout, 0);
        if (m.hasIcon)
        {
            if (app.iconWasPushed && app.pushIcon == 1)
            {
                app.scrollposition = 0 + app.textOffset;
            }
            else
            {
                app.scrollposition = m.textStartX + app.textOffset;
            }
        }
        else
        {
            app.scrollposition = 0 + app.textOffset;
        }
```

- [ ] **Step 4: Build**

Run: `pio run -e ulanzi`
Expected: SUCCESS

- [ ] **Step 5: Run all tests**

Run: `pio test -e native_test`
Expected: All pass

- [ ] **Step 6: Commit**

```bash
git add src/DisplayManager/AppContentRenderer.cpp src/AppContent.h
git commit -m "refactor(display): use LayoutEngine in AppContentRenderer

Replace hardcoded icon/text positions with LayoutEngine::computeLayout().
Supports left, right, and no-icon layouts. Push animation mirrors for
right layout. Separator line draws on correct side."
```

---

### Task 4: Update custom apps and notifications scroll reset positions

**Files:**
- Modify: `src/Apps/Apps_CustomApp.cpp:123,131` (scroll reset positions)
- Modify: `src/Overlays/Overlays.cpp:120` (scroll reset position)

- [ ] **Step 1: Update Apps_CustomApp.cpp scroll reset**

In `src/Apps/Apps_CustomApp.cpp`, add include:

```cpp
#include "LayoutEngine.h"
```

Replace hardcoded `9 + ca->textOffset` scroll resets. There are two occurrences (around lines 123 and 131):

```cpp
// Replace both: ca->scrollposition = 9 + ca->textOffset;
// With:
LayoutMetrics m = LayoutEngine::computeLayout(ca->layout, 0);
ca->scrollposition = m.textStartX + ca->textOffset;
```

Also update `availableWidth` calculation (line 96):

```cpp
// Replace: uint16_t availableWidth = hasIcon ? 24 : 32;
// With:
LayoutMetrics lm = LayoutEngine::computeLayout(ca->layout, 0);
uint16_t availableWidth = lm.textAvailableWidth;
```

- [ ] **Step 2: Update Overlays.cpp scroll reset**

In `src/Overlays/Overlays.cpp`, add include:

```cpp
#include "LayoutEngine.h"
```

Replace hardcoded scroll reset (around line 120):

```cpp
// Replace: notifications[0].scrollposition = 9 + notifications[0].textOffset;
// With:
LayoutMetrics m = LayoutEngine::computeLayout(notifications[0].layout, 0);
notifications[0].scrollposition = m.textStartX + notifications[0].textOffset;
```

Also update `availableWidth` (around line 109):

```cpp
// Replace: uint16_t availableWidth = hasIcon ? 24 : 32;
// With:
LayoutMetrics lm = LayoutEngine::computeLayout(notifications[0].layout, 0);
uint16_t availableWidth = lm.textAvailableWidth;
```

- [ ] **Step 3: Build**

Run: `pio run -e ulanzi`
Expected: SUCCESS

- [ ] **Step 4: Run all tests**

Run: `pio test -e native_test`
Expected: All pass

- [ ] **Step 5: Commit**

```bash
git add src/Apps/Apps_CustomApp.cpp src/Overlays/Overlays.cpp
git commit -m "refactor: use LayoutEngine for scroll reset in custom apps and notifications

Replace hardcoded scroll start positions and available width calculations
with LayoutEngine::computeLayout()."
```

---

### Task 5: Add nativeIconLayout setting to AppConfig with persistence

**Files:**
- Modify: `lib/config/src/ConfigTypes.h` (add field to `AppConfig`)
- Modify: `src/Globals.cpp` (NVS load/save)
- Modify: `src/DisplayManager/DisplayManager_Settings.cpp` (expose in settings JSON)

- [ ] **Step 1: Add field to AppConfig**

In `lib/config/src/ConfigTypes.h`, add include at top:

```cpp
#include "LayoutEngine.h"
```

Add field to `AppConfig` struct, after `scrollSpeed`:

```cpp
    IconLayout nativeIconLayout = IconLayout::Left;
```

- [ ] **Step 2: Add NVS load/save in Globals.cpp**

In `src/Globals.cpp`, add include:

```cpp
#include "LayoutEngine.h"
```

In `loadSettings()`, after the `appConfig.scrollSpeed` line (around line 288):

```cpp
    appConfig.nativeIconLayout = static_cast<IconLayout>(Settings.getUChar("NILAYOUT", 0));
```

In `saveSettings()`, after the `appConfig.scrollSpeed` line (around line 337):

```cpp
    Settings.putUChar("NILAYOUT", static_cast<uint8_t>(appConfig.nativeIconLayout));
```

- [ ] **Step 3: Expose in settings JSON**

In `src/DisplayManager/DisplayManager_Settings.cpp`:

Add include if not present:

```cpp
#include "LayoutEngine.h"
```

In `getSettings()`, after line `doc["SSPEED"]` (line 93):

```cpp
    doc["NILAYOUT"] = layoutToString(appConfig.nativeIconLayout);
```

In `setNewSettings()`, add parsing block (find the pattern of existing setting keys):

```cpp
    if (doc.containsKey("NILAYOUT"))
    {
        appConfig.nativeIconLayout = layoutFromString(doc["NILAYOUT"].as<String>());
    }
```

- [ ] **Step 4: Build**

Run: `pio run -e ulanzi`
Expected: SUCCESS

- [ ] **Step 5: Run all tests**

Run: `pio test -e native_test`
Expected: All pass

- [ ] **Step 6: Commit**

```bash
git add lib/config/src/ConfigTypes.h src/Globals.cpp \
        src/DisplayManager/DisplayManager_Settings.cpp
git commit -m "feat(config): add nativeIconLayout setting with NVS persistence

New 'NILAYOUT' setting controls icon position for native apps
(Temp/Hum/Bat). Values: 'left', 'right', 'none'. Persisted in NVS,
exposed via settings JSON API."
```

---

### Task 6: Wire native apps (Temp/Hum/Bat) to use layout setting

**Files:**
- Modify: `src/Apps/Apps_NativeApps.cpp:262-321` (TempApp, HumApp, BatApp)

- [ ] **Step 1: Add LayoutEngine include**

In `src/Apps/Apps_NativeApps.cpp`, add at top:

```cpp
#include "LayoutEngine.h"
```

- [ ] **Step 2: Refactor TempApp**

Replace TempApp function body (lines 262-288):

```cpp
void TempApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Temperature"))
        return;

    applyNativeAppColor(colorConfig.tempColor);

    LayoutMetrics m = LayoutEngine::computeLayout(appConfig.nativeIconLayout, 0);

    if (m.hasIcon)
    {
        matrix->drawRGBBitmap(x + m.iconX, y, icon_234, 8, 8);
    }

    String tempStr;
    if (timeConfig.isCelsius)
    {
        tempStr = String(sensorConfig.currentTemp, timeConfig.tempDecimalPlaces) + "\xB0" + "C";
    }
    else
    {
        double tempF = (sensorConfig.currentTemp * 9 / 5) + 32;
        tempStr = String(tempF, timeConfig.tempDecimalPlaces) + "\xB0" + "F";
    }

    uint16_t textWidth = getTextWidth(tempStr.c_str(), 0);
    LayoutMetrics tm = LayoutEngine::computeLayout(appConfig.nativeIconLayout, textWidth);
    int16_t textX = tm.textCenterX;

    DisplayManager.setCursor(textX + x, 6 + y);
    DisplayManager.matrixPrint(tempStr.c_str());
}
```

- [ ] **Step 3: Refactor HumApp**

Replace HumApp function body (lines 293-305):

```cpp
void HumApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Humidity"))
        return;

    applyNativeAppColor(colorConfig.humColor);

    LayoutMetrics m = LayoutEngine::computeLayout(appConfig.nativeIconLayout, 0);

    if (m.hasIcon)
    {
        matrix->drawRGBBitmap(x + m.iconX, y + 1, icon_2075, 8, 8);
    }

    String humStr = String(static_cast<int>(sensorConfig.currentHum)) + "%";
    uint16_t textWidth = getTextWidth(humStr.c_str(), 0);
    LayoutMetrics tm = LayoutEngine::computeLayout(appConfig.nativeIconLayout, textWidth);
    int16_t textX = tm.textCenterX;

    DisplayManager.setCursor(textX + x, 6 + y);
    DisplayManager.matrixPrint(humStr.c_str());
}
```

- [ ] **Step 4: Refactor BatApp**

Replace BatApp function body (lines 310-321):

```cpp
void BatApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Battery"))
        return;

    applyNativeAppColor(colorConfig.batColor);

    LayoutMetrics m = LayoutEngine::computeLayout(appConfig.nativeIconLayout, 0);

    if (m.hasIcon)
    {
        matrix->drawRGBBitmap(x + m.iconX, y, icon_1486, 8, 8);
    }

    String batStr = String(static_cast<int>(batteryConfig.percent)) + "%";
    uint16_t textWidth = getTextWidth(batStr.c_str(), 0);
    LayoutMetrics tm = LayoutEngine::computeLayout(appConfig.nativeIconLayout, textWidth);
    int16_t textX = tm.textCenterX;

    DisplayManager.setCursor(textX + x, 6 + y);
    DisplayManager.matrixPrint(batStr.c_str());
}
```

- [ ] **Step 5: Build**

Run: `pio run -e ulanzi`
Expected: SUCCESS

- [ ] **Step 6: Run all tests**

Run: `pio test -e native_test`
Expected: All pass

- [ ] **Step 7: Commit**

```bash
git add src/Apps/Apps_NativeApps.cpp
git commit -m "feat(apps): wire native apps to use nativeIconLayout setting

TempApp, HumApp, BatApp now use LayoutEngine for icon/text positioning.
Icon can be left, right, or hidden based on NILAYOUT setting."
```

---

### Task 7: Update documentation

**Files:**
- Modify: `lib/services/CLAUDE.md` (add LayoutEngine to service table)
- Modify: `src/CLAUDE.md` (document NILAYOUT setting)
- Modify: `CLAUDE.md` (update service count 13→14, test counts)

- [ ] **Step 1: Update lib/services/CLAUDE.md**

Add to the service map table:

```
| **LayoutEngine** | Icon/text position: left, right, none layouts | Yes | `computeLayout()`, `layoutToString()`, `layoutFromString()` |
```

Add to the dependency graph:

```
LayoutEngine (standalone)
```

Add to the "Who Uses What" table:

```
| DisplayManager, Apps, Overlays | LayoutEngine |
```

Add to the test coverage table:

```
| LayoutEngine | `test_layout_engine/` |
```

- [ ] **Step 2: Update src/CLAUDE.md**

In the AppConfig section (Globals & Config), add the new field to the table:

```
| `nativeIconLayout` | IconLayout | `Left` | Icon position for native apps |
```

- [ ] **Step 3: Update root CLAUDE.md**

Update service count from 13 to 14 in all occurrences. Update test suite count if changed.

- [ ] **Step 4: Commit**

```bash
git add lib/services/CLAUDE.md src/CLAUDE.md CLAUDE.md
git commit -m "docs: add LayoutEngine service to documentation

Update service count to 14, document nativeIconLayout setting,
add LayoutEngine to service map and test coverage tables."
```

---

### Task 8: Final verification

- [ ] **Step 1: Run full test suite**

Run: `pio test -e native_test`
Expected: 26 suites, 470 tests PASS

- [ ] **Step 2: Build firmware**

Run: `pio run -e ulanzi`
Expected: SUCCESS, flash < 70%

- [ ] **Step 3: Run linters**

Run: `python3 -m pre_commit run --all-files`
Expected: All pass

- [ ] **Step 4: Verify backward compatibility**

Check that `IconLayout::Left` is default (value 0):
- Missing `"layout"` in JSON → Left
- Missing `NILAYOUT` in NVS → Left
- All existing custom apps render identically
