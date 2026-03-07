#pragma once

#include "EffectTypes.h"

void TheaterChase(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void CheckerboardEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void Fade(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void MovingLine(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void RadarEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);

void resetPatternEffectState();
