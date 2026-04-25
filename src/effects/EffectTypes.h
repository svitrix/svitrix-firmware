#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <IPixelCanvas.h>

// Matrix dimensions used across all effects
constexpr uint16_t kMatrixWidth = 32;
constexpr uint16_t kMatrixHeight = 8;

struct EffectSettings
{
    double speed;
    CRGBPalette16 palette;
    bool blend;

    explicit EffectSettings(double s = 2, CRGBPalette16 p = OceanColors_p, bool b = false)
        : speed(s)
        , palette(p)
        , blend(b)
    {
    }
};

using EffectFunc = void (*)(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);

struct Effect
{
    String name;
    EffectFunc func;
    EffectSettings settings;
};

// Shared helper: get blend mode from settings
inline TBlendType blendMode(const EffectSettings *s)
{
    return s->blend ? LINEARBLEND : NOBLEND;
}
