/**
 * @file DisplayManager_Settings.cpp
 * @brief Settings get/set, device stats, and display state serialization.
 *
 * getSettings() and setNewSettings() handle the JSON settings API —
 * reading and writing all display/color/time/app configuration.
 * getStats() builds the device status JSON (battery, sensors, uptime,
 * WiFi signal, RAM). Also provides LED state export (ledsAsJson),
 * effect/transition name lists, and pixel color access.
 */
#include "DisplayManager.h"
#include "DisplayManager_internal.h"
#include "Globals.h"
#include "INotifier.h"
#include "IPeripheryProvider.h"
#include <ArduinoJson.h>
#include "EffectRegistry.h"
#include "Overlays.h"
#include "ColorUtils.h"
#include "StatsBuilder.h"
#include "Functions.h"
#include <WiFi.h>
#include "Dictionary.h"
#include "LayoutEngine.h"

/// Builds and returns the device status JSON: battery, sensors, uptime, WiFi RSSI,
/// free RAM, brightness, indicator states, current app, firmware version, and IP address.
String DisplayManager_::getStats()
{
    StatsData data;
    data.batteryPercent = batteryConfig.percent;
    data.batteryRaw = batteryConfig.raw;
    data.lux = static_cast<int>(sensorConfig.currentLux);
    data.ldrRaw = sensorConfig.ldrRaw;
    data.freeRam = ESP.getFreeHeap() + ESP.getFreePsram();
    data.brightness = brightnessConfig.brightness;
    data.hasSensor = sensorConfig.sensorReading;
    if (sensorConfig.sensorReading)
    {
        data.temperature = roundToDecimalPlaces(sensorConfig.currentTemp, timeConfig.tempDecimalPlaces);
        data.tempDecimalPlaces = timeConfig.tempDecimalPlaces;
        data.humidity = static_cast<uint8_t>(sensorConfig.currentHum);
    }
    data.uptime = periphery_ ? periphery_->readUptime() : 0;
    data.wifiSignal = WiFi.RSSI();
    data.receivedMessages = notifier_ ? notifier_->getReceivedMessages() : 0;
    data.version = VERSION;
    data.indicator1 = ui->indicator1State;
    data.indicator2 = ui->indicator2State;
    data.indicator3 = ui->indicator3State;
    data.currentApp = currentApp;
    data.uid = systemConfig.deviceId;
    data.matrixOn = !displayConfig.matrixOff;
    data.ipAddress = WiFi.localIP().toString();
    return buildStatsJson(data);
}

/// Serializes all current display/app/color/time settings to a JSON string.
/// Used by the HTTP API and MQTT to expose current configuration.
String DisplayManager_::getSettings()
{
    StaticJsonDocument<2048> doc;
    doc["MATP"] = !displayConfig.matrixOff;
    doc["ABRI"] = brightnessConfig.autoBrightness;
    doc["BRI"] = brightnessConfig.brightness;
    doc["ATRANS"] = appConfig.autoTransition;
    doc["TCOL"] = colorConfig.textColor;
    doc["TEFF"] = appConfig.transEffect;
    doc["TSPEED"] = appConfig.timePerTransition;
    doc["ATIME"] = appConfig.timePerApp / 1000;
    doc["TMODE"] = timeConfig.timeMode;
    doc["CHCOL"] = colorConfig.calendarHeaderColor;
    doc["CTCOL"] = colorConfig.calendarTextColor;
    doc["CBCOL"] = colorConfig.calendarBodyColor;
    doc["TFORMAT"] = timeConfig.timeFormat;
    doc["DFORMAT"] = timeConfig.dateFormat;
    doc["SOM"] = timeConfig.startOnMonday;
    doc["CEL"] = timeConfig.isCelsius;
    doc["BLOCKN"] = appConfig.blockNavigation;
    doc["MAT"] = displayConfig.matrixLayout;
    doc["SOUND"] = audioConfig.soundActive;
    doc["GAMMA"] = displayGamma;
    doc["UPPERCASE"] = displayConfig.uppercaseLetters;
    doc["CCORRECTION"] = CRGBtoHex(colorCorrection);
    doc["CTEMP"] = CRGBtoHex(colorTemperature);
    doc["WD"] = appConfig.showWeekday;
    doc["WDCA"] = colorConfig.wdcActive;
    doc["WDCI"] = colorConfig.wdcInactive;
    doc["TIME_COL"] = colorConfig.timeColor;
    doc["DATE_COL"] = colorConfig.dateColor;
    doc["HUM_COL"] = colorConfig.humColor;
    doc["TEMP_COL"] = colorConfig.tempColor;
    doc["BAT_COL"] = colorConfig.batColor;
    doc["SSPEED"] = appConfig.scrollSpeed;
    doc["NILAYOUT"] = layoutToString(appConfig.nativeIconLayout);
    doc["NMODE"] = appConfig.nightMode;
    doc["NSTART"] = appConfig.nightStart;
    doc["NEND"] = appConfig.nightEnd;
    doc["NBRI"] = appConfig.nightBrightness;
    doc["NCOL"] = appConfig.nightColor;
    doc["TIM"] = appConfig.showTime;
    doc["DAT"] = appConfig.showDate;
    doc["HUM"] = appConfig.showHum;
    doc["TEMP"] = appConfig.showTemp;
    doc["BAT"] = appConfig.showBat;
    doc["VOL"] = audioConfig.soundVolume;
    doc["OVERLAY"] = overlayToString(ui->getGlobalOverlay());
    doc["BEFF"] = displayConfig.backgroundEffect;
    String jsonString;
    return serializeJson(doc, jsonString), jsonString;
}

/// Applies a partial settings update from JSON. Only keys present in the payload are changed.
/// Handles display, brightness, color, time format, app visibility, audio, gamma,
/// color correction/temperature, and overlay settings.
/// Persists changes and reapplies all settings after parsing.
void DisplayManager_::setNewSettings(const char *json)
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Got new settings:"));
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(json);
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Error while parsing json"));
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(error.c_str());
        return;
    }
    if (doc.containsKey("ATIME"))
    {
        long atime = doc["ATIME"].as<int>();
        appConfig.timePerApp = atime * 1000;
    }

    if (doc.containsKey("OVERLAY"))
    {
        ui->setGlobalOverlay(overlayFromString(doc["OVERLAY"].as<String>()));
        if (doc.size() == 1)
            return;
    }

    timeConfig.timeMode = doc.containsKey("TMODE") ? doc["TMODE"].as<int>() : timeConfig.timeMode;
    appConfig.transEffect = doc.containsKey("TEFF") ? doc["TEFF"] : appConfig.transEffect;
    appConfig.timePerTransition = doc.containsKey("TSPEED") ? doc["TSPEED"] : appConfig.timePerTransition;
    brightnessConfig.brightness = doc.containsKey("BRI") ? doc["BRI"] : brightnessConfig.brightness;
    appConfig.scrollSpeed = doc.containsKey("SSPEED") ? doc["SSPEED"] : appConfig.scrollSpeed;
    if (doc.containsKey("NILAYOUT"))
    {
        appConfig.nativeIconLayout = layoutFromString(doc["NILAYOUT"].as<String>());
    }
    if (doc.containsKey("NMODE"))
        appConfig.nightMode = doc["NMODE"].as<bool>();
    if (doc.containsKey("NSTART"))
        appConfig.nightStart = doc["NSTART"].as<uint16_t>();
    if (doc.containsKey("NEND"))
        appConfig.nightEnd = doc["NEND"].as<uint16_t>();
    if (doc.containsKey("NBRI"))
        appConfig.nightBrightness = doc["NBRI"].as<uint8_t>();
    if (doc.containsKey("NCOL"))
        appConfig.nightColor = doc["NCOL"].as<uint32_t>();
    timeConfig.isCelsius = doc.containsKey("CEL") ? doc["CEL"] : timeConfig.isCelsius;
    timeConfig.startOnMonday = doc.containsKey("SOM") ? doc["SOM"].as<bool>() : timeConfig.startOnMonday;
    displayConfig.matrixOff = doc.containsKey("MATP") ? !doc["MATP"].as<bool>() : displayConfig.matrixOff;
    timeConfig.timeFormat = doc.containsKey("TFORMAT") ? doc["TFORMAT"].as<String>() : timeConfig.timeFormat;
    displayGamma = doc.containsKey("GAMMA") ? doc["GAMMA"].as<float>() : displayGamma;
    timeConfig.dateFormat = doc.containsKey("DFORMAT") ? doc["DFORMAT"].as<String>() : timeConfig.dateFormat;
    brightnessConfig.autoBrightness = doc.containsKey("ABRI") ? doc["ABRI"].as<bool>() : brightnessConfig.autoBrightness;
    appConfig.autoTransition = doc.containsKey("ATRANS") ? doc["ATRANS"].as<bool>() : appConfig.autoTransition;
    displayConfig.uppercaseLetters = doc.containsKey("UPPERCASE") ? doc["UPPERCASE"].as<bool>() : displayConfig.uppercaseLetters;
    appConfig.showWeekday = doc.containsKey("WD") ? doc["WD"].as<bool>() : appConfig.showWeekday;
    appConfig.blockNavigation = doc.containsKey("BLOCKN") ? doc["BLOCKN"].as<bool>() : appConfig.blockNavigation;
    appConfig.showTime = doc.containsKey("TIM") ? doc["TIM"].as<bool>() : appConfig.showTime;
    appConfig.showDate = doc.containsKey("DAT") ? doc["DAT"].as<bool>() : appConfig.showDate;
    appConfig.showHum = doc.containsKey("HUM") ? doc["HUM"].as<bool>() : appConfig.showHum;
    appConfig.showTemp = doc.containsKey("TEMP") ? doc["TEMP"].as<bool>() : appConfig.showTemp;
    appConfig.showBat = doc.containsKey("BAT") ? doc["BAT"].as<bool>() : appConfig.showBat;
    audioConfig.soundActive = doc.containsKey("SOUND") ? doc["SOUND"].as<bool>() : audioConfig.soundActive;

    if (doc.containsKey("BEFF"))
    {
        int beff = doc["BEFF"].as<int>();
        if (beff < -1 || beff >= kNumEffects)
            beff = -1;
        displayConfig.backgroundEffect = beff;
        ui->setBackgroundEffect(beff);
    }

    if (doc.containsKey("VOL"))
    {
        audioConfig.soundVolume = doc["VOL"];
        if (periphery_)
            periphery_->setVolume(audioConfig.soundVolume);
    }

    JsonObject obj = doc.as<JsonObject>();

    if (parseCRGBFromJson(obj, "CCORRECTION", colorCorrection))
        if (colorCorrection)
            FastLED.setCorrection(colorCorrection);

    if (parseCRGBFromJson(obj, "CTEMP", colorTemperature))
        if (colorTemperature)
            FastLED.setTemperature(colorTemperature);

    readColorField(obj, "WDCA", colorConfig.wdcActive, 0xFFFFFF);
    readColorField(obj, "CHCOL", colorConfig.calendarHeaderColor, 0xFF0000);
    readColorField(obj, "CTCOL", colorConfig.calendarTextColor, 0x000000);
    readColorField(obj, "CBCOL", colorConfig.calendarBodyColor, 0xFFFFFF);
    readColorField(obj, "WDCI", colorConfig.wdcInactive, 0x666666);

    if (doc.containsKey("TCOL"))
    {
        uint32_t TempColor = getColorFromJsonVariant(doc["TCOL"], 0xFFFFFF);
        setCustomAppColors(TempColor);
        colorConfig.textColor = TempColor;
    }

    readColorField(obj, "TIME_COL", colorConfig.timeColor, colorConfig.textColor);
    readColorField(obj, "DATE_COL", colorConfig.dateColor, colorConfig.textColor);
    readColorField(obj, "TEMP_COL", colorConfig.tempColor, colorConfig.textColor);
    readColorField(obj, "HUM_COL", colorConfig.humColor, colorConfig.textColor);
    readColorField(obj, "BAT_COL", colorConfig.batColor, colorConfig.textColor);
    doc.clear();
    applyAllSettings();
    saveSettings();
    if (systemConfig.debugMode)
        DEBUG_PRINTLN("Settings loaded");
}

/// Returns the current LED state as a flat JSON array of 256 RGB color values (row-major order).
/// Uses ledsCopy[] (pre-gamma) for accurate color representation.
String DisplayManager_::ledsAsJson()
{
    StaticJsonDocument<JSON_ARRAY_SIZE(MATRIX_WIDTH * MATRIX_HEIGHT)> jsonDoc;
    JsonArray jsonColors = jsonDoc.to<JsonArray>();
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            int index = matrix->XY(x, y);
            int color = (ledsCopy[index].r << 16) | (ledsCopy[index].g << 8) | ledsCopy[index].b;
            jsonColors.add(color);
        }
    }
    String jsonString;
    serializeJson(jsonColors, jsonString);
    return jsonString;
}

/// Returns a JSON array of all registered visual effect names (e.g. ["Fireworks","Matrix",...]).
String DisplayManager_::getEffectNames()
{
    StaticJsonDocument<1024> doc;
    JsonArray array = doc.to<JsonArray>();
    for (int i = 0; i < kNumEffects; i++)
    {
        array.add(effects[i].name);
    }
    String result;
    serializeJson(array, result);
    doc.clear();
    return result;
}

/// Returns a JSON array of available transition animation names (parsed from HAeffectOptions PROGMEM string).
String DisplayManager_::getTransitionNames()
{
    size_t len = strlen_P(HAeffectOptions);
    char effectOptions[len + 1];
    strcpy_P(effectOptions, HAeffectOptions);
    StaticJsonDocument<1024> doc;
    char *effect = strtok(effectOptions, ";");
    while (effect != NULL)
    {
        doc.add(effect);
        effect = strtok(NULL, ";");
    }
    String json;
    serializeJson(doc, json);
    doc.clear();
    return json;
}
