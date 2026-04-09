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

/// Compact glyph entry — 5 bytes instead of 9.
/// Glyphs are stored sorted by codepoint for binary-search lookup.
///
/// Packed byte layout: [height:3][xAdvance:3][yOffIdx:2]
///   height   (bits 7-5): glyph height in pixels (1-7)
///   xAdvance (bits 4-2): cursor advance in pixels (0-7)
///   yOffIdx  (bits 1-0): index into kYOffsets[] lookup table
///
/// Constant fields removed (all glyphs share these):
///   width  = 8  (bitmap row = 1 byte, stored in UniFont::bitmapWidth)
///   xOffset = 0 (stored in UniFont::xOffset)
struct UniGlyph {
    uint16_t codepoint;    ///< Unicode code point (U+0020 – U+FFFF)
    uint16_t bitmapOffset; ///< Byte offset into UniFont::bitmap
    uint8_t  packed;       ///< height[7:5] | xAdvance[4:2] | yOffIdx[1:0]
};

/// yOffset lookup table (4 possible values, indexed by bits [1:0] of packed).
static constexpr int8_t kYOffsets[] = {-5, -4, -3, -1};

/// Inline accessors — work on both native and PROGMEM pointers.
inline uint8_t glyphHeight(const UniGlyph *g) { return (UNI_READ_BYTE(&g->packed) >> 5) & 0x07; }
inline uint8_t glyphXAdvance(const UniGlyph *g) { return (UNI_READ_BYTE(&g->packed) >> 2) & 0x07; }
inline int8_t  glyphYOffset(const UniGlyph *g) { return kYOffsets[UNI_READ_BYTE(&g->packed) & 0x03]; }

/// Unicode bitmap font: a bitmap array + sorted glyph table.
struct UniFont {
    const uint8_t  *bitmap;     ///< Concatenated monochrome glyph bitmaps (1bpp, MSB first)
    const UniGlyph *glyphs;     ///< Glyph array, sorted by codepoint
    uint16_t        glyphCount; ///< Number of entries in glyphs[]
    uint8_t         yAdvance;   ///< Newline distance (pixels)
    uint8_t         bitmapWidth; ///< Bitmap row width in pixels (always 8 for this font)
    int8_t          xOffset;    ///< X offset from cursor (always 0 for this font)
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

/// Callback for each pixel set by renderGlyph(). Coordinates are absolute (cursor + offset applied).
typedef void (*PixelCallback)(int16_t x, int16_t y, void *userData);

/// Render a single glyph's bitmap, calling pixelCb for each set pixel.
/// @return xAdvance of the rendered glyph, or 0 if not found.
uint8_t renderGlyph(const UniFont &font, uint16_t codepoint, int16_t cursorX, int16_t cursorY, PixelCallback pixelCb, void *userData);
