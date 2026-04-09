/**
 * @file DisplayRenderer.h
 * @brief Rendering engine extracted from DisplayManager_ (Phase 11.1).
 *
 * DisplayRenderer_ owns all drawing primitives, text rendering (SvitrixFont
 * glyphs), charts, bitmaps, JPG decoding, and JSON draw commands.
 * Implements IDisplayRenderer for consumer injection.
 *
 * Accesses shared globals (matrix, leds, cursor, textColor) via
 * DisplayManager_internal.h — same as the original monolithic code.
 */
#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include "IDisplayRenderer.h"

struct CRGB;

class DisplayRenderer_ : public IDisplayRenderer
{
  public:
    // ── IDisplayRenderer interface (11 methods) ─────────────────────
    void clear() override;
    void show() override;
    void resetTextColor() override;
    void setTextColor(uint32_t color) override;
    void printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase) override;
    void HSVtext(int16_t x, int16_t y, const char *text, bool clear, byte textCase) override;
    void drawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) override;
    void drawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h) override;
    void drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) override;
    void drawProgressBar(int16_t x, int16_t y, int progress, uint32_t pColor, uint32_t pbColor) override;
    void drawMenuIndicator(int cur, int total, uint32_t color) override;

    // ── Text rendering internals ────────────────────────────────────
    void GradientText(int16_t x, int16_t y, const char *text, int color1, int color2, bool clear, byte textCase);
    void renderColoredText(int16_t x, int16_t y, const char *text, bool rainbow, int gradient0, int gradient1, uint32_t color, uint32_t fade, uint32_t blink, byte textCase);
    void setCursor(int16_t x, int16_t y);
    void matrixPrint(const char *str);
    void matrixPrint(String str);
    void matrixPrint(char *str);
    void matrixPrint(char str[], size_t length);
    void matrixPrint(double number, uint8_t digits);

    // ── Drawing primitives ──────────────────────────────────────────
    void drawPixel(int16_t x0, int16_t y0, uint32_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint32_t color);
    void drawJPG(uint16_t x, uint16_t y, fs::File jpgFile);
    void drawJPG(int32_t x, int32_t y, const uint8_t jpeg_data[], uint32_t data_size);
    void drawBarChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color, uint32_t barBG);
    void drawLineChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color);
    void processDrawInstructions(int16_t x, int16_t y, String& drawInstructions);
    CRGB getPixelColor(int16_t x, int16_t y);

  private:
    void matrixPrintGlyph(uint16_t codepoint);
};
