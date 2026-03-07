/**
 * @file DisplayManager_Artnet.cpp
 * @brief Artnet/DMX input and moodlight mode.
 *
 * Receives Art-Net DMX frames over WiFi and renders them directly
 * to the LED matrix (256 pixels, 768 channels across 2 universes).
 * Universe 10 is reserved for global brightness control.
 * moodlight() fills the entire matrix with a solid color or kelvin
 * temperature value, operating independently from the app loop.
 */
#include "DisplayManager.h"
#include "DisplayManager_internal.h"
#include "Globals.h"
#include <ArtnetWifi.h>
#include <ArduinoJson.h>
#include "ColorUtils.h"
#include "Functions.h"

// Artnet-related globals (extern declarations in DisplayManager_internal.h)
unsigned long lastArtnetStatusTime = 0;
const int numberOfChannels = 256 * 3;
// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

uint8_t received_packets = 0;
bool universe1_complete = false;
bool universe2_complete = false;

/// Art-Net DMX frame callback — maps incoming channel data to LED pixels.
/// Universe 10 is reserved for global brightness control.
/// Waits until all universes are received before calling matrix->show().
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
    sendFrame = 1;
    // set brightness of the whole matrix
    if (universe == 10)
    {
        matrix->setBrightness(data[0]);
        matrix->show();
    }

    // Store which universe has got in
    if ((universe - startUniverse) < maxUniverses)
        universesReceived[universe - startUniverse] = 1;

    for (int i = 0; i < maxUniverses; i++)
    {
        if (universesReceived[i] == 0)
        {
            // Serial.println("Broke");
            sendFrame = 0;
            break;
        }
    }

    // read universe and put into the right part of the display buffer
    for (int i = 0; i < length / 3; i++)
    {
        int led = i + (universe - startUniverse) * (previousDataLength / 3);
        if (led < MATRIX_WIDTH * MATRIX_HEIGHT)
            matrix->drawPixel(led % matrix->width(), led / matrix->width(), matrix->Color(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));
    }
    previousDataLength = length;

    if (sendFrame)
    {
        matrix->show();
        // Reset universeReceived to 0
        memset(universesReceived, 0, maxUniverses);
    }
}

/// Initializes the Art-Net receiver and registers the DMX frame callback.
void DisplayManager_::startArtnet()
{
    artnet.begin();
    artnet.onDmx(onDmxFrame);
}

/// Fills the entire matrix with a solid color or kelvin temperature.
/// Empty string turns off moodlight mode. JSON: {"color":..., "kelvin":N, "brightness":N}.
/// While active, the normal app loop is paused (moodlightMode=true).
bool DisplayManager_::moodlight(const char *json)
{
    if (strcmp(json, "") == 0)
    {
        moodlightMode = false;
        return true;
    }

    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return false;

    int brightness = doc["brightness"] | brightnessConfig.brightness;
    matrix->setBrightness(brightness);

    if (doc.containsKey("kelvin"))
    {
        int kelvin = doc["kelvin"];
        CRGB color;
        color = kelvinToRGB(kelvin);
        color.nscale8(brightness);
        for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++)
        {
            leds[i] = color;
        }
    }
    else if (doc.containsKey("color"))
    {
        auto c = doc["color"];
        uint32_t color888 = getColorFromJsonVariant(c, colorConfig.textColor);
        drawFilledRect(0, 0, 32, 8, color888);
    }
    else
    {
        doc.clear();
        return true;
    }

    moodlightMode = true;
    doc.clear();
    matrix->show();
    return true;
}

CRGB *DisplayManager_::getLeds()
{
    return leds;
}
