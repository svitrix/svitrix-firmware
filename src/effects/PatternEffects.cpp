#include "PatternEffects.h"

namespace
{

CRGB tempLedsRadar[kMatrixWidth][kMatrixHeight];
uint16_t theaterOffset = 0;
uint32_t theaterLastUpdate = 0;
float checkerIndex1 = 0;
float checkerIndex2 = 0;
uint8_t fadeHue = 0;
int16_t movingLinePos = 0;
int8_t movingLineDir = 1;
uint32_t movingLineLastUpdate = 0;
uint8_t movingLinePalIdx = 0;
float radarBeamAngle = 0;

} // namespace

void TheaterChase(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    if (millis() - theaterLastUpdate > 100 - settings->speed * 10)
    {
        theaterLastUpdate = millis();
        ++theaterOffset;
    }

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            if ((i + theaterOffset) % 3 == 0)
            {
                auto colorIndex = static_cast<uint8_t>((i * 256 / kMatrixWidth) & 255);
                CRGB color = ColorFromPalette(settings->palette, colorIndex, 255, blendMode(settings));
                canvas.drawPixel(x + i, y + j, color);
            }
            else
            {
                canvas.drawPixel(x + i, y + j, canvas.Color(0, 0, 0));
            }
        }
    }
}

void CheckerboardEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    CRGB color1 = ColorFromPalette(settings->palette, checkerIndex1, 255, blendMode(settings));
    CRGB color2 = ColorFromPalette(settings->palette, checkerIndex2, 255, blendMode(settings));

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            bool isEvenCell = ((i + j) % 2 == 0);
            canvas.drawPixel(x + i, y + j, isEvenCell ? color1 : color2);
        }
    }

    checkerIndex1 += settings->speed * 0.6f;
    checkerIndex2 += settings->speed;
}

void Fade(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    fadeHue += settings->speed;

    for (int16_t j = kMatrixHeight - 1; j >= 0; j--)
    {
        auto palIdx = static_cast<uint8_t>(fadeHue + (j * 256 / kMatrixHeight));
        CRGB color = ColorFromPalette(settings->palette, palIdx, 255, blendMode(settings));
        for (uint16_t i = 0; i < kMatrixWidth; i++)
        {
            canvas.drawPixel(x + i, y + j, color);
        }
    }
}

void MovingLine(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    if (millis() - movingLineLastUpdate > 100 - settings->speed * 10)
    {
        movingLineLastUpdate = millis();
        movingLinePos += movingLineDir;
        if (movingLinePos <= 0 || movingLinePos + 1 >= kMatrixHeight)
            movingLineDir = -movingLineDir;
        ++movingLinePalIdx;
    }

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        CRGB color = ColorFromPalette(settings->palette, movingLinePalIdx, 255, blendMode(settings));
        canvas.drawPixel(x + i, y + movingLinePos, color);
    }
}

void RadarEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    for (uint16_t i = 0; i < kMatrixWidth; i++)
        for (uint16_t j = 0; j < kMatrixHeight; j++)
            tempLedsRadar[i][j].fadeToBlackBy(20);

    const int maxRadius = max(kMatrixWidth, kMatrixHeight);
    for (uint8_t r = 0; r <= maxRadius; r++)
    {
        auto i = static_cast<uint16_t>(kMatrixWidth / 2 + r * (cos8(radarBeamAngle) - 128) / 128.0f);
        auto j = static_cast<uint16_t>(kMatrixHeight / 2 - r * (sin8(radarBeamAngle) - 128) / 128.0f);
        if (i < kMatrixWidth && j < kMatrixHeight)
            tempLedsRadar[i][j] = ColorFromPalette(settings->palette, radarBeamAngle, 255, blendMode(settings));
    }

    for (uint16_t i = 0; i < kMatrixWidth; i++)
        for (uint16_t j = 0; j < kMatrixHeight; j++)
            canvas.drawPixel(x + i, y + j, tempLedsRadar[i][j]);

    radarBeamAngle += settings->speed;
}

// ---- Reset all pattern effect state ----

void resetPatternEffectState()
{
    memset(tempLedsRadar, 0, sizeof(tempLedsRadar));
    theaterOffset = 0;
    theaterLastUpdate = 0;
    checkerIndex1 = 0;
    checkerIndex2 = 0;
    fadeHue = 0;
    movingLinePos = 0;
    movingLineDir = 1;
    movingLineLastUpdate = 0;
    movingLinePalIdx = 0;
    radarBeamAngle = 0;
}
