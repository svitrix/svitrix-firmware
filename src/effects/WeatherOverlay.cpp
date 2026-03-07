#include "WeatherOverlay.h"

namespace
{

struct WeatherState
{
    CRGB leds[kMatrixWidth][kMatrixHeight] = {};
    int colorChanges[kMatrixWidth][kMatrixHeight] = {};
    bool lightning = false;
    unsigned long lastLightningMs = 0;
    int lightningDuration = 50;
    int updateFrame = 0;
    int windFrame = 0;
};

WeatherState weather;

bool isRainType(OverlayEffect effect)
{
    return effect == RAIN || effect == STORM || effect == THUNDER || effect == DRIZZLE;
}

bool isWindy(OverlayEffect effect)
{
    return effect == STORM || effect == THUNDER;
}

} // namespace

void EffectOverlay(IPixelCanvas& canvas, int16_t x, int16_t y, OverlayEffect effect)
{
    auto& w = weather;

    // Lightning logic
    if (effect == THUNDER)
    {
        if (random8() < 1)
        {
            w.lightning = true;
            w.lastLightningMs = millis();
        }
        else if (w.lightning && (millis() - w.lastLightningMs > w.lightningDuration))
        {
            w.lightning = false;
        }
    }

    // Spawn particles
    if (isRainType(effect))
    {
        int chance = isWindy(effect) ? 60 : (effect == DRIZZLE ? 15 : 50);
        CRGB color = (effect == DRIZZLE) ? CRGB(CHSV(160, 255, 230)) : CRGB(CHSV(160, 255, 200));
        if (random8() < chance)
            w.leds[random8(kMatrixWidth)][0] = color;
    }
    else if (effect == SNOW && random8() < 20)
    {
        w.leds[random8(kMatrixWidth)][0] = CRGB(CHSV(0, 0, 255));
    }

    // Frost effect
    if (effect == FROST)
    {
        for (uint16_t i = 0; i < kMatrixWidth; i++)
        {
            for (uint16_t j = 0; j < kMatrixHeight; j++)
            {
                if (w.colorChanges[i][j] > 0)
                {
                    w.leds[i][j] = ColorFromPalette(OceanColors_p, random8(), 255, LINEARBLEND);
                    w.colorChanges[i][j]++;
                    if (w.colorChanges[i][j] > 6)
                        w.colorChanges[i][j] = 0;
                }
                if (w.colorChanges[i][j] == 0)
                    w.leds[i][j] = CRGB::Black;
            }
        }

        if (random8() < 25)
        {
            int ri = random8(kMatrixWidth);
            int rj = random8(kMatrixHeight);
            if (w.colorChanges[ri][rj] == 0)
            {
                w.colorChanges[ri][rj] = 1;
                w.leds[ri][rj] = ColorFromPalette(OceanColors_p, random8(), 255, LINEARBLEND);
            }
        }
    }

    // Gravity: shift particles down
    int frameThreshold = (effect == SNOW) ? 5 : 2;
    if (effect != NONE && effect != FROST && ++w.updateFrame >= frameThreshold)
    {
        for (uint16_t i = 0; i < kMatrixWidth; i++)
        {
            for (int j = kMatrixHeight - 1; j > 0; j--)
                w.leds[i][j] = w.leds[i][j - 1];
            w.leds[i][0] = CRGB::Black;
        }
        w.updateFrame = 0;
    }

    // Wind: shift particles sideways
    if (isWindy(effect) && ++w.windFrame >= 3)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            for (int i = kMatrixWidth - 1; i > 0; i--)
                w.leds[i][j] = w.leds[i - 1][j];
            w.leds[0][j] = CRGB::Black;
        }
        w.windFrame = 0;
    }

    // Draw
    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            if (w.lightning && effect == THUNDER)
                canvas.drawPixel(i + x, j + y, CHSV(0, 0, 255));
            else if (w.leds[i][j])
                canvas.drawPixel(i + x, j + y, w.leds[i][j]);
        }
    }
}
