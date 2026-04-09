#pragma once

/**
 * @file TextUtils.h
 * @brief Text measurement for Unicode bitmap fonts.
 *
 * All functions are pure — no hardware access.
 * Uses UnicodeFont for glyph lookup and width calculation.
 * Used by: DisplayManager (scroll width, text rendering), Apps (text layout).
 * Tests: test/test_native/test_text_metrics/
 */

#include <cstdint>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

#include "UnicodeFont.h"

/**
 * Set the global font used by getTextWidth().
 * Must be called once at startup (from main.cpp) before any text measurement.
 */
void setTextFont(const UniFont *font);

/**
 * Calculate the total pixel width of a UTF-8 text string.
 *
 * Decodes UTF-8 codepoints and sums per-glyph xAdvance values from the font.
 *
 * @param text              Null-terminated UTF-8 C string.
 * @param textCase          Case conversion mode: 0 = auto (uses uppercaseLetters flag),
 *                          1 = force uppercase, 2 = as-is.
 * @param uppercaseLetters  If true and textCase==0, convert ASCII to uppercase before lookup.
 * @return Total width in pixels. Used to calculate scroll distance.
 */
float getTextWidth(const char *text, byte textCase, bool uppercaseLetters);
