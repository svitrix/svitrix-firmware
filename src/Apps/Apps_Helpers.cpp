/**
 * @file Apps_Helpers.cpp
 * @brief Shared helper functions for native and custom apps.
 *
 * Extracted helpers that eliminate code duplication across native apps:
 *   - nativeAppGuard: notification check + current app tracking
 *   - applyNativeAppColor: per-app color override or global fallback
 *   - drawWeekdayBar: 7-segment weekday indicator bar
 *   - replacePlaceholders: {{topic}} → MQTT value substitution
 */
#include "Apps_internal.h"
#include "MQTTManager.h"
#include "PlaceholderUtils.h"

// ── Native app preamble ────────────────────────────────────────────

/// Common guard for all native apps. Checks the notification flag,
/// sets the current app name, and clears the custom app tracker.
/// @param appName Display name of the native app (e.g. "Time").
/// @return true if the app should return immediately (notification active).
bool nativeAppGuard(const char *appName)
{
    if (notifyFlag)
        return true;
    DisplayManager.setCurrentApp(appName);
    currentCustomApp = "";
    return false;
}

// ── Color override ─────────────────────────────────────────────────

/// Apply a per-app color if set (> 0), otherwise the global text color.
/// DisplayManager.resolveTextColor() lets any active IDisplayPolicy
/// (e.g. NightModePolicy) veto the choice, so this function stays
/// policy-agnostic.
/// @param colorValue Per-app color from colorConfig (0 = use global).
void applyNativeAppColor(uint32_t colorValue)
{
    const uint32_t preferred = (colorValue > 0) ? colorValue : colorConfig.textColor;
    DisplayManager.setTextColor(DisplayManager.resolveTextColor(preferred));
}

// ── Weekday indicator bar ──────────────────────────────────────────

/// Draw a horizontal weekday indicator bar with the current day highlighted.
/// Used by TimeApp and DateApp with different spacing parameters.
/// @param x          X origin on the matrix.
/// @param y          Y position of the bar.
/// @param lineWidth  Width of each day segment in pixels.
/// @param lineSpacing Gap between segments in pixels.
/// @param lineStart  X offset of the first segment relative to origin.
void drawWeekdayBar(int16_t x, int16_t y, uint8_t lineWidth,
                    uint8_t lineSpacing, uint8_t lineStart)
{
    uint8_t dayOffset = timeConfig.startOnMonday ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        int start = lineStart + i * (lineWidth + lineSpacing);
        int end = start + lineWidth - 1;
        uint32_t color = (i == (timer_localtime()->tm_wday + 6 + dayOffset) % 7)
                             ? colorConfig.wdcActive
                             : colorConfig.wdcInactive;
        DisplayManager.drawLine(start + x, y, end + x, y, color);
    }
}

// ── Placeholder substitution ───────────────────────────────────────

/// Replace all {{topic}} placeholders in text with live MQTT values.
/// Delegates to the hardware-free replacePlaceholdersWith() utility.
/// @param input Input string containing zero or more {{topic}} patterns.
/// @return String with all placeholders resolved via MQTTManager.
String replacePlaceholders(const String& input)
{
    return replacePlaceholdersWith(input, [](const String& key) -> String
                                   { return MQTTManager.getValueForTopic(key); });
}
