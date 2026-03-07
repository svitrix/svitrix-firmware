/**
 * @file DisplayManager.cpp
 * @brief Core display controller — setup, main loop, power, brightness, navigation.
 *
 * Defines the shared globals (leds[], matrix, ui) and implements the main
 * lifecycle: setup() initializes FastLED/NeoMatrix hardware, tick() drives
 * the frame loop dispatching to games, Artnet, or the normal app UI.
 * Also handles button input, power on/off with sleep animation, matrix
 * layout switching, gamma correction, and the New Year easter egg.
 */
#include <DisplayManager.h>
#include "DisplayManager_internal.h"
#include "INotifier.h"
#include "IPeripheryProvider.h"
#include "MatrixDisplayUi.h"
#include <TJpg_Decoder.h>
#include "Globals.h"
#include "GifPlayer.h"
#include "timer.h"
#include "Apps.h"
#include "effects.h"
#include "Overlays.h"
#include <ArtnetWifi.h>
#ifdef ENABLE_GAMES
#include "Games/GameManager.h"
#endif

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

// Display calibration (declared extern in DisplayManager_internal.h)
float displayGamma = 0;
CRGB colorCorrection;
CRGB colorTemperature;

// Current app name (declared extern in DisplayManager_internal.h)
String currentApp;

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

/// Sets matrix brightness, respecting matrixOff state.
/// When display is off, brightness stays 0 unless the front notification has wakeup=true.
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
    if (active && appConfig.autoTransition)
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
    ui->setAppAnimation(SLIDE_DOWN);

    ui->setTargetFPS(displayConfig.matrixFps);
    ui->setTimePerApp(appConfig.timePerApp);
    ui->setTimePerTransition(appConfig.timePerTransition);
    ui->setOverlays(overlays, 3);
    ui->setBackgroundEffect(displayConfig.backgroundEffect);
    setAutoTransition(appConfig.autoTransition);
    ui->init();
}

/// Main display loop — called every frame from the Arduino loop().
/// Dispatches to game engine, AP mode text, or the normal app UI framework.
/// Also polls for incoming Artnet packets and checks the New Year easter egg.
void DisplayManager_::tick()
{
#ifdef ENABLE_GAMES
    if (GameManager.isActive())
    {
        GameManager.tick();
        matrix->show();
        memcpy(ledsCopy, leds, sizeof(leds));
    }
    else
#endif
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
            setAppTime(appConfig.timePerApp);
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
    if (!(isMenuActive_ && isMenuActive_()))
        ui->previousApp();
}

void DisplayManager_::rightButton()
{
    if (!(isMenuActive_ && isMenuActive_()))
        ui->nextApp();
}

void DisplayManager_::nextApp()
{
    if (!(isMenuActive_ && isMenuActive_()))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Switching to next app"));
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
    if (!(isMenuActive_ && isMenuActive_()))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Switching to previous app"));
        ui->previousApp();
    }
}

void DisplayManager_::selectButton()
{
    if (!(isMenuActive_ && isMenuActive_()))
    {
        DisplayManager.dismissNotify();
    }
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
void DisplayManager_::gammaCorrection()
{
    float gamma = calculateGamma(actualBri);
    memcpy(ledsCopy, leds, sizeof(leds));
    for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++)
    {
        leds[i] = applyGamma_video(leds[i], gamma);
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
