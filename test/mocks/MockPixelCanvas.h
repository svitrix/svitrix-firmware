#ifndef MOCK_PIXEL_CANVAS_H
#define MOCK_PIXEL_CANVAS_H

#include "IPixelCanvas.h"
#include "FastLED.h"

struct PixelRecord {
    int16_t x, y;
    CRGB color;
};

struct MockPixelCanvas : public IPixelCanvas {
    static constexpr int kMaxPixels = 1024;
    PixelRecord pixels[kMaxPixels];
    int pixelCount = 0;
    int drawPixelCalls = 0;
    int fillRectCalls = 0;
    int drawBitmapCalls = 0;

    void drawPixel(int16_t x, int16_t y, CRGB color) override {
        drawPixelCalls++;
        if (pixelCount < kMaxPixels)
            pixels[pixelCount++] = {x, y, color};
    }

    void drawPixel(int16_t x, int16_t y, CHSV color) override {
        CRGB rgb(color);
        drawPixel(x, y, rgb);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        uint8_t r = (color >> 11) << 3;
        uint8_t g = ((color >> 5) & 0x3F) << 2;
        uint8_t b = (color & 0x1F) << 3;
        drawPixel(x, y, CRGB(r, g, b));
    }

    uint16_t Color(uint8_t r, uint8_t g, uint8_t b) override {
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }

    void drawRGBBitmap(int16_t, int16_t, const uint16_t[], int16_t, int16_t) override {
        drawBitmapCalls++;
    }

    void fillRect(int16_t, int16_t, int16_t, int16_t, uint16_t) override {
        fillRectCalls++;
    }

    void reset() {
        pixelCount = 0;
        drawPixelCalls = 0;
        fillRectCalls = 0;
        drawBitmapCalls = 0;
    }

    // Look up the last pixel drawn at (px, py)
    CRGB getPixel(int16_t px, int16_t py) const {
        for (int i = pixelCount - 1; i >= 0; i--)
            if (pixels[i].x == px && pixels[i].y == py)
                return pixels[i].color;
        return CRGB(0, 0, 0);
    }

    // Check if any pixel was drawn at (px, py)
    bool hasPixel(int16_t px, int16_t py) const {
        for (int i = 0; i < pixelCount; i++)
            if (pixels[i].x == px && pixels[i].y == py)
                return true;
        return false;
    }
};

#endif // MOCK_PIXEL_CANVAS_H
