#pragma once

#include "EffectTypes.h"

void Pacifica(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void Plasma(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void ColorWaves(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void PlasmaCloudEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void SwirlIn(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void SwirlOut(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);

void resetWaveEffectState();
