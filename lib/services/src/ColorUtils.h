#pragma once

/**
 * @file ColorUtils.h
 * @brief Color conversion and interpolation utilities.
 *
 * All functions are pure — no global state, no hardware access.
 * Colors are represented as either:
 *   - uint32_t packed RGB: 0xRRGGBB (red in bits 23-16, green 15-8, blue 7-0)
 *   - CRGB struct from FastLED (fields: .r, .g, .b)
 *
 * Extracted from: Functions.cpp (hex, kelvin, hsv), DisplayManager.cpp (interpolate, CRGBtoHex).
 * Used by: DisplayManager, Apps, MQTTManager (via Functions.h re-export).
 * Tests: test/test_native/test_color/, test/test_native/test_crgb_hex/
 */

#include <cstdint>

#ifdef UNIT_TEST
#include "FastLED.h"
#include "Arduino.h"
#else
#include <FastLED.h>
#include <Arduino.h>
#endif

/**
 * Parse a hex color string (without '#') into a packed RGB uint32.
 *
 * @param hexString  6-char hex string, e.g. "FF00AA". Case-insensitive.
 * @return Packed 0xRRGGBB value. Example: "FF0000" → 0xFF0000
 */
uint32_t hexToUint32(const char *hexString);

/**
 * Convert a color temperature in Kelvin to an RGB color.
 *
 * Uses Tanner Helland's algorithm for blackbody radiation approximation.
 *   - 1000K → warm orange (candle)
 *   - 2700K → incandescent bulb
 *   - 6500K → daylight (near white)
 *   - 10000K → cool blue-white
 *
 * @param kelvin Color temperature in Kelvin (typical range 1000–10000).
 * @return CRGB with the approximated color. All channels clamped to [0..255].
 */
CRGB kelvinToRGB(int kelvin);

/**
 * Convert HSV (hue/saturation/value) to packed RGB uint32.
 *
 * Uses FastLED's hsv2rgb_spectrum() internally.
 *
 * @param h Hue (0-255, where 0=red, 85=green, 170=blue).
 * @param s Saturation (0=grey, 255=full color).
 * @param v Value/brightness (0=black, 255=full brightness).
 * @return Packed 0xRRGGBB value.
 */
uint32_t hsvToRgb(uint8_t h, uint8_t s, uint8_t v);

/**
 * Linear interpolation between two packed RGB colors.
 *
 * Each channel (R, G, B) is interpolated independently.
 * t is clamped: t<=0 returns color1, t>=1 returns color2.
 *
 * @param color1 Start color (packed 0xRRGGBB).
 * @param color2 End color (packed 0xRRGGBB).
 * @param t      Interpolation factor [0.0 .. 1.0].
 * @return Interpolated color as packed 0xRRGGBB.
 */
uint32_t interpolateColor(uint32_t color1, uint32_t color2, float t);

/**
 * Convert a CRGB color to a hex string with '#' prefix.
 *
 * @param color CRGB struct with .r, .g, .b fields.
 * @return String like "#FF00AA". Always uppercase, 7 chars.
 */
String CRGBtoHex(CRGB color);

