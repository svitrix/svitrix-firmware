#pragma once

#include <IPixelCanvas.h>
#include <FastLED_NeoMatrix.h>

/// Adapter: delegates IPixelCanvas calls to a real FastLED_NeoMatrix.
class NeoMatrixCanvas : public IPixelCanvas
{
    FastLED_NeoMatrix *matrix_;

  public:
    explicit NeoMatrixCanvas(FastLED_NeoMatrix *m)
        : matrix_(m)
    {
    }

    void drawPixel(int16_t x, int16_t y, CRGB color) override
    {
        matrix_->drawPixel(x, y, color);
    }
    void drawPixel(int16_t x, int16_t y, CHSV color) override
    {
        matrix_->drawPixel(x, y, color);
    }
    void drawPixel(int16_t x, int16_t y, uint16_t color) override
    {
        matrix_->drawPixel(x, y, color);
    }

    uint16_t Color(uint8_t r, uint8_t g, uint8_t b) override
    {
        return matrix_->Color(r, g, b);
    }

    void drawRGBBitmap(int16_t x, int16_t y,
                       const uint16_t bitmap[],
                       int16_t w, int16_t h) override
    {
        matrix_->drawRGBBitmap(x, y, bitmap, w, h);
    }

    void fillRect(int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  uint16_t color) override
    {
        matrix_->fillRect(x, y, w, h, color);
    }
};
