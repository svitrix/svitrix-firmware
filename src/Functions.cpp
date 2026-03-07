#include "Functions.h"
#include "ColorUtils.h"
#include "TextUtils.h"
#include "MathUtils.h"
#include "TimeEffects.h"

// getColorFromJsonVariant stays here — depends on ArduinoJson which is not mocked
uint32_t getColorFromJsonVariant(JsonVariant colorVariant, uint32_t defaultColor)
{
    if (colorVariant.is<String>())
    {
        String hexString = colorVariant.as<String>();
        hexString.replace("#", "");
        return hexToUint32(hexString.c_str());
    }
    else if (colorVariant.is<JsonArray>())
    {
        JsonArray colorArray = colorVariant.as<JsonArray>();
        if (colorArray.size() == 3) // RGB
        {
            uint8_t r = colorArray[0];
            uint8_t g = colorArray[1];
            uint8_t b = colorArray[2];
            return (r << 16) | (g << 8) | b;
        }
        else if (colorArray.size() == 4 && colorArray[0] == "HSV") // HSV
        {
            uint8_t h = colorArray[1];
            uint8_t s = colorArray[2];
            uint8_t v = colorArray[3];
            uint8_t r = 0, g = 0, b = 0;
            return (r << 16) | (g << 8) | b;
        }
    }

    return defaultColor;
}

// Wrapper: old 2-param signature delegates to services 3-param version
float getTextWidth(const char *text, byte textCase)
{
    return getTextWidth(text, textCase, displayConfig.uppercaseLetters);
}

// Wrappers: inject millis() into time-parameterized services versions
uint32_t fadeColor(uint32_t color, uint32_t interval)
{
    return fadeColorAt(color, interval, millis());
}

uint32_t TextEffect(uint32_t color, uint32_t fade, uint32_t blink)
{
    return textEffectAt(color, fade, blink, millis());
}
