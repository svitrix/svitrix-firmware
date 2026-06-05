/**
 * @file DisplayManager.cpp
 * @brief Core display controller — setup, main loop, power, brightness, navigation.
 *
 * Defines the shared globals (leds[], matrix, ui) and implements the main
 * lifecycle: setup() initializes FastLED/NeoMatrix hardware, tick() drives
 * the frame loop dispatching to Artnet, moodlight, or the normal app UI.
 * Also handles button input, power on/off with sleep animation, matrix
 * layout switching, gamma correction, and the New Year easter egg.
 */
#include <DisplayManager.h>
#include <algorithm>
#include "DisplayManager_internal.h"
#include "INotifier.h"
#include "IPeripheryProvider.h"
#include "MatrixDisplayUi.h"
#include <TJpg_Decoder.h>
#include "Globals.h"
#include "GifPlayer.h"
#include "timer.h"
#include "Apps.h"
#include "EffectRegistry.h"
#include "Overlays.h"
#include "AlarmManager/AlarmManager.h"
#include <ArtnetWifi.h>

extern ArtnetWifi artnet;
#include "GammaUtils.h"
#include <ArduinoJson.h>

constexpr int MATRIX_PIN = 32;


CRGB leds[MATRIX_WIDTH * MATRIX_HEIGHT];
CRGB ledsCopy[MATRIX_WIDTH * MATRIX_HEIGHT];
float actualBri;
int16_t cursor_x, cursor_y;
uint32_t textColor;

// NeoMatrix — created in setup() via setMatrixLayout(), not at file scope.
FastLED_NeoMatrix *matrix = nullptr;
MatrixDisplayUi *ui = nullptr;

// Artnet/moodlight runtime state (declared extern in DisplayManager_internal.h)
bool artnetMode = false;
bool moodlightMode = false;

// Alarm indicator state tracking
static bool lastHasEnabledAlarms = false;
static bool lastAlarmRinging = false;

// Display calibration (declared extern in DisplayManager_internal.h)
float displayGamma = 0;
CRGB colorCorrection;
CRGB colorTemperature;

// Current app name (declared extern in DisplayManager_internal.h)
String currentApp;

// Rotation runtime state (declared extern in DisplayManager_internal.h)
std::vector<RotationItemRuntime> rotationItems;
int rotationIndex = -1;
bool rotationEffectOnly = false;
const RotationItemRuntime* currentRotationItem = nullptr;

// Legacy aliases for gradual migration
std::vector<PlaylistItemRuntime> playlistItems;
int playlistIndex = -1;
bool playlistEffectOnly = false;

// Forward declaration for getDurationForApp (defined below)
static long getDurationForApp(const String& appName);

/// Parses rotationConfig.items JSON into rotationItems vector.
/// Only includes enabled items. Called at boot and when rotation config changes.
void parseRotationConfig()
{
    rotationItems.clear();
    rotationIndex = -1;
    currentRotationItem = nullptr;

    if (rotationConfig.items.isEmpty())
    {
        rotationEffectOnly = false;
        if (ui)
            ui->setBackgroundEffect(displayConfig.backgroundEffect);
        return;
    }

    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, rotationConfig.items);
    if (err)
    {
        DEBUG_PRINTLN(F("parseRotationConfig: JSON error"));
        return;
    }

    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject item : arr)
    {
        bool enabled = item["enabled"] | true;
        if (!enabled)
            continue;  // Skip disabled items

        RotationItemRuntime rir;
        rir.id = item["id"].as<String>();
        const char* typeStr = item["type"] | "app";
        rir.type = (strcmp(typeStr, "effect") == 0) ? 1 : 0;
        rir.name = item["name"].as<String>();
        rir.enabled = true;
        rir.duration = item["duration"] | 0;
        rir.color = item["color"] | 0;
        rir.icon = item["icon"].as<String>();

        if (!rir.name.isEmpty())
            rotationItems.push_back(rir);
    }

    // Sync legacy playlistEffectOnly
    playlistEffectOnly = rotationEffectOnly;

    DEBUG_PRINTF("Rotation loaded: %d enabled items\n", rotationItems.size());
}

/// @deprecated Legacy wrapper - calls parseRotationConfig
static void parsePlaylistConfig()
{
    parseRotationConfig();
}

/// Advances display to the current rotation item (app or effect).
/// For apps: transitions to that app. For effects: sets background effect only.
static void advanceToRotationItem()
{
    if (rotationIndex < 0 || rotationIndex >= (int)rotationItems.size())
        return;

    auto& item = rotationItems[rotationIndex];
    currentRotationItem = &item;

    if (item.type == 0) // app
    {
        rotationEffectOnly = false;
        playlistEffectOnly = false;
        // Restore normal background (no effect override)
        ui->setBackgroundEffect(displayConfig.backgroundEffect);

        int idx = findAppIndexByName(item.name);
        if (idx >= 0)
        {
            ui->transitionToApp(idx);
        }
        else
        {
            DEBUG_PRINTF("Rotation: app '%s' not found\n", item.name.c_str());
            // Skip to next item
            rotationIndex = (rotationIndex + 1) % rotationItems.size();
            advanceToRotationItem();
            return;
        }
    }
    else // effect (standalone)
    {
        rotationEffectOnly = true;
        playlistEffectOnly = true;
        // Find effect index by name
        int effectIdx = getEffectIndex(item.name.c_str());
        if (effectIdx >= 0)
        {
            ui->setBackgroundEffect(effectIdx);
            DEBUG_PRINTF("Rotation: showing effect '%s' (%d)\n", item.name.c_str(), effectIdx);
        }
        else
        {
            DEBUG_PRINTF("Rotation: effect '%s' not found\n", item.name.c_str());
            rotationEffectOnly = false;
            playlistEffectOnly = false;
            // Skip to next item
            rotationIndex = (rotationIndex + 1) % rotationItems.size();
            advanceToRotationItem();
            return;
        }
    }

    // Set duration for this item
    long dur = item.duration > 0 ? item.duration * 1000L : 7000L; // 7s default for effects
    if (item.type == 0 && item.duration == 0)
        dur = getDurationForApp(item.name);
    ui->setTimePerApp(dur);
}

/// @deprecated Legacy wrapper
static void advanceToPlaylistItem()
{
    advanceToRotationItem();
}

/// Maps native app names to their configured per-app duration (in milliseconds).
/// Custom apps and unknown names fall back to the global timePerApp setting.
static long getDurationForApp(const String& appName)
{
    if (appName == "Time")
        return appConfig.timeDuration * 1000L;
    if (appName == "Date")
        return appConfig.dateDuration * 1000L;
    if (appName == "Temperature")
        return appConfig.tempDuration * 1000L;
    if (appName == "Humidity")
        return appConfig.humDuration * 1000L;
    if (appName == "Battery")
        return appConfig.batDuration * 1000L;
    if (appName == "OutdoorTemp")
        return weatherConfig.outdoorTempDuration * 1000L;
    if (appName == "OutdoorHum")
        return weatherConfig.outdoorHumDuration * 1000L;
    if (appName == "Pressure")
        return weatherConfig.pressureDuration * 1000L;
    if (appName == "AirQuality")
        return weatherConfig.aqiDuration * 1000L;
    if (appName == "UV")
        return weatherConfig.uvDuration * 1000L;
    return appConfig.timePerApp;
}

DisplayManager_& DisplayManager_::getInstance()
{
    static DisplayManager_ instance;
    return instance;
}

DisplayManager_& DisplayManager = DisplayManager.getInstance();

void DisplayManager_::setNotifier(INotifier *n)
{
    notifier_ = n;
    notifMgr_.setDependencies(n, periphery_);
}
void DisplayManager_::setPeriphery(IPeripheryProvider *p)
{
    periphery_ = p;
    notifMgr_.setDependencies(notifier_, p);
}
void DisplayManager_::setMenuActiveQuery(bool (*cb)())
{
    isMenuActive_ = cb;
}
void DisplayManager_::subscribeViaMqtt(const char *topic)
{
    if (notifier_)
        notifier_->subscribe(topic);
}

void DisplayManager_::registerPolicy(IDisplayPolicy *policy)
{
    if (policy)
        policies_.push_back(policy);
}

void DisplayManager_::markPolicyConfigDirty()
{
    policyDirty_ = true;
}

uint32_t DisplayManager_::resolveTextColor(uint32_t preferred) const
{
    // Use the cached active policy (computed in tick()) rather than
    // re-evaluating isActive() — resolveTextColor is on the hot per-app
    // render path called many times per frame.
    if (activePolicy_)
    {
        uint32_t override_;
        if (activePolicy_->overridesTextColor(override_))
            return override_;
    }
    return preferred;
}
void DisplayManager_::setColorCorrection(CRGB c)
{
    colorCorrection = c;
}
void DisplayManager_::setColorTemperature(CRGB t)
{
    colorTemperature = t;
}
void DisplayManager_::setCurrentApp(const String& name)
{
    currentApp = name;
}
const String& DisplayManager_::getCurrentApp() const
{
    return currentApp;
}

/// Sets matrix brightness, respecting matrixOff state and active policies.
/// When display is off, brightness stays 0 unless the front notification has wakeup=true.
/// When a registered policy is active and overrides brightness, the value is clamped.
void DisplayManager_::setBrightness(int bri)
{
    bool wakeup = false;
    if (!notifications.empty())
    {
        wakeup = notifications[0].wakeup;
    }

    if (displayConfig.matrixOff && !wakeup)
    {
        matrix->setBrightness(0);
    }
    else
    {
        if (activePolicy_)
        {
            uint8_t clamp;
            if (activePolicy_->overridesBrightness(clamp))
                bri = clamp;
        }
        matrix->setBrightness(bri);
        actualBri = bri;
    }
}

/// Enables or disables automatic app cycling.
/// Auto-transition is forced off when fewer than 2 apps are registered.
bool DisplayManager_::setAutoTransition(bool active)
{
    if (ui->AppCount < 2)
    {
        ui->disablesetAutoTransition();
        return false;
    }
    if (active)
    {
        ui->enablesetAutoTransition();
        return true;
    }
    else
    {
        ui->disablesetAutoTransition();
        return false;
    }
}

/// Applies all persisted settings to the display: FPS, timing, brightness, color, transition.
void DisplayManager_::applyAllSettings()
{
    if (!ui)
        return;
    ui->setTargetFPS(displayConfig.matrixFps);
    ui->setTimePerApp(appConfig.timePerApp);
    ui->setTimePerTransition(appConfig.timePerTransition);

    if (!brightnessConfig.autoBrightness)
        setBrightness(brightnessConfig.brightness);
    setTextColor(colorConfig.textColor);
    setAutoTransition(appConfig.autoTransition);

    // Parse playlist config - actual playlist rotation happens via resolveNextApp()
    // during auto-transitions, not here
    parsePlaylistConfig();
}

void DisplayManager_::setAppTime(long duration)
{
    ui->setTimePerApp(duration);
}

/// Initializes FastLED hardware, NeoMatrix, TJpg decoder, GIF player, and UI framework.
/// Must be called once from main setup() before any display operations.
void DisplayManager_::setup()
{
    TJpgDec.setCallback(jpg_output);
    TJpgDec.setJpgScale(1);
    random16_set_seed(millis());
    FastLED.addLeds<NEOPIXEL, MATRIX_PIN>(leds, MATRIX_WIDTH * MATRIX_HEIGHT);
    setMatrixLayout(displayConfig.matrixLayout);
    matrix->setRotation(displayConfig.rotateScreen ? 1 : 0);
    displayGamma = 1.9;
    if (colorCorrection)
    {
        FastLED.setCorrection(colorCorrection);
    }
    if (colorTemperature)
    {
        FastLED.setTemperature(colorTemperature);
    }
    ui->setAppAnimation(ANIM_SLIDE_DOWN);

    ui->setTargetFPS(displayConfig.matrixFps);
    ui->setTimePerApp(appConfig.timePerApp);
    ui->setTimePerTransition(appConfig.timePerTransition);
    ui->setOverlays(overlays, 3);
    ui->setBackgroundEffect(displayConfig.backgroundEffect);
    // Only apply config's auto-transition if no active policy blocks it.
    // Query policies directly (not the cached activePolicy_) to handle
    // calls from setNewSettings() before the next tick() updates the cache.
    bool policyBlocks = false;
    for (auto *p : policies_)
    {
        if (p->isActive() && p->blocksAutoTransition())
        {
            policyBlocks = true;
            break;
        }
    }
    setAutoTransition(policyBlocks ? false : appConfig.autoTransition);

    // Ensure clean playlist state on startup
    playlistEffectOnly = false;
    playlistIndex = -1;

    ui->init();
}

/// Main display loop — called every frame from the Arduino loop().
/// Dispatches to AP mode text, Artnet, moodlight, or the normal app UI framework.
/// Also polls for incoming Artnet packets and checks the New Year easter egg.
void DisplayManager_::tick()
{
    if (systemConfig.apMode)
    {
        HSVtext(2, 6, "AP MODE", true, 1);
    }
    else if (artnetMode)
    {
        // handled by the DMXFrame callback
    }
    else if (moodlightMode)
    {
        // handled by the moodlight function
    }
    else
    {
        // Edge-trigger: resolve which policy (if any) is active this tick and
        // only apply side-effectful setters when the active policy changes.
        // Per-tick application would churn UI state, clobber per-app colours,
        // and potentially spam downstream listeners (MQTT, logs).
        auto it = std::find_if(policies_.begin(), policies_.end(),
                               [](IDisplayPolicy *p)
                               { return p->isActive(); });
        IDisplayPolicy *newActive = (it != policies_.end()) ? *it : nullptr;

        // Re-apply when either the active policy identity changed, or a
        // policy-relevant config field was mutated since the last activation
        // (dirty flag set by markPolicyConfigDirty()). The dirty branch is
        // gated on a policy being involved on at least one side of the
        // transition — firing a re-apply when both sides are nullptr would
        // needlessly overwrite whatever the current app just rendered.
        const bool changed = (newActive != activePolicy_);
        const bool dirtyWithPolicy = policyDirty_ && (newActive || activePolicy_);
        policyDirty_ = false;
        if (changed || dirtyWithPolicy)
        {
            activePolicy_ = newActive;
            if (activePolicy_)
            {
                uint32_t col;
                if (activePolicy_->overridesTextColor(col))
                    setTextColor(col);
                if (activePolicy_->blocksAutoTransition())
                {
                    setAutoTransition(false);
                    int timeIdx = findAppIndexByName("Time");
                    if (timeIdx >= 0)
                        ui->switchToApp(timeIdx);
                }
            }
            else
            {
                setTextColor(colorConfig.textColor);
                setAutoTransition(appConfig.autoTransition);
            }
            // Reapply brightness so the clamp inside setBrightness() takes
            // effect (or is released) immediately on the transition edge.
            setBrightness(brightnessConfig.brightness);
        }
        ui->update();
        if (ui->getUiState()->appState == IN_TRANSITION && !appIsSwitching)
        {
            appIsSwitching = true;
        }
        else if (ui->getUiState()->appState == FIXED && appIsSwitching)
        {
            appIsSwitching = false;
            resetAllEffectState();
            if (notifier_)
                notifier_->setCurrentApp(currentApp);
            // Use playlist duration if enabled and valid
            if (playlistConfig.enabled && playlistIndex >= 0 && playlistIndex < (int)playlistItems.size())
            {
                auto& item = playlistItems[playlistIndex];
                long dur = item.duration > 0 ? item.duration * 1000L : getDurationForApp(currentApp);
                setAppTime(dur);
            }
            else
            {
                setAppTime(getDurationForApp(currentApp));
            }
            checkLifetime(ui->getnextAppNumber());
            ResetCustomApps();
        }
    }

    if (!systemConfig.apMode)
    {
        auto artnetStatus = artnet.read();
        if (artnetStatus != artnetlib::OpCode::None)
        {
            lastArtnetStatusTime = millis();
            artnetMode = true;
        }
        else if (millis() - lastArtnetStatusTime > 1000)
        {
            artnetMode = false;
        }
    }

    if (systemConfig.newyear)
        DisplayManager.checkNewYear();

    // Update indicator 3 for alarms: show when any alarm is enabled
    if (appConfig.showAlarms && AlarmManager.hasServices())
    {
        bool hasEnabledAlarms = false;
        for (const auto& alarm : AlarmManager.getAlarms())
        {
            if (alarm.enabled)
            {
                hasEnabledAlarms = true;
                break;
            }
        }
        bool isRinging = AlarmManager.isRinging();

        if (hasEnabledAlarms != lastHasEnabledAlarms || isRinging != lastAlarmRinging)
        {
            lastHasEnabledAlarms = hasEnabledAlarms;
            lastAlarmRinging = isRinging;
            ui->setIndicator3State(hasEnabledAlarms || isRinging);
            if (isRinging)
            {
                ui->setIndicator3Color(0xFF0000); // Red when ringing
                ui->setIndicator3Blink(300);      // Fast blink
                ui->setIndicator3Fade(0);
            }
            else if (hasEnabledAlarms)
            {
                ui->setIndicator3Color(0xFFAA00); // Amber when alarm set
                ui->setIndicator3Blink(0);        // Solid, no blink
                ui->setIndicator3Fade(0);
            }
        }
    }
}

bool newYearEventTriggered = false;

/// Easter egg: triggers a fireworks notification at midnight on January 1st.
void DisplayManager_::checkNewYear()
{
    struct tm *timeInfo = timer_localtime();
    if (!timeInfo)
        return;
    if (timeInfo->tm_mon == 0 && timeInfo->tm_mday == 1 && timeInfo->tm_hour == 0 && timeInfo->tm_min == 0 && timeInfo->tm_sec == 0)
    {
        if (!newYearEventTriggered)
        {
            int year = 1900 + timeInfo->tm_year;
            char message[300];
            snprintf(message, sizeof(message), "{\"stack\":false,\"text\":\"%d\",\"duration\":20,\"effect\":\"Fireworks\",\"rtttl\":\"Auld:d=4,o=6,b=125:a5,d.,8d,d,f#,e.,8d,e,8f#,8e,d.,8d,f#,a,2b.,b,a.,8f#,f#,d,e.,8d,e,8f#,8e,d.,8b5,b5,a5,2d,16p\"}", year);
            DisplayManager.generateNotification(0, message);
            newYearEventTriggered = true;
        }
    }
    else
    {
        newYearEventTriggered = false;
    }
}

void DisplayManager_::clearMatrix()
{
    renderer_.clear();
    renderer_.show();
}

void DisplayManager_::leftButton()
{
    if (isMenuActive_ && isMenuActive_())
        return;

    // Global: Snooze ringing alarm from any app
    if (AlarmManager.isRinging())
    {
        const Alarm *ringing = AlarmManager.getRingingAlarm();
        AlarmManager.snooze(ringing ? ringing->snoozeMinutes : 5);
        return;
    }

    ui->previousApp();
}

void DisplayManager_::rightButton()
{
    if (isMenuActive_ && isMenuActive_())
        return;

    // Global: Snooze ringing alarm from any app
    if (AlarmManager.isRinging())
    {
        const Alarm *ringing = AlarmManager.getRingingAlarm();
        AlarmManager.snooze(ringing ? ringing->snoozeMinutes : 5);
        return;
    }

    ui->nextApp();
}

void DisplayManager_::nextApp()
{
    if (isMenuActive_ && isMenuActive_())
        return;

    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Switching to next app"));

    // Playlist mode: advance in playlist sequence
    if (playlistConfig.enabled && !playlistItems.empty())
    {
        playlistIndex = (playlistIndex + 1) % playlistItems.size();
        advanceToPlaylistItem();
    }
    else
    {
        ui->nextApp();
    }
}

/// Immediately transitions to the current app (forces redraw) and publishes the app loop via MQTT.
void DisplayManager_::forceNextApp()
{
    ui->switchToApp(ui->getUiState()->currentApp);
    setAppTime(appConfig.timePerApp);
    if (notifier_)
        notifier_->publish("stats/loop", getAppsAsJson().c_str());
    if (notifier_)
        notifier_->setCurrentApp(getAppNameAtIndex(ui->getUiState()->currentApp));
}

void DisplayManager_::previousApp()
{
    if (isMenuActive_ && isMenuActive_())
        return;

    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Switching to previous app"));

    // Playlist mode: go back in playlist sequence
    if (playlistConfig.enabled && !playlistItems.empty())
    {
        playlistIndex = (playlistIndex - 1 + playlistItems.size()) % playlistItems.size();
        advanceToPlaylistItem();
    }
    else
    {
        ui->previousApp();
    }
}

void DisplayManager_::selectButton()
{
    if (isMenuActive_ && isMenuActive_())
        return;

    // Global: Dismiss ringing alarm from any app
    if (AlarmManager.isRinging())
    {
        AlarmManager.dismiss();
        return;
    }

    DisplayManager.dismissNotify();
}

void DisplayManager_::selectButtonLong()
{
}

/// Recreates the NeoMatrix and UI objects with a new pixel wiring layout.
/// @param layout 0=32x8 zigzag rows, 1=4x(8x8) progressive tiles, 2=32x8 zigzag columns.
void DisplayManager_::setMatrixLayout(int layout)
{
    // Delete ui FIRST — its destructor may reference the matrix pointer
    delete ui;
    ui = nullptr;
    delete matrix;
    matrix = nullptr;

    if (systemConfig.debugMode)
        DEBUG_PRINTF("Set matrix layout to %i", layout);
    switch (layout)
    {
    case 0:
        matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
        break;
    case 1:
        matrix = new FastLED_NeoMatrix(leds, 8, 8, 4, 1, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);
        break;
    case 2:
        matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
        break;
    default:
        matrix = new FastLED_NeoMatrix(leds, 8, 8, 4, 1, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);
        break;
    }

    ui = new MatrixDisplayUi(matrix, this);
}

/// Parses a power command from JSON ({"power": true/false}) or plain string ("true"/"1").
void DisplayManager_::powerStateParse(const char *json)
{
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        setPower((strcmp(json, "true") == 0 || strcmp(json, "1") == 0) ? true : false);
        return;
    }

    if (doc.containsKey("power"))
    {
        bool power = doc["power"].as<bool>();
        setPower(power);
    }
}

/// Plays a "Z" character bouncing animation before the display turns off.
/// Blocks the display loop for ~700ms total (14 frames × 50ms).
/// ESP32 delay() yields to FreeRTOS, so WiFi/MQTT tasks keep running.
void DisplayManager_::showSleepAnimation()
{
    setTextColor(0xFFFFFF);
    int steps[][2] = {{12, 8}, {13, 7}, {14, 6}, {15, 5}, {14, 4}, {13, 3}, {12, 2}, {13, 1}, {14, 0}, {15, -1}, {14, -2}, {13, -3}, {12, -4}, {13, -5}};
    int numSteps = sizeof(steps) / sizeof(steps[0]);
    for (int i = 0; i < numSteps; i++)
    {
        clear();
        printText(steps[i][0], steps[i][1], "Z", false, 1);
        show();
        delay(50);
    }
}

/// Turns the display on (restore brightness) or off (sleep animation + brightness 0).
void DisplayManager_::setPower(bool state)
{
    if (state)
    {
        displayConfig.matrixOff = false;
        setBrightness(brightnessConfig.brightness);
    }
    else
    {
        displayConfig.matrixOff = true;
        showSleepAnimation();
        setBrightness(0);
    }
}

/// Applies gamma correction to all LEDs based on current brightness.
/// Saves original colors to ledsCopy[] before correction.
/// Also mirrors the display horizontally if mirrorDisplay is enabled.
/// At very low brightness (1-5), applies additional dimming for better low-light control.
void DisplayManager_::gammaCorrection()
{
    float gamma = calculateGamma(actualBri);
    memcpy(ledsCopy, leds, sizeof(leds));
    for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++)
    {
        leds[i] = applyGamma_video(leds[i], gamma);
    }
    // Apply extra dimming at very low brightness for better low-light control
    if (actualBri <= 5)
    {
        uint8_t scale = map(actualBri, 1, 5, 32, 192);
        for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++)
        {
            leds[i].nscale8(scale);
        }
    }

    if (displayConfig.mirrorDisplay)
    {
        for (int y = 0; y < MATRIX_HEIGHT; y++)
        {
            for (int x = 0; x < MATRIX_WIDTH / 2; x++)
            {
                int index1 = y * MATRIX_WIDTH + x;
                int index2 = y * MATRIX_WIDTH + (MATRIX_WIDTH - x - 1);
                std::swap(leds[index1], leds[index2]);
            }
        }
    }
}
