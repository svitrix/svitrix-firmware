#pragma once

#ifdef UNIT_TEST
#include <cstdint>
#include <cstddef>
#include <cctype>
typedef uint8_t byte;
#else
#include <Arduino.h>
#endif

#ifdef ESP32
#include <pgmspace.h>
#define UNI_READ_BYTE(addr) pgm_read_byte(addr)
#define UNI_READ_WORD(addr) pgm_read_word(addr)
#else
#define UNI_READ_BYTE(addr) (*(const uint8_t *)(addr))
#define UNI_READ_WORD(addr) (*(const uint16_t *)(addr))
#ifndef PROGMEM
#define PROGMEM
#endif
#endif

/// A single glyph in a Unicode bitmap font.
/// Glyphs are stored sorted by codepoint for binary-search lookup.
struct UniGlyph {
    uint16_t codepoint;     ///< Unicode code point (U+0020 – U+FFFF)
    uint16_t bitmapOffset;  ///< Byte offset into UniFont::bitmap
    uint8_t  width;         ///< Bitmap width in pixels
    uint8_t  height;        ///< Bitmap height in pixels
    uint8_t  xAdvance;      ///< Cursor advance after drawing (pixels)
    int8_t   xOffset;       ///< X offset from cursor to glyph upper-left
    int8_t   yOffset;       ///< Y offset from cursor to glyph upper-left
};

/// Unicode bitmap font: a bitmap array + sorted glyph table.
struct UniFont {
    const uint8_t  *bitmap;     ///< Concatenated monochrome glyph bitmaps (1bpp, MSB first)
    const UniGlyph *glyphs;     ///< Glyph array, sorted by codepoint
    uint16_t        glyphCount; ///< Number of entries in glyphs[]
    uint8_t         yAdvance;   ///< Newline distance (pixels)
};

/// Find a glyph by Unicode codepoint using binary search.
/// @return Pointer to matching UniGlyph, or nullptr if not found.
const UniGlyph *findGlyph(const UniFont &font, uint16_t codepoint);

/// Decode the next UTF-8 codepoint from a string, advancing the pointer.
/// Returns 0 and does not advance on end-of-string or invalid sequence.
/// On invalid bytes, advances past them and returns U+FFFD (replacement char).
uint16_t utf8NextCodepoint(const char *&ptr);

/// Count the number of Unicode codepoints in a UTF-8 string.
uint16_t utf8Length(const char *text);

/// Calculate pixel width of a UTF-8 string using glyph xAdvance values.
/// @param textCase  0=use uppercaseLetters flag, 1=force uppercase, 2=as-is
float getUnicodeTextWidth(const UniFont &font, const char *text, byte textCase, bool uppercaseLetters);

/// Get the xAdvance for a single codepoint. Returns defaultWidth if glyph not found.
uint8_t getGlyphAdvance(const UniFont &font, uint16_t codepoint, uint8_t defaultWidth = 4);
