#pragma once
#include <Arduino.h>

/// Interface for drawing primitives and text rendering.
/// Consumers: UpdateManager, GameManager, SlotMachine, SvitrixSays, MenuManager, ServerManager.
class IDisplayRenderer
{
public:
    virtual ~IDisplayRenderer() = default;
    virtual void clear() = 0;
    virtual void show() = 0;
    virtual void resetTextColor() = 0;
    virtual void setTextColor(uint32_t color) = 0;
    virtual void printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase) = 0;
    virtual void HSVtext(int16_t x, int16_t y, const char *text, bool clear, byte textCase) = 0;
    virtual void drawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) = 0;
    virtual void drawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h) = 0;
    virtual void drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) = 0;
    virtual void drawProgressBar(int16_t x, int16_t y, int progress, uint32_t pColor, uint32_t pbColor) = 0;
    virtual void drawMenuIndicator(int cur, int total, uint32_t color) = 0;
};
