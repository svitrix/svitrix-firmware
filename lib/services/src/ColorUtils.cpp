#include "ColorUtils.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>

// Parse hex string to uint32 via strtol (base 16).
// Accepts uppercase and lowercase. No '#' prefix expected.
uint32_t hexToUint32(const char *hexString)
{
    return (uint32_t)strtol(hexString, NULL, 16);
}

// Tanner Helland's blackbody radiation algorithm.
// Attempt to approximate the RGB color for a given Kelvin temperature.
// Reference: https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
//
// Below 6600K: red=255, green rises logarithmically, blue rises logarithmically.
// Above 6600K: red drops as power function, green drops as power function, blue=255.
CRGB kelvinToRGB(int kelvin)
{
    // Guard: kelvin <= 0 causes log(0) = -Inf or log(negative) = NaN.
    // Values below 100K are physically meaningless; clamp to safe minimum.
    if (kelvin < 100)
        kelvin = 100;
    float temperature = kelvin / 100.0;
    float red, green, blue;

    if (temperature <= 66)
    {
        red = 255;
        green = 99.4708025861 * log(temperature) - 161.1195681661;
    }
    else
    {
        red = 329.698727446 * pow(temperature - 60, -0.1332047592);
        green = 288.1221695283 * pow(temperature - 60, -0.0755148492);
    }

    if (temperature >= 66)
        blue = 255;
    else if (temperature <= 19)
        blue = 0;
    else
        blue = 138.5177312231 * log(temperature - 10) - 305.0447927307;

    red = constrain(red, 0, 255);
    green = constrain(green, 0, 255);
    blue = constrain(blue, 0, 255);

    return CRGB((uint8_t)red, (uint8_t)green, (uint8_t)blue);
}

// Delegates to FastLED's hsv2rgb_spectrum(), then packs into uint32.
uint32_t hsvToRgb(uint8_t h, uint8_t s, uint8_t v)
{
    CHSV hsv(h, s, v);
    CRGB rgb;
    hsv2rgb_spectrum(hsv, rgb);
    return ((uint32_t)rgb.r << 16) |
           ((uint32_t)rgb.g << 8) |
           (uint32_t)rgb.b;
}

// Per-channel linear interpolation: result[ch] = c1[ch] + (c2[ch] - c1[ch]) * t.
// t is clamped to [0..1].
uint32_t interpolateColor(uint32_t color1, uint32_t color2, float t)
{
    if (t <= 0.0f)
        return color1;
    if (t >= 1.0f)
        return color2;

    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;

    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;

    uint8_t r = r1 + (r2 - r1) * t;
    uint8_t g = g1 + (g2 - g1) * t;
    uint8_t b = b1 + (b2 - b1) * t;

    return (r << 16) | (g << 8) | b;
}

// Format: "#RRGGBB" (7 chars). Always uppercase hex digits.
String CRGBtoHex(CRGB color)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "#%02X%02X%02X", color.r, color.g, color.b);
    return String(buf);
}
