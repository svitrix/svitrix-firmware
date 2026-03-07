#pragma once

#include <cstdint>

struct CRGB;
struct CHSV;

/// Abstract drawing surface for visual effects.
/// Decouples effect logic from FastLED_NeoMatrix hardware.
class IPixelCanvas {
public:
    virtual ~IPixelCanvas() = default;

    virtual void drawPixel(int16_t x, int16_t y, CRGB color) = 0;
    virtual void drawPixel(int16_t x, int16_t y, CHSV color) = 0;
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

    virtual uint16_t Color(uint8_t r, uint8_t g, uint8_t b) = 0;

    virtual void drawRGBBitmap(int16_t x, int16_t y,
                                const uint16_t bitmap[],
                                int16_t w, int16_t h) = 0;

    virtual void fillRect(int16_t x, int16_t y,
                           int16_t w, int16_t h,
                           uint16_t color) = 0;
};
