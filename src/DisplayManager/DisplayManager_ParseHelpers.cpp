/**
 * @file DisplayManager_ParseHelpers.cpp
 * @brief Shared JSON parsing helpers for CustomApps and Notifications.
 *
 * Extracts common parsing logic that was duplicated between
 * generateCustomPage() and generateNotification(): progress bar colors,
 * bar/line chart data with autoscale, gradient colors, text/fragments,
 * CRGB color parsing, color field reading, and base64 icon decoding.
 * Also provides indicator dispatch helpers to eliminate repetitive switch/case.
 */
#include "DisplayManager.h"
#include "DisplayManager_internal.h"
#include "Globals.h"
#include <ArduinoJson.h>
#include "EffectRegistry.h"
#include "ColorUtils.h"
#include "Functions.h"
#include "LayoutEngine.h"

// Forward declarations
bool parseFragmentsText(const JsonArray& fragmentArray, std::vector<uint32_t>& colors, std::vector<String>& fragments, uint32_t standardColor); // DisplayManager_CustomApps.cpp
unsigned int decode_base64(const unsigned char *source, unsigned char *target);                                                                 // base64 library

// ── Generic JSON field helpers ────────────────────────────────────

/// Parse a hex string or [r,g,b] array from JSON into a CRGB object.
/// @return true if the key existed and was parsed.
bool parseCRGBFromJson(JsonObject doc, const char *key, CRGB& target)
{
    if (!doc.containsKey(key))
        return false;
    auto colorValue = doc[key];
    if (colorValue.is<String>())
    {
        uint32_t rgb = strtoul(colorValue.as<String>().c_str(), NULL, 16);
        target.setRGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    }
    else if (colorValue.is<JsonArray>() && colorValue.size() == 3)
    {
        target.setRGB(static_cast<uint8_t>(colorValue[0].as<int>()), static_cast<uint8_t>(colorValue[1].as<int>()), static_cast<uint8_t>(colorValue[2].as<int>()));
    }
    return true;
}

/// Read a color field from JSON into target. No-op if key is absent.
void readColorField(JsonObject doc, const char *key, uint32_t& target, uint32_t defaultColor)
{
    if (doc.containsKey(key))
        target = getColorFromJsonVariant(doc[key], defaultColor);
}

/// Decode a base64-encoded icon string into a JPEG data buffer.
void decodeBase64Icon(const String& data, std::vector<uint8_t>& buffer)
{
    unsigned int estimatedSize = (data.length() * 3) / 4;
    buffer.resize(estimatedSize);
    unsigned int decoded = decode_base64(reinterpret_cast<const unsigned char *>(data.c_str()), buffer.data());
    buffer.resize(decoded);
}

// ── App/notification field parsers ────────────────────────────────

/// Extracts progress bar fields from JSON: "progress" (0-100), "progressC" (bar color),
/// "progressBC" (background color). Defaults: progress=-1 (hidden), green bar, white bg.
void parseProgressBar(JsonObject doc, int& progress, uint32_t& pColor, uint32_t& pbColor)
{
    progress = doc.containsKey("progress") ? doc["progress"].as<int>() : -1;
    pColor = doc.containsKey("progressC") ? getColorFromJsonVariant(doc["progressC"], 0x00FF00) : 0x00FF00;
    pbColor = doc.containsKey("progressBC") ? getColorFromJsonVariant(doc["progressBC"], 0xFFFFFF) : 0xFFFFFF;
}

/// Extracts bar/line chart data arrays from JSON keys "bar" and "line" (max 16 points each).
/// Applies autoscale (default on) to normalize values to 0-8 range for the 8px display height.
/// Also reads "barBC" for bar chart background color.
// cppcheck-suppress constParameterReference
void parseChartData(JsonObject doc, int *barData, int& barSize, int *lineData, int& lineSize, uint32_t& barBG)
{
    bool autoscale = doc.containsKey("autoscale") ? doc["autoscale"].as<bool>() : true;

    const char *dataKeys[] = {"bar", "line"};
    int *dataArrays[] = {barData, lineData};
    int *dataSizeArrays[] = {&barSize, &lineSize};

    for (int i = 0; i < 2; i++)
    {
        const char *key = dataKeys[i];
        // cppcheck-suppress variableScope
        int *dataArray = dataArrays[i];
        int *dataSize = dataSizeArrays[i];

        if (doc.containsKey(key))
        {
            if (doc.containsKey("barBC"))
            {
                barBG = getColorFromJsonVariant(doc["barBC"], 0);
            }
            else
            {
                barBG = 0;
            }

            JsonArray data = doc[key];
            int index = 0;
            int maximum = 0;
            for (JsonVariant v : data)
            {
                if (index >= 16)
                    break;
                int d = v.as<int>();
                if (d > maximum)
                    maximum = d;
                dataArray[index] = d;
                index++;
            }
            *dataSize = index;

            if (autoscale && maximum > 0)
            {
                for (int j = 0; j < *dataSize; j++)
                {
                    dataArray[j] = map(dataArray[j], 0, maximum, 0, 8);
                }
            }
        }
        else
        {
            *dataSize = 0;
        }
    }
}

/// Extracts gradient colors from JSON "gradient" array [color1, color2].
/// Sets both to -1 if not present (indicating no gradient).
void parseGradient(JsonObject doc, int gradient[2])
{
    gradient[0] = -1;
    gradient[1] = -1;

    if (doc.containsKey("gradient"))
    {
        JsonArray arr = doc["gradient"].as<JsonArray>();
        if (arr.size() == 2)
        {
            gradient[0] = getColorFromJsonVariant(arr[0], colorConfig.textColor);
            gradient[1] = getColorFromJsonVariant(arr[1], colorConfig.textColor);
        }
    }
}

/// Parses the "text" field which can be either a plain string or an array of colored fragments.
/// Array form: [{"t":"hello","c":"#FF0000"}, {"t":" world"}] — each fragment has text and optional color.
/// Plain form: "hello world" — stored as-is in text, fragments/colors left empty.
void parseTextOrFragments(JsonObject doc, String& text, std::vector<uint32_t>& colors, std::vector<String>& fragments, uint32_t defaultColor)
{
    colors.clear();
    fragments.clear();

    if (doc.containsKey("text") && doc["text"].is<JsonArray>())
    {
        JsonArray textArray = doc["text"].as<JsonArray>();
        parseFragmentsText(textArray, colors, fragments, defaultColor);
    }
    else if (doc.containsKey("text"))
    {
        text = doc["text"].as<String>();
    }
    else
    {
        text = "";
    }
}

/// Extracts common app/notification display fields from JSON: effect, overlay, rainbow,
/// pushIcon, textCase, offsets, scrollSpeed, topText, fade, blink, center, noScroll,
/// repeat, and draw instructions. Used by both generateCustomPage and generateNotification.
void parseCommonAppFields(JsonObject doc, int& effect, OverlayEffect& overlay,
                          bool& rainbow, byte& pushIcon, byte& textCase,
                          int& iconOffset, int& textOffset, float& scrollSpeed,
                          bool& topText, int& fade, int& blink, bool& center,
                          bool& noScrolling, int16_t& repeat, String& drawInstructions,
                          IconLayout& layout)
{
    drawInstructions = doc.containsKey("draw") ? doc["draw"].as<String>() : "";

    if (doc.containsKey("effect"))
    {
        effect = getEffectIndex(doc["effect"].as<String>());
        if (doc.containsKey("effectSettings"))
        {
            updateEffectSettings(effect, doc["effectSettings"].as<String>());
        }
    }

    overlay = doc.containsKey("overlay") ? overlayFromString(doc["overlay"].as<String>()) : NONE;
    rainbow = doc.containsKey("rainbow") ? doc["rainbow"].as<bool>() : false;
    pushIcon = doc.containsKey("pushIcon") ? doc["pushIcon"] : 0;
    textCase = doc.containsKey("textCase") ? doc["textCase"] : 0;
    iconOffset = doc.containsKey("iconOffset") ? doc["iconOffset"] : 0;
    textOffset = doc.containsKey("textOffset") ? doc["textOffset"] : 0;
    scrollSpeed = doc.containsKey("scrollSpeed") ? doc["scrollSpeed"].as<float>() : -1;
    topText = doc.containsKey("topText") ? doc["topText"].as<bool>() : false;
    fade = doc.containsKey("fadeText") ? doc["fadeText"].as<int>() : 0;
    blink = doc.containsKey("blinkText") ? doc["blinkText"].as<int>() : 0;
    center = doc.containsKey("center") ? doc["center"].as<bool>() : true;
    noScrolling = doc.containsKey("noScroll") ? doc["noScroll"] : false;

    repeat = doc.containsKey("repeat") ? doc["repeat"].as<int>() : -1;
    if (noScrolling)
    {
        repeat = -1;
    }

    layout = doc.containsKey("layout") ? layoutFromString(doc["layout"].as<String>()) : IconLayout::Left;
}

/// Returns the "color" field from JSON, or the global text color if not present.
uint32_t parseAppColor(JsonObject doc)
{
    if (doc.containsKey("color"))
    {
        return getColorFromJsonVariant(doc["color"], colorConfig.textColor);
    }
    return colorConfig.textColor;
}

/// Returns the "background" color from JSON, or 0 (transparent) if not present.
uint32_t parseBackground(JsonObject doc)
{
    if (doc.containsKey("background"))
    {
        return getColorFromJsonVariant(doc["background"], 0);
    }
    return 0;
}

// --- Indicator dispatch helpers ---
// These helpers route calls to the correct indicator (1-3) on the UI object,
// eliminating repetitive switch/case blocks in indicatorParser().

/// Sets the on/off state for the given indicator (1-3).
void setIndicatorState(uint8_t indicator, bool state)
{
    switch (indicator)
    {
    case 1:
        ui->setIndicator1State(state);
        break;
    case 2:
        ui->setIndicator2State(state);
        break;
    case 3:
        ui->setIndicator3State(state);
        break;
    }
}

void setIndicatorColor(uint8_t indicator, uint32_t color)
{
    switch (indicator)
    {
    case 1:
        ui->setIndicator1Color(color);
        break;
    case 2:
        ui->setIndicator2Color(color);
        break;
    case 3:
        ui->setIndicator3Color(color);
        break;
    }
}

void setIndicatorBlink(uint8_t indicator, int blink)
{
    switch (indicator)
    {
    case 1:
        ui->setIndicator1Blink(blink);
        break;
    case 2:
        ui->setIndicator2Blink(blink);
        break;
    case 3:
        ui->setIndicator3Blink(blink);
        break;
    }
}

void setIndicatorFade(uint8_t indicator, int fade)
{
    switch (indicator)
    {
    case 1:
        ui->setIndicator1Fade(fade);
        break;
    case 2:
        ui->setIndicator2Fade(fade);
        break;
    case 3:
        ui->setIndicator3Fade(fade);
        break;
    }
}

bool getIndicatorState(uint8_t indicator)
{
    switch (indicator)
    {
    case 1:
        return ui->indicator1State;
    case 2:
        return ui->indicator2State;
    case 3:
        return ui->indicator3State;
    default:
        return false;
    }
}

uint32_t getIndicatorColor(uint8_t indicator)
{
    switch (indicator)
    {
    case 1:
        return ui->indicator1Color;
    case 2:
        return ui->indicator2Color;
    case 3:
        return ui->indicator3Color;
    default:
        return 0;
    }
}
