/**
 * @file Apps_internal.h
 * @brief Shared internal state for Apps modules.
 *
 * Declares extern references to globals defined in Apps_NativeApps.cpp
 * and forward-declares internal helper functions from Apps_Helpers.cpp.
 *
 * Only included by Apps_*.cpp files -- NOT part of the public API.
 */
#pragma once

#include "Apps.h"
#include "Globals.h"
#include "DisplayManager.h"
#include "Overlays.h"
#include "timer.h"

// ── TimeApp-only data (defined in Apps_NativeApps.cpp) ──────────────

extern const uint8_t bigdigits_mask[12][7]; ///< Bitmasks for big-digit clock segments
extern uint32_t COLOR_HOUR_ON;              ///< Binary clock hour color (red)
extern uint32_t COLOR_MINUTE_ON;            ///< Binary clock minute color (green)
extern uint32_t COLOR_SECOND_ON;            ///< Binary clock second color (blue)
extern uint32_t COLOR_OFF;                  ///< Binary clock "off" pixel color (white)

// ── Internal helpers (defined in Apps_Helpers.cpp) ──────────────────

/// Common preamble for native apps: checks notifyFlag, sets current app name.
/// @return true if the app should return immediately (notification active).
bool nativeAppGuard(const char *appName);

/// Apply per-app color override, or reset to global text color.
/// @param colorValue Per-app color (0 = use global).
void applyNativeAppColor(uint32_t colorValue);

/// Draw the 7-segment weekday indicator bar.
/// @param x, y       Top-left corner of the bar.
/// @param lineWidth   Width of each day segment in pixels.
/// @param lineSpacing Spacing between segments in pixels.
/// @param lineStart   X offset of the first segment.
void drawWeekdayBar(int16_t x, int16_t y, uint8_t lineWidth,
                    uint8_t lineSpacing, uint8_t lineStart);

/// Replace {{topic}} placeholders with live MQTT values.
/// @param text Input string with optional {{topic}} placeholders.
/// @return String with placeholders substituted.
String replacePlaceholders(const String& input);
