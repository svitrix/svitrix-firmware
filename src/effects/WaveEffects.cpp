#include "WaveEffects.h"

namespace
{

uint32_t pacificaTime = 0;
double plasmaTime = 0;
float plasmaCloudTime = 0;
float plasmaCloudHueShift = 0;
uint32_t swirlInLastUpdate = 0;
uint16_t swirlInAngle = 0;
uint32_t swirlOutLastUpdate = 0;
uint16_t swirlOutAngle = 0;

} // namespace

void Pacifica(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    pacificaTime += settings->speed;

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            uint16_t ulx = (pacificaTime / 8) - (i * 16);
            uint16_t uly = (pacificaTime / 4) + (j * 16);

            uint16_t v = 0;
            v += sin16(ulx * 6 + pacificaTime / 2) / 8 + 127;
            v += sin16(uly * 9 + pacificaTime / 2) / 8 + 127;
            v += sin16(ulx * 7 + uly * 2 - pacificaTime) / 16;
            v /= 3;

            CRGB color = ColorFromPalette(settings->palette, v, 255, blendMode(settings));
            canvas.drawPixel(x + i, y + j, color);
        }
    }
}

void Plasma(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            uint8_t value = sin8(i * 10 + plasmaTime) + sin8(j * 10 + plasmaTime / 2) + sin8((i + j) * 10 + plasmaTime / 3) / 3;
            CRGB color = ColorFromPalette(settings->palette, value, 255, blendMode(settings));
            canvas.drawPixel(x + i, y + j, color);
        }
    }
    plasmaTime += settings->speed;
}

void ColorWaves(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    constexpr float colorStep = 255.0f / (kMatrixWidth - 1);

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            auto paletteIndex = static_cast<uint8_t>(
                static_cast<uint8_t>(i * colorStep + millis() * settings->speed / 100) % 256);
            CRGB color = ColorFromPalette(settings->palette, paletteIndex, 255, blendMode(settings));
            canvas.drawPixel(x + i, y + j, color);
        }
    }
}

void PlasmaCloudEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            float noise = inoise8(i * 16, j * 16, plasmaCloudTime);
            auto hue = static_cast<uint8_t>(noise * 255 / 1024 + plasmaCloudHueShift);
            CRGB color = ColorFromPalette(settings->palette, hue, 255, blendMode(settings));
            canvas.drawPixel(x + i, y + j, color);
        }
    }

    plasmaCloudTime += settings->speed;
    plasmaCloudHueShift += settings->speed / 5;
}

void SwirlIn(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    if (millis() - swirlInLastUpdate > 100 - settings->speed * 10)
    {
        swirlInLastUpdate = millis();
        swirlInAngle += 4;
    }

    constexpr float centerX = kMatrixWidth / 2.0f;
    constexpr float centerY = kMatrixHeight / 2.0f;
    const float maxDist = sqrt(centerX * centerX + centerY * centerY);

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            float dx = centerX - i;
            float dy = centerY - j;
            auto dist = static_cast<uint16_t>(sqrt(dx * dx + dy * dy));
            auto hue = static_cast<uint8_t>(map(dist, 0, static_cast<long>(maxDist), 0, 255) + swirlInAngle);
            CRGB color = ColorFromPalette(settings->palette, hue, 255, blendMode(settings));
            canvas.drawPixel(x + i, y + j, color);
        }
    }
}

void SwirlOut(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    if (millis() - swirlOutLastUpdate > 100 - settings->speed * 10)
    {
        swirlOutLastUpdate = millis();
        swirlOutAngle += 4;
    }

    constexpr float centerX = kMatrixWidth / 2.0f;
    constexpr float centerY = kMatrixHeight / 2.0f;
    const float maxDist = sqrt(centerX * centerX + centerY * centerY);

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            float dx = centerX - i;
            float dy = centerY - j;
            auto dist = static_cast<uint16_t>(sqrt(dx * dx + dy * dy));
            auto hue = static_cast<uint8_t>(255 - map(dist, 0, static_cast<long>(maxDist), 0, 255) + swirlOutAngle);
            CRGB color = ColorFromPalette(settings->palette, hue, 255, blendMode(settings));
            canvas.drawPixel(x + i, y + j, color);
        }
    }
}

// ---- Reset all wave effect state ----

void resetWaveEffectState()
{
    pacificaTime = 0;
    plasmaTime = 0;
    plasmaCloudTime = 0;
    plasmaCloudHueShift = 0;
    swirlInLastUpdate = 0;
    swirlInAngle = 0;
    swirlOutLastUpdate = 0;
    swirlOutAngle = 0;
}
