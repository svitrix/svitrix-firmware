#pragma once

/**
 * @file TextUtils.h
 * @brief Text measurement and UTF-8 decoding for the SvitrixFont bitmap font.
 *
 * All functions are pure — no hardware access.
 * Extracted from: Functions.cpp (getTextWidth, utf8ascii, CharMap).
 * Used by: DisplayManager (scroll width, text rendering), Apps (text layout).
 * Tests: test/test_native/test_text/
 */

#include <cstdint>
#include <map>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * Pixel-width lookup table for SvitrixFont glyphs.
 *
 * Key: character code point (ASCII 32-255 + some Unicode code points).
 * Value: glyph width in pixels (includes 1px inter-character gap).
 *
 * Most characters are 4px wide. Notable exceptions:
 *   - Space (32), '!' (33), 'I' (73), 'i' (105), '|' (124): 2px
 *   - 'M' (77), 'W' (87): 6px
 *   - 'N' (78), 'Q' (81): 5px
 */
extern std::map<int, uint16_t> CharMap;

/**
 * Calculate the total pixel width of a text string using CharMap.
 *
 * Sums per-character widths. Characters not in CharMap use fallback widths
 * (4-7px depending on the byte value — these cover wide Cyrillic glyphs).
 *
 * @param text              Null-terminated C string (may contain UTF-8 bytes).
 * @param textCase          Case conversion mode: 0 = auto (uses uppercaseLetters flag),
 *                          1 = force uppercase.
 * @param uppercaseLetters  If true and textCase==0, convert to uppercase before lookup.
 * @return Total width in pixels. Used to calculate scroll distance.
 */
float getTextWidth(const char *text, byte textCase, bool uppercaseLetters);

/**
 * Stateful UTF-8 to single-byte decoder for the SvitrixFont encoding.
 *
 * Feed one byte at a time. Returns 0 for continuation bytes (caller should
 * skip those), or a single-byte glyph index for the decoded character.
 *
 * Supported encodings:
 *   - ASCII (< 128): passed through unchanged.
 *   - Latin Extended (C2, C3 prefix): maps to 0x80-0xFF range.
 *   - Polish characters (C4, C5 prefix): mapped to nearest ASCII equivalent.
 *   - Cyrillic (D0, D1, D2 prefix): mapped to custom font indices 0x7F-0xEF.
 *   - Euro sign (€, 82 AC): mapped to 0xB6.
 *
 * @param ascii Single byte from a UTF-8 stream.
 * @return Decoded single-byte glyph index, or 0 if byte is a continuation.
 */
byte utf8ascii(byte ascii);

/**
 * Convert an entire UTF-8 Arduino String to a single-byte encoded string.
 *
 * Applies utf8ascii() byte-by-byte and drops zero results (continuation bytes).
 *
 * @param s Input UTF-8 string.
 * @return String with each character as a single-byte glyph index.
 */
String utf8ascii(String s);

