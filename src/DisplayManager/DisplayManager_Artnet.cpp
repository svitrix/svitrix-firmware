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
#include "INotifier.h"
#include <ArtnetWifi.h>
#include <ArduinoJson.h>
#include "ColorUtils.h"
#include "Functions.h"
#include "Apps.h"
#include "EffectRegistry.h"

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

int8_t DisplayManager_::resolveNextApp(int8_t currentApp, int8_t direction)
{
    // Use rotation items if available, otherwise fall back to default behavior
    if (rotationItems.empty() || !ui || Apps.empty())
        return -1; // Use default sequential behavior

    // Save the current item as previous only if it was an app (not an effect).
    // This ensures outgoing app color works correctly when transitioning from effect to app.
    if (currentRotationItem && currentRotationItem->type == 0)
        prevRotationItem = currentRotationItem;

    // Try to find a valid item, but limit attempts to prevent infinite loop
    for (size_t attempts = 0; attempts < rotationItems.size(); attempts++)
    {
        // Advance rotation index
        if (direction >= 0)
            rotationIndex = (rotationIndex + 1) % rotationItems.size();
        else
            rotationIndex = (rotationIndex - 1 + rotationItems.size()) % rotationItems.size();

        auto& item = rotationItems[rotationIndex];
        currentRotationItem = &item;

        // Sync legacy aliases
        playlistIndex = rotationIndex;

        if (item.type == 0) // App
        {
            bool wasEffectOnly = rotationEffectOnly;
            rotationEffectOnly = false;
            playlistEffectOnly = false;
            ui->setBackgroundEffect(displayConfig.backgroundEffect);

            // Find app index
            for (size_t i = 0; i < Apps.size(); i++)
            {
                if (Apps[i].first == item.name)
                {
                    // Set duration for this rotation item
                    long dur = item.duration > 0 ? item.duration * 1000L : appConfig.timePerApp;
                    ui->setTimePerApp(dur);

                    // If coming from an effect, switch instantly (no transition animation)
                    // to avoid showing the stale "previous app" during transition
                    if (wasEffectOnly)
                    {
                        ui->switchToApp((uint8_t)i);
                        return -2; // Signal: already switched, don't start transition
                    }
                    return (int8_t)i;
                }
            }
            // App not found, continue to next item
        }
        else // Effect (standalone)
        {
            int effectIdx = getEffectIndex(item.name.c_str());
            if (effectIdx >= 0)
            {
                rotationEffectOnly = true;
                playlistEffectOnly = true;
                ui->setBackgroundEffect(effectIdx);
                // Set duration for effect display
                long dur = item.duration > 0 ? item.duration * 1000L : appConfig.timePerApp;
                ui->setTimePerApp(dur);
                // Report effect to HA (prefixed to distinguish from apps)
                setCurrentApp(String("Effect: ") + item.name);
                if (notifier_)
                    notifier_->setCurrentApp(getCurrentApp());
                // Return -2 to signal "don't change app, just reset timer"
                return -2;
            }
            // Effect not found, continue to next item
            rotationEffectOnly = false;
            playlistEffectOnly = false;
        }
    }

    // No valid items found, fall back to default behavior
    currentRotationItem = nullptr;
    return -1;
}
