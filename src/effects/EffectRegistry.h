#pragma once

#include "EffectTypes.h"

constexpr int kNumEffects = 19;

extern Effect effects[];

void callEffect(IPixelCanvas& canvas, int16_t x, int16_t y, uint8_t index);
int getEffectIndex(const String& name);
void updateEffectSettings(uint8_t index, const String& json);
void resetAllEffectState();
