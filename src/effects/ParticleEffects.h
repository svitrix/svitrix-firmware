#pragma once

#include "EffectTypes.h"

void TwinklingStars(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void Fireworks(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void RippleEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void Matrix(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void Fire(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);

void resetParticleEffectState();
