#include "Globals.h"
#include "Preferences.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "EffectRegistry.h"
#include "DisplayManager.h"
#include "LayoutEngine.h"
#include "timer.h"
#include "NightModeWindow.h"

Preferences Settings;

bool isNightModeActive()
{
    if (!appConfig.nightMode)
        return false;

    const struct tm *now = timer_localtime();
    if (!now)
        return false;

    // Guard against the pre-NTP epoch window. tm_year is years-since-1900,
    // so values < 120 (i.e. before 2020) mean NTP hasn't synced yet and the
    // clock is reading ~1970. Without this guard, the defaults (start=21:00,
    // end=06:00) would activate at boot because 00:00 falls inside the
    // wrap-around window.
    if (now->tm_year < 120)
        return false;

    const uint16_t currentMinutes = static_cast<uint16_t>(now->tm_hour * 60 + now->tm_min);
    return isWithinNightWindow(currentMinutes, appConfig.nightStart, appConfig.nightEnd);
}

const char *getID()
{
    static char macStr[24];
    uint8_t mac[6];
    WiFi.macAddress(mac);
    snprintf(macStr, sizeof(macStr), "svitrix_%02x%02x%02x", mac[3], mac[4], mac[5]);
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Generated device ID"));
    return macStr;
}

void startLittleFS()
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Starting filesystem"));
    if (LittleFS.begin())
    {
        if (LittleFS.exists("/config.json"))
        {
            LittleFS.rename("/config.json", "/DoNotTouch.json");
        }

#ifdef ULANZI
        LittleFS.mkdir("/MELODIES");
#endif
        LittleFS.mkdir("/ICONS");
        LittleFS.mkdir("/PALETTES");
        LittleFS.mkdir("/CUSTOMAPPS");
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Filesystem started"));
    }
    else
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Filesystem corrupt. Formatting..."));
        LittleFS.format();
        ESP.restart();
    }
}

void loadDevSettings()
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN("Loading Devsettings");
    if (LittleFS.exists("/dev.json"))
    {
        File file = LittleFS.open("/dev.json", "r");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            file.close();
            if (systemConfig.debugMode)
                DEBUG_PRINTLN(F("Failed to read dev settings"));
            return;
        }

        if (systemConfig.debugMode)
            DEBUG_PRINTF("%i dev settings found", doc.size());

        if (doc.containsKey("bootsound"))
        {
            audioConfig.bootSound = doc["bootsound"].as<String>();
        }

        if (doc.containsKey("sensor_reading"))
        {
            sensorConfig.sensorReading = doc["sensor_reading"].as<bool>();
        }

        if (doc.containsKey("matrix"))
        {
            displayConfig.matrixLayout = doc["matrix"];
        }

        if (doc.containsKey("mirror_screen"))
        {
            displayConfig.mirrorDisplay = doc["mirror_screen"].as<bool>();
        }

        if (doc.containsKey("temp_offset"))
        {
            sensorConfig.tempOffset = doc["temp_offset"];
        }

        if (doc.containsKey("min_battery"))
        {
            batteryConfig.minRaw = doc["min_battery"];
        }

        if (doc.containsKey("max_battery"))
        {
            batteryConfig.maxRaw = doc["max_battery"];
        }

        if (doc.containsKey("ap_timeout"))
        {
            systemConfig.apTimeout = doc["ap_timeout"];
        }

        if (doc.containsKey("background_effect"))
        {
            displayConfig.backgroundEffect = getEffectIndex(doc["background_effect"].as<const char *>());
        }

        if (doc.containsKey("min_brightness"))
        {
            brightnessConfig.minBrightness = doc["min_brightness"];
        }

        if (doc.containsKey("max_brightness"))
        {
            brightnessConfig.maxBrightness = doc["max_brightness"];
        }

        if (doc.containsKey("ldr_factor"))
        {
            brightnessConfig.ldrFactor = doc["ldr_factor"].as<float>();
        }

        if (doc.containsKey("ldr_gamma"))
        {
            brightnessConfig.ldrGamma = doc["ldr_gamma"].as<float>();
        }

        if (doc.containsKey("hum_offset"))
        {
            sensorConfig.humOffset = doc["hum_offset"];
        }

        if (doc.containsKey("ha_prefix"))
        {
            haConfig.prefix = doc["ha_prefix"].as<String>();
        }

        if (doc.containsKey("stats_interval"))
        {
            systemConfig.statsInterval = doc["stats_interval"].as<long>();
        }

        if (doc.containsKey("update_check"))
        {
            systemConfig.updateCheck = doc["update_check"].as<bool>();
        }

        if (doc.containsKey("hostname"))
        {
            systemConfig.hostname = doc["hostname"].as<String>();
        }


        if (doc.containsKey("web_port"))
        {
            systemConfig.webPort = doc["web_port"];
        }

        if (doc.containsKey("temp_dec_places"))
        {
            timeConfig.tempDecimalPlaces = doc["temp_dec_places"].as<int>();
        }

        if (doc.containsKey("rotate_screen"))
        {
            displayConfig.rotateScreen = doc["rotate_screen"].as<bool>();
        }

        if (doc.containsKey("debug_mode"))
        {
            systemConfig.debugMode = doc["debug_mode"].as<bool>();
        }

        if (doc.containsKey("update_version_url"))
        {
            systemConfig.updateVersionUrl = doc["update_version_url"].as<String>();
        }

        if (doc.containsKey("update_firmware_url"))
        {
            systemConfig.updateFirmwareUrl = doc["update_firmware_url"].as<String>();
        }

        if (doc.containsKey("new_year"))
        {
            systemConfig.newyear = doc["new_year"].as<bool>();
        }

        if (doc.containsKey("swap_buttons"))
        {
            systemConfig.swapButtons = doc["swap_buttons"].as<bool>();
        }

        if (doc.containsKey("ldr_on_ground"))
        {
            brightnessConfig.ldrOnGround = doc["ldr_on_ground"].as<bool>();
        }

        if (doc.containsKey("button_callback"))
        {
            systemConfig.buttonCallback = doc["button_callback"].as<String>();
        }

        if (doc.containsKey("color_correction"))
        {
            auto correction = doc["color_correction"];
            if (correction.is<JsonArray>() && correction.size() == 3)
            {
                CRGB c;
                c.setRGB(correction[0], correction[1], correction[2]);
                DisplayManager.setColorCorrection(c);
            }
        }

        if (doc.containsKey("color_temperature"))
        {
            auto temperature = doc["color_temperature"];
            if (temperature.is<JsonArray>() && temperature.size() == 3)
            {
                CRGB t;
                t.setRGB(temperature[0], temperature[1], temperature[2]);
                DisplayManager.setColorTemperature(t);
            }
        }

        file.close();
    }
    else
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN("Devsettings not found");
    }
}

void formatSettings()
{
    Settings.begin("svitrix", false);
    Settings.clear();
    Settings.end();
}

void loadSettings()
{
    startLittleFS();
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Loading Usersettings"));
    Settings.begin("svitrix", false);
    brightnessConfig.brightness = Settings.getUInt("BRI", 120);
    brightnessConfig.autoBrightness = Settings.getBool("ABRI", false);
    displayConfig.uppercaseLetters = Settings.getBool("UPPER", true);
    colorConfig.textColor = Settings.getUInt("TCOL", 0xFFFFFF);
    colorConfig.calendarHeaderColor = Settings.getUInt("CHCOL", 0xFF0000);
    colorConfig.calendarTextColor = Settings.getUInt("CTCOL", 0x000000);
    colorConfig.calendarBodyColor = Settings.getUInt("CBCOL", 0xFFFFFF);
    appConfig.transEffect = Settings.getUInt("TEFF", 1);
    timeConfig.timeMode = Settings.getUInt("TMODE", 1);
    colorConfig.timeColor = Settings.getUInt("TIME_COL", 0);
    colorConfig.dateColor = Settings.getUInt("DATE_COL", 0);
    colorConfig.tempColor = Settings.getUInt("TEMP_COL", 0);
    colorConfig.humColor = Settings.getUInt("HUM_COL", 0);
#ifdef ULANZI
    colorConfig.batColor = Settings.getUInt("BAT_COL", 0);
#endif
    colorConfig.wdcActive = Settings.getUInt("WDCA", 0xFFFFFF);
    colorConfig.wdcInactive = Settings.getUInt("WDCI", 0x666666);
    appConfig.autoTransition = Settings.getBool("ATRANS", true);
    appConfig.showWeekday = Settings.getBool("WD", true);
    appConfig.timePerTransition = Settings.getUInt("TSPEED", 400);
    appConfig.timePerApp = Settings.getUInt("ATIME", 7000);
    timeConfig.timeFormat = Settings.getString("TFORMAT", "%H %M");
    timeConfig.dateFormat = Settings.getString("DFORMAT", "%d.%m.%y");
    timeConfig.startOnMonday = Settings.getBool("SOM", true);
    appConfig.blockNavigation = Settings.getBool("BLOCKN", false);
    timeConfig.isCelsius = Settings.getBool("CEL", true);
    appConfig.showTime = Settings.getBool("TIM", true);
    appConfig.showDate = Settings.getBool("DAT", false);
    appConfig.showTemp = Settings.getBool("TEMP", true);
    appConfig.showHum = Settings.getBool("HUM", true);
    displayConfig.matrixLayout = Settings.getUInt("MAT", 0);
    displayConfig.backgroundEffect = Settings.getInt("BEFF", -1);
    appConfig.scrollSpeed = Settings.getUInt("SSPEED", 100);
    appConfig.nativeIconLayout = static_cast<IconLayout>(Settings.getUChar("NILAYOUT", 0));
    appConfig.nightMode = Settings.getBool("NMODE", false);
    appConfig.nightStart = Settings.getUShort("NSTART", 1260);
    appConfig.nightEnd = Settings.getUShort("NEND", 360);
    appConfig.nightBrightness = Settings.getUChar("NBRI", 5);
    appConfig.nightColor = Settings.getUInt("NCOL", 0xFF0000);
    appConfig.nightBlockTransition = Settings.getBool("NBTRANS", true);
#ifdef ULANZI
    appConfig.showBat = Settings.getBool("BAT", true);
#endif
    audioConfig.soundActive = Settings.getBool("SOUND", true);
    audioConfig.soundVolume = Settings.getUInt("VOL", 25);
    Settings.end();
    systemConfig.deviceId = getID();
    mqttConfig.prefix = systemConfig.deviceId;
    systemConfig.hostname = systemConfig.deviceId;
    loadDevSettings();
}

void saveSettings()
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Saving usersettings"));
    Settings.begin("svitrix", false);
    Settings.putUInt("CHCOL", colorConfig.calendarHeaderColor);
    Settings.putUInt("CTCOL", colorConfig.calendarTextColor);
    Settings.putUInt("CBCOL", colorConfig.calendarBodyColor);
    Settings.putUInt("TEFF", appConfig.transEffect);
    Settings.putUInt("BRI", brightnessConfig.brightness);
    Settings.putBool("WD", appConfig.showWeekday);
    Settings.putBool("ABRI", brightnessConfig.autoBrightness);
    Settings.putBool("BLOCKN", appConfig.blockNavigation);
    Settings.putBool("ATRANS", appConfig.autoTransition);
    Settings.putBool("UPPER", displayConfig.uppercaseLetters);
    Settings.putUInt("TCOL", colorConfig.textColor);
    Settings.putUInt("TMODE", timeConfig.timeMode);
    Settings.putUInt("TIME_COL", colorConfig.timeColor);
    Settings.putUInt("DATE_COL", colorConfig.dateColor);
    Settings.putUInt("TEMP_COL", colorConfig.tempColor);
    Settings.putUInt("HUM_COL", colorConfig.humColor);
#ifdef ULANZI
    Settings.putUInt("BAT_COL", colorConfig.batColor);
#endif
    Settings.putUInt("WDCA", colorConfig.wdcActive);
    Settings.putUInt("WDCI", colorConfig.wdcInactive);
    Settings.putUInt("TSPEED", appConfig.timePerTransition);
    Settings.putUInt("ATIME", appConfig.timePerApp);
    Settings.putString("TFORMAT", timeConfig.timeFormat);
    Settings.putString("DFORMAT", timeConfig.dateFormat);
    Settings.putBool("SOM", timeConfig.startOnMonday);
    Settings.putBool("CEL", timeConfig.isCelsius);
    Settings.putBool("TIM", appConfig.showTime);
    Settings.putBool("DAT", appConfig.showDate);
    Settings.putBool("TEMP", appConfig.showTemp);
    Settings.putBool("HUM", appConfig.showHum);
    Settings.putUInt("SSPEED", appConfig.scrollSpeed);
    Settings.putInt("BEFF", displayConfig.backgroundEffect);
    Settings.putUChar("NILAYOUT", static_cast<uint8_t>(appConfig.nativeIconLayout));
    Settings.putBool("NMODE", appConfig.nightMode);
    Settings.putUShort("NSTART", appConfig.nightStart);
    Settings.putUShort("NEND", appConfig.nightEnd);
    Settings.putUChar("NBRI", appConfig.nightBrightness);
    Settings.putUInt("NCOL", appConfig.nightColor);
    Settings.putBool("NBTRANS", appConfig.nightBlockTransition);
#ifdef ULANZI
    Settings.putBool("BAT", appConfig.showBat);
#endif
    Settings.putBool("SOUND", audioConfig.soundActive);
    Settings.putUInt("VOL", audioConfig.soundVolume);
    Settings.end();
}

MqttConfig mqttConfig = {"", 1883, "", "", ""};
NetworkConfig networkConfig = {false, "192.168.178.10", "192.168.178.1", "255.255.255.0", "8.8.8.8", "1.1.1.1"};
HaConfig haConfig = {false, "homeassistant"};
SensorConfig sensorConfig = {0, 0, 0, 0, TEMP_SENSOR_TYPE_NONE, true, false, -9, 0};
BatteryConfig batteryConfig = {0, 0, 475, 665};
AuthConfig authConfig = {"", "svitrix"};

DisplayConfig displayConfig = {0, 42, false, false, false, true, -1};
BrightnessConfig brightnessConfig = {120, 0, true, 2, 160, 3.0, 1.0, false};
ColorConfig colorConfig = {0xFFFFFF, 0, 0, 0, 0, 0, 0xFFFFFF, 0x666666, 0xFF0000, 0x000000, 0xFFFFFF};
TimeConfig timeConfig = {"%H:%M:%S", "%d.%m.%y", 1, false, "de.pool.ntp.org", "CET-1CEST,M3.5.0,M10.5.0/3", false, 0};
AppConfig appConfig = {true, true, true, true, true, true, false, 1, 400, 7000, 100, IconLayout::Left, false, false, 1260, 360, 5, 0xFF0000, true};
AudioConfig audioConfig = {false, 30, ""};
SystemConfig systemConfig = {true, 15, 80, "", false, 10000, false, false, "", "", false, false, "", ""};
