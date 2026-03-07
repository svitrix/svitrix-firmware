#pragma once

#include <FastLED.h>
#include <ArduinoJson.h>

CRGBPalette16 loadPaletteFromLittleFS(const String& paletteName);
CRGBPalette16 getPalette(const JsonVariant& paletteVariant);
