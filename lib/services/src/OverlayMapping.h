#pragma once

/**
 * @file OverlayMapping.h
 * @brief Weather overlay effect enum and string conversion.
 *
 * Defines the OverlayEffect enum (weather particle effects rendered on top
 * of the LED matrix) and bidirectional string↔enum mapping.
 *
 * Extracted from: effects.h (enum), effects.cpp (getOverlay), DisplayManager.cpp (getOverlayName).
 * Used by: DisplayManager (overlay rendering), MQTTManager (API overlay commands),
 *          effects.h (re-exports via #include "OverlayMapping.h").
 * Tests: test/test_native/test_overlay/
 */

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * Weather overlay effects for the LED matrix.
 *
 * Each effect renders animated particles on top of the current display:
 *   NONE    — no overlay (clear weather).
 *   DRIZZLE — light rain (few slow particles).
 *   RAIN    — moderate rain (more particles, faster).
 *   SNOW    — snowfall (white, slow, drifting).
 *   STORM   — heavy rain (dense, fast particles).
 *   THUNDER — storm + lightning flashes.
 *   FROST   — static frost crystals on edges.
 */
enum OverlayEffect
{
    NONE,
    DRIZZLE,
    RAIN,
    SNOW,
    STORM,
    THUNDER,
    FROST,
};

/**
 * Convert an OverlayEffect enum value to its lowercase string name.
 *
 * @param effect The overlay effect enum value.
 * @return Lowercase name: "clear", "drizzle", "rain", "snow", "storm",
 *         "thunder", "frost". Returns "invalid" for unknown values.
 */
String overlayToString(OverlayEffect effect);

/**
 * Parse a string into an OverlayEffect enum. Case-insensitive.
 *
 * @param overlay String name (e.g. "Rain", "SNOW", "clear").
 * @return Matching OverlayEffect, or NONE for unrecognized strings.
 */
OverlayEffect overlayFromString(const String &overlay);

