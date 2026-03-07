#pragma once

#include "EffectTypes.h"

void SnakeGame(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void PingPongEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void BrickBreakerEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);
void LookingEyes(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings);

void resetGameEffectState();
