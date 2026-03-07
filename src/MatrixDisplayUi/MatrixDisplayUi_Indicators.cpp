/**
 * @file MatrixDisplayUi_Indicators.cpp
 * @brief Status indicator rendering and configuration.
 *
 * Manages three RGB status indicators displayed at the right edge
 * of the 32x8 matrix. Each indicator supports three modes:
 *   - Solid color (default)
 *   - Blink: on/off toggling at a configurable interval
 *   - Fade: smooth sine-wave pulsing over a configurable period
 *
 * Indicator positions on the matrix:
 *   Indicator 1 (top-right):    pixels (31,0), (30,0), (31,1)
 *   Indicator 2 (middle-right): pixels (31,3), (31,4)
 *   Indicator 3 (bottom-right): pixels (31,7), (31,6), (30,7)
 */
#include "MatrixDisplayUi.h"

// ── Indicator drawing ───────────────────────────────────────────────

/// Render all active indicators onto the matrix buffer.
/// Skips disabled indicators. For each enabled one, computes the
/// draw color based on mode (solid / blink / fade) and paints its pixels.
void MatrixDisplayUi::drawIndicators()
{
    uint32_t drawColor;

    if (indicator1State)
    {
        if (indicator1Blink)
        {
            if (millis() % (2 * indicator1Blink) < indicator1Blink)
            {
                drawColor = indicator1Color;
            }
            else
            {
                drawColor = 0;
            }
        }
        else if (indicator1Fade)
        {
            drawColor = fadeColor(indicator1Color, indicator1Fade);
        }
        else
        {
            drawColor = indicator1Color;
        }
        matrix->drawPixel(31, 0, drawColor);
        matrix->drawPixel(30, 0, drawColor);
        matrix->drawPixel(31, 1, drawColor);
    }

    if (indicator2State)
    {
        if (indicator2Blink)
        {
            if (millis() % (2 * indicator2Blink) < indicator2Blink)
            {
                drawColor = indicator2Color;
            }
            else
            {
                drawColor = 0;
            }
        }
        else if (indicator2Fade)
        {
            drawColor = fadeColor(indicator2Color, indicator2Fade);
        }
        else
        {
            drawColor = indicator2Color;
        }
        matrix->drawPixel(31, 3, drawColor);
        matrix->drawPixel(31, 4, drawColor);
    }

    if (indicator3State)
    {
        if (indicator3Blink)
        {
            if (millis() % (2 * indicator3Blink) < indicator3Blink)
            {
                drawColor = indicator3Color;
            }
            else
            {
                drawColor = 0;
            }
        }
        else if (indicator3Fade)
        {
            drawColor = fadeColor(indicator3Color, indicator3Fade);
        }
        else
        {
            drawColor = indicator3Color;
        }
        matrix->drawPixel(31, 7, drawColor);
        matrix->drawPixel(31, 6, drawColor);
        matrix->drawPixel(30, 7, drawColor);
    }
}

/// Compute a faded color using a sine wave cycle.
/// The brightness oscillates smoothly between 0 and full over the given interval.
/// @param color     Base RGB color (packed 0xRRGGBB)
/// @param interval  Full fade cycle period in milliseconds
/// @return Color with brightness modulated by the current sine phase
uint32_t MatrixDisplayUi::fadeColor(uint32_t color, uint32_t interval)
{
    float phase = (sin(2 * PI * millis() / float(interval)) + 1) * 0.5;
    uint8_t r = ((color >> 16) & 0xFF) * phase;
    uint8_t g = ((color >> 8) & 0xFF) * phase;
    uint8_t b = (color & 0xFF) * phase;
    return (r << 16) | (g << 8) | b;
}

// ── Indicator setters ───────────────────────────────────────────────

void MatrixDisplayUi::setIndicator1Color(uint32_t color)
{
    this->indicator1Color = color;
}

void MatrixDisplayUi::setIndicator1State(bool state)
{
    this->indicator1State = state;
}

void MatrixDisplayUi::setIndicator1Blink(int blink)
{
    this->indicator1Blink = blink;
}

void MatrixDisplayUi::setIndicator1Fade(int fade)
{
    this->indicator1Fade = fade;
}

void MatrixDisplayUi::setIndicator2Color(uint32_t color)
{
    this->indicator2Color = color;
}

void MatrixDisplayUi::setIndicator2State(bool state)
{
    this->indicator2State = state;
}

void MatrixDisplayUi::setIndicator2Blink(int blink)
{
    this->indicator2Blink = blink;
}

void MatrixDisplayUi::setIndicator2Fade(int fade)
{
    this->indicator2Fade = fade;
}

void MatrixDisplayUi::setIndicator3Color(uint32_t color)
{
    this->indicator3Color = color;
}

void MatrixDisplayUi::setIndicator3State(bool state)
{
    this->indicator3State = state;
}

void MatrixDisplayUi::setIndicator3Blink(int blink)
{
    this->indicator3Blink = blink;
}

void MatrixDisplayUi::setIndicator3Fade(int fade)
{
    this->indicator3Fade = fade;
}
