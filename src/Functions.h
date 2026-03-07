#pragma once

#include <map>
#include <FastLED_NeoMatrix.h>
#include <Globals.h>
#include <ArduinoJson.h>

// Re-export services so all existing callers still see the same symbols
#include "ColorUtils.h"
#include "TextUtils.h"
#include "MathUtils.h"
#include "TimeEffects.h"

uint32_t getColorFromJsonVariant(JsonVariant colorVariant, uint32_t defaultColor);

// Original 2-param signature (delegates to 3-param services version with global)
float getTextWidth(const char *text, byte textCase);

// Original signatures (delegate to time-parameterized services versions)
uint32_t TextEffect(uint32_t color, uint32_t fade, uint32_t blink);

uint32_t fadeColor(uint32_t color, uint32_t interval);
