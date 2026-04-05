#include "UnicodeFont.h"

const UniGlyph *findGlyph(const UniFont &font, uint16_t codepoint)
{
    int lo = 0;
    int hi = font.glyphCount - 1;
    while (lo <= hi)
    {
        int mid = (lo + hi) / 2;
        uint16_t cp = UNI_READ_WORD(&font.glyphs[mid].codepoint);
        if (cp == codepoint)
            return &font.glyphs[mid];
        if (cp < codepoint)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return nullptr;
}

uint16_t utf8NextCodepoint(const char *&ptr)
{
    if (!ptr || *ptr == '\0')
        return 0;

    uint8_t b0 = static_cast<uint8_t>(*ptr);

    // Single-byte ASCII (0xxxxxxx)
    if (b0 < 0x80)
    {
        ptr++;
        return b0;
    }

    // Determine sequence length from leading byte
    uint16_t codepoint;
    int remaining;

    if ((b0 & 0xE0) == 0xC0) // 2-byte (110xxxxx)
    {
        codepoint = b0 & 0x1F;
        remaining = 1;
    }
    else if ((b0 & 0xF0) == 0xE0) // 3-byte (1110xxxx)
    {
        codepoint = b0 & 0x0F;
        remaining = 2;
    }
    else if ((b0 & 0xF8) == 0xF0) // 4-byte (11110xxx) — BMP only, cap at U+FFFF
    {
        codepoint = b0 & 0x07;
        remaining = 3;
    }
    else
    {
        // Invalid leading byte — skip it, return replacement
        ptr++;
        return 0xFFFD;
    }

    ptr++;
    for (int i = 0; i < remaining; i++)
    {
        uint8_t b = static_cast<uint8_t>(*ptr);
        if ((b & 0xC0) != 0x80)
        {
            // Missing continuation byte — don't advance past it
            return 0xFFFD;
        }
        codepoint = (codepoint << 6) | (b & 0x3F);
        ptr++;
    }

    // Clamp to BMP
    if (codepoint > 0xFFFF)
        return 0xFFFD;

    return codepoint;
}

uint16_t utf8Length(const char *text)
{
    uint16_t count = 0;
    const char *p = text;
    while (utf8NextCodepoint(p) != 0)
        count++;
    return count;
}

/// Map ASCII lowercase a-z to uppercase A-Z. Pass through everything else.
static uint16_t asciiToUpper(uint16_t cp)
{
    if (cp >= 'a' && cp <= 'z')
        return cp - 32;
    return cp;
}

float getUnicodeTextWidth(const UniFont &font, const char *text, byte textCase, bool uppercaseLetters)
{
    float width = 0;
    const char *p = text;
    uint16_t cp;

    while ((cp = utf8NextCodepoint(p)) != 0)
    {
        if ((uppercaseLetters && textCase == 0) || textCase == 1)
            cp = asciiToUpper(cp);

        width += getGlyphAdvance(font, cp, 4);
    }
    return width;
}

uint8_t getGlyphAdvance(const UniFont &font, uint16_t codepoint, uint8_t defaultWidth)
{
    const UniGlyph *g = findGlyph(font, codepoint);
    if (g)
        return UNI_READ_BYTE(&g->xAdvance);
    return defaultWidth;
}
