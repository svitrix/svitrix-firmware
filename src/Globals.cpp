#include "Globals.h"
#include "Preferences.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "EffectRegistry.h"
#include "DisplayManager.h"
#include "LayoutEngine.h"

Preferences Settings;

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
    brightnessConfig.minBrightness = Settings.getUChar("MINBRI", 2);
    brightnessConfig.maxBrightness = Settings.getUChar("MAXBRI", 160);
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
    colorConfig.timerColor = Settings.getUInt("TIMER_COL", 0);
    colorConfig.stopwatchColor = Settings.getUInt("SW_COL", 0);
    colorConfig.alarmsColor = Settings.getUInt("ALARMS_COL", 0);
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
    appConfig.showTimer = Settings.getBool("STIMER", false);
    appConfig.showStopwatch = Settings.getBool("SSW", false);
    appConfig.showAlarms = Settings.getBool("SALARMS", false);
    sensorConfig.tempOffset = Settings.getFloat("TOFF", -9.0);
    displayConfig.matrixLayout = Settings.getUInt("MAT", 0);
    displayConfig.backgroundEffect = Settings.getInt("BEFF", -1);
    appConfig.scrollSpeed = Settings.getUInt("SSPEED", 100);
    appConfig.timeDuration = Settings.getUShort("TIMEDUR", 7);
    appConfig.dateDuration = Settings.getUShort("DATEDUR", 7);
    appConfig.tempDuration = Settings.getUShort("TEMPDUR", 7);
    appConfig.humDuration = Settings.getUShort("HUMDUR", 7);
    appConfig.batDuration = Settings.getUShort("BATDUR", 7);
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
    wifiConfig.networks[0].ssid = Settings.getString("WSSID1", "");
    wifiConfig.networks[0].password = Settings.getString("WPASS1", "");
    wifiConfig.networks[1].ssid = Settings.getString("WSSID2", "");
    wifiConfig.networks[1].password = Settings.getString("WPASS2", "");
    wifiConfig.networks[2].ssid = Settings.getString("WSSID3", "");
    wifiConfig.networks[2].password = Settings.getString("WPASS3", "");
    // Weather API config
    weatherConfig.apiKey = Settings.getString("WAPI_KEY", "");
    weatherConfig.locationType = static_cast<WeatherLocationType>(Settings.getUChar("WAPI_LOC", 0));
    weatherConfig.city = Settings.getString("WAPI_CITY", "");
    weatherConfig.latitude = Settings.getFloat("WAPI_LAT", 0.0);
    weatherConfig.longitude = Settings.getFloat("WAPI_LON", 0.0);
    weatherConfig.stationId = Settings.getString("WAPI_STA", "");
    weatherConfig.updateInterval = Settings.getUShort("WAPI_INT", 30);
    weatherConfig.showOutdoorTemp = Settings.getBool("WAPI_OTEMP", true);
    weatherConfig.showOutdoorHumidity = Settings.getBool("WAPI_OHUM", false);
    weatherConfig.showPressure = Settings.getBool("WAPI_PRES", false);
    weatherConfig.showAirQuality = Settings.getBool("WAPI_AQI", false);
    weatherConfig.showIndoorTemp = Settings.getBool("WAPI_ITEMP", false);
    weatherConfig.showIndoorHumidity = Settings.getBool("WAPI_IHUM", false);
    weatherConfig.outdoorTempColor = Settings.getUInt("WAPI_OTCOL", 0);
    weatherConfig.outdoorHumColor = Settings.getUInt("WAPI_OHCOL", 0);
    weatherConfig.pressureColor = Settings.getUInt("WAPI_PCOL", 0);
    weatherConfig.aqiColor = Settings.getUInt("WAPI_AQCOL", 0);
    weatherConfig.outdoorTempDuration = Settings.getUChar("WAPI_OTDUR", 7);
    weatherConfig.outdoorHumDuration = Settings.getUChar("WAPI_OHDUR", 7);
    weatherConfig.pressureDuration = Settings.getUChar("WAPI_PDUR", 7);
    weatherConfig.aqiDuration = Settings.getUChar("WAPI_AQDUR", 7);
    weatherConfig.showUV = Settings.getBool("WAPI_UV", false);
    weatherConfig.uvColor = Settings.getUInt("WAPI_UVCOL", 0x9C27B0);
    weatherConfig.uvDuration = Settings.getUChar("WAPI_UVDUR", 7);
    Settings.end();
    systemConfig.deviceId = getID();
    mqttConfig.prefix = systemConfig.deviceId;
    systemConfig.hostname = "svitrix";
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
    Settings.putUChar("MINBRI", brightnessConfig.minBrightness);
    Settings.putUChar("MAXBRI", brightnessConfig.maxBrightness);
    Settings.putBool("BLOCKN", appConfig.blockNavigation);
    Settings.putBool("ATRANS", appConfig.autoTransition);
    Settings.putBool("UPPER", displayConfig.uppercaseLetters);
    Settings.putUInt("TCOL", colorConfig.textColor);
    Settings.putUInt("TMODE", timeConfig.timeMode);
    Settings.putUInt("TIME_COL", colorConfig.timeColor);
    Settings.putUInt("DATE_COL", colorConfig.dateColor);
    Settings.putUInt("TEMP_COL", colorConfig.tempColor);
    Settings.putUInt("HUM_COL", colorConfig.humColor);
    Settings.putUInt("TIMER_COL", colorConfig.timerColor);
    Settings.putUInt("SW_COL", colorConfig.stopwatchColor);
    Settings.putUInt("ALARMS_COL", colorConfig.alarmsColor);
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
    Settings.putBool("STIMER", appConfig.showTimer);
    Settings.putBool("SSW", appConfig.showStopwatch);
    Settings.putBool("SALARMS", appConfig.showAlarms);
    Settings.putFloat("TOFF", sensorConfig.tempOffset);
    Settings.putUInt("SSPEED", appConfig.scrollSpeed);
    Settings.putUShort("TIMEDUR", appConfig.timeDuration);
    Settings.putUShort("DATEDUR", appConfig.dateDuration);
    Settings.putUShort("TEMPDUR", appConfig.tempDuration);
    Settings.putUShort("HUMDUR", appConfig.humDuration);
    Settings.putUShort("BATDUR", appConfig.batDuration);
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
    Settings.putString("WSSID1", wifiConfig.networks[0].ssid);
    Settings.putString("WPASS1", wifiConfig.networks[0].password);
    Settings.putString("WSSID2", wifiConfig.networks[1].ssid);
    Settings.putString("WPASS2", wifiConfig.networks[1].password);
    Settings.putString("WSSID3", wifiConfig.networks[2].ssid);
    Settings.putString("WPASS3", wifiConfig.networks[2].password);
    // Weather API config
    Settings.putString("WAPI_KEY", weatherConfig.apiKey);
    Settings.putUChar("WAPI_LOC", static_cast<uint8_t>(weatherConfig.locationType));
    Settings.putString("WAPI_CITY", weatherConfig.city);
    Settings.putFloat("WAPI_LAT", weatherConfig.latitude);
    Settings.putFloat("WAPI_LON", weatherConfig.longitude);
    Settings.putString("WAPI_STA", weatherConfig.stationId);
    Settings.putUShort("WAPI_INT", weatherConfig.updateInterval);
    Settings.putBool("WAPI_OTEMP", weatherConfig.showOutdoorTemp);
    Settings.putBool("WAPI_OHUM", weatherConfig.showOutdoorHumidity);
    Settings.putBool("WAPI_PRES", weatherConfig.showPressure);
    Settings.putBool("WAPI_AQI", weatherConfig.showAirQuality);
    Settings.putBool("WAPI_ITEMP", weatherConfig.showIndoorTemp);
    Settings.putBool("WAPI_IHUM", weatherConfig.showIndoorHumidity);
    Settings.putUInt("WAPI_OTCOL", weatherConfig.outdoorTempColor);
    Settings.putUInt("WAPI_OHCOL", weatherConfig.outdoorHumColor);
    Settings.putUInt("WAPI_PCOL", weatherConfig.pressureColor);
    Settings.putUInt("WAPI_AQCOL", weatherConfig.aqiColor);
    Settings.putUChar("WAPI_OTDUR", weatherConfig.outdoorTempDuration);
    Settings.putUChar("WAPI_OHDUR", weatherConfig.outdoorHumDuration);
    Settings.putUChar("WAPI_PDUR", weatherConfig.pressureDuration);
    Settings.putUChar("WAPI_AQDUR", weatherConfig.aqiDuration);
    Settings.putBool("WAPI_UV", weatherConfig.showUV);
    Settings.putUInt("WAPI_UVCOL", weatherConfig.uvColor);
    Settings.putUChar("WAPI_UVDUR", weatherConfig.uvDuration);
    Settings.end();
}

WifiConfig wifiConfig = {{{""}, {""}, {""}}};
MqttConfig mqttConfig = {"", 1883, "", "", ""};
NetworkConfig networkConfig = {false, "192.168.178.10", "192.168.178.1", "255.255.255.0", "8.8.8.8", "1.1.1.1"};
HaConfig haConfig = {false, "homeassistant"};
SensorConfig sensorConfig = {0, 0, 0, 0, TEMP_SENSOR_TYPE_NONE, true, false, -9, 0};
BatteryConfig batteryConfig = {0, 0, 475, 665};
AuthConfig authConfig = {"", "svitrix"};

DisplayConfig displayConfig = {0, 42, false, false, false, true, -1};
BrightnessConfig brightnessConfig = {120, 0, true, 2, 160, 3.0, 1.0, false};
ColorConfig colorConfig = {0xFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0xFFFFFF, 0x666666, 0xFF0000, 0x000000, 0xFFFFFF};
TimeConfig timeConfig = {"%H:%M:%S", "%d.%m.%y", 1, false, "time.cloudflare.com", "CST6", false, 0};
AppConfig appConfig = {true, true, true, true, true, true, false, false, false, false, 1, 400, 7000, 100, 7, 7, 7, 7, 7, IconLayout::Left, false, false, 1260, 360, 5, 0xFF0000, true};
AudioConfig audioConfig = {false, 30, ""};
SystemConfig systemConfig = {true, 15, 80, "", false, 10000, false, false, "", "", false, false, "", ""};
WeatherConfig weatherConfig = {"", WEATHER_LOC_CITY, "", 0.0, 0.0, "", 30, true, false, false, false, false, false, 0, 0, 0, 0, 7, 7, 7, 7};
WeatherData weatherData = {0, 0, 0, 0, 0, "", 0, 0, false};

String exportSettings()
{
    DynamicJsonDocument doc(4096);

    // Brightness
    doc["BRI"] = brightnessConfig.brightness;
    doc["ABRI"] = brightnessConfig.autoBrightness;
    doc["MINBRI"] = brightnessConfig.minBrightness;
    doc["MAXBRI"] = brightnessConfig.maxBrightness;

    // Display
    doc["UPPER"] = displayConfig.uppercaseLetters;
    doc["MAT"] = displayConfig.matrixLayout;
    doc["BEFF"] = displayConfig.backgroundEffect;

    // Colors
    doc["TCOL"] = colorConfig.textColor;
    doc["CHCOL"] = colorConfig.calendarHeaderColor;
    doc["CTCOL"] = colorConfig.calendarTextColor;
    doc["CBCOL"] = colorConfig.calendarBodyColor;
    doc["TIME_COL"] = colorConfig.timeColor;
    doc["DATE_COL"] = colorConfig.dateColor;
    doc["TEMP_COL"] = colorConfig.tempColor;
    doc["HUM_COL"] = colorConfig.humColor;
    doc["TIMER_COL"] = colorConfig.timerColor;
    doc["SW_COL"] = colorConfig.stopwatchColor;
    doc["ALARMS_COL"] = colorConfig.alarmsColor;
    doc["BAT_COL"] = colorConfig.batColor;
    doc["WDCA"] = colorConfig.wdcActive;
    doc["WDCI"] = colorConfig.wdcInactive;

    // App config
    doc["TEFF"] = appConfig.transEffect;
    doc["ATRANS"] = appConfig.autoTransition;
    doc["WD"] = appConfig.showWeekday;
    doc["TSPEED"] = appConfig.timePerTransition;
    doc["ATIME"] = appConfig.timePerApp;
    doc["BLOCKN"] = appConfig.blockNavigation;
    doc["TIM"] = appConfig.showTime;
    doc["DAT"] = appConfig.showDate;
    doc["TEMP"] = appConfig.showTemp;
    doc["HUM"] = appConfig.showHum;
    doc["BAT"] = appConfig.showBat;
    doc["STIMER"] = appConfig.showTimer;
    doc["SSW"] = appConfig.showStopwatch;
    doc["SALARMS"] = appConfig.showAlarms;
    doc["SSPEED"] = appConfig.scrollSpeed;
    doc["TIMEDUR"] = appConfig.timeDuration;
    doc["DATEDUR"] = appConfig.dateDuration;
    doc["TEMPDUR"] = appConfig.tempDuration;
    doc["HUMDUR"] = appConfig.humDuration;
    doc["BATDUR"] = appConfig.batDuration;
    doc["NILAYOUT"] = static_cast<uint8_t>(appConfig.nativeIconLayout);
    doc["NMODE"] = appConfig.nightMode;
    doc["NSTART"] = appConfig.nightStart;
    doc["NEND"] = appConfig.nightEnd;
    doc["NBRI"] = appConfig.nightBrightness;
    doc["NCOL"] = appConfig.nightColor;
    doc["NBTRANS"] = appConfig.nightBlockTransition;

    // Time config
    doc["TMODE"] = timeConfig.timeMode;
    doc["TFORMAT"] = timeConfig.timeFormat;
    doc["DFORMAT"] = timeConfig.dateFormat;
    doc["SOM"] = timeConfig.startOnMonday;
    doc["CEL"] = timeConfig.isCelsius;

    // Sensor
    doc["TOFF"] = sensorConfig.tempOffset;

    // Audio
    doc["SOUND"] = audioConfig.soundActive;
    doc["VOL"] = audioConfig.soundVolume;

    // WiFi (include for full backup)
    doc["WSSID1"] = wifiConfig.networks[0].ssid;
    doc["WPASS1"] = wifiConfig.networks[0].password;
    doc["WSSID2"] = wifiConfig.networks[1].ssid;
    doc["WPASS2"] = wifiConfig.networks[1].password;
    doc["WSSID3"] = wifiConfig.networks[2].ssid;
    doc["WPASS3"] = wifiConfig.networks[2].password;

    // Weather API
    doc["WAPI_KEY"] = weatherConfig.apiKey;
    doc["WAPI_LOC"] = static_cast<uint8_t>(weatherConfig.locationType);
    doc["WAPI_CITY"] = weatherConfig.city;
    doc["WAPI_LAT"] = weatherConfig.latitude;
    doc["WAPI_LON"] = weatherConfig.longitude;
    doc["WAPI_STA"] = weatherConfig.stationId;
    doc["WAPI_INT"] = weatherConfig.updateInterval;
    doc["WAPI_OTEMP"] = weatherConfig.showOutdoorTemp;
    doc["WAPI_OHUM"] = weatherConfig.showOutdoorHumidity;
    doc["WAPI_PRES"] = weatherConfig.showPressure;
    doc["WAPI_AQI"] = weatherConfig.showAirQuality;
    doc["WAPI_ITEMP"] = weatherConfig.showIndoorTemp;
    doc["WAPI_IHUM"] = weatherConfig.showIndoorHumidity;
    doc["WAPI_OTCOL"] = weatherConfig.outdoorTempColor;
    doc["WAPI_OHCOL"] = weatherConfig.outdoorHumColor;
    doc["WAPI_PCOL"] = weatherConfig.pressureColor;
    doc["WAPI_AQCOL"] = weatherConfig.aqiColor;
    doc["WAPI_OTDUR"] = weatherConfig.outdoorTempDuration;
    doc["WAPI_OHDUR"] = weatherConfig.outdoorHumDuration;
    doc["WAPI_PDUR"] = weatherConfig.pressureDuration;
    doc["WAPI_AQDUR"] = weatherConfig.aqiDuration;
    doc["WAPI_UV"] = weatherConfig.showUV;
    doc["WAPI_UVCOL"] = weatherConfig.uvColor;
    doc["WAPI_UVDUR"] = weatherConfig.uvDuration;

    String output;
    serializeJson(doc, output);
    return output;
}

bool importSettings(const char* json)
{
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        DEBUG_PRINTLN(F("importSettings: JSON parse error"));
        return false;
    }

    // Brightness
    if (doc.containsKey("BRI")) brightnessConfig.brightness = doc["BRI"];
    if (doc.containsKey("ABRI")) brightnessConfig.autoBrightness = doc["ABRI"];
    if (doc.containsKey("MINBRI")) brightnessConfig.minBrightness = doc["MINBRI"];
    if (doc.containsKey("MAXBRI")) brightnessConfig.maxBrightness = doc["MAXBRI"];

    // Display
    if (doc.containsKey("UPPER")) displayConfig.uppercaseLetters = doc["UPPER"];
    if (doc.containsKey("MAT")) displayConfig.matrixLayout = doc["MAT"];
    if (doc.containsKey("BEFF")) displayConfig.backgroundEffect = doc["BEFF"];

    // Colors
    if (doc.containsKey("TCOL")) colorConfig.textColor = doc["TCOL"];
    if (doc.containsKey("CHCOL")) colorConfig.calendarHeaderColor = doc["CHCOL"];
    if (doc.containsKey("CTCOL")) colorConfig.calendarTextColor = doc["CTCOL"];
    if (doc.containsKey("CBCOL")) colorConfig.calendarBodyColor = doc["CBCOL"];
    if (doc.containsKey("TIME_COL")) colorConfig.timeColor = doc["TIME_COL"];
    if (doc.containsKey("DATE_COL")) colorConfig.dateColor = doc["DATE_COL"];
    if (doc.containsKey("TEMP_COL")) colorConfig.tempColor = doc["TEMP_COL"];
    if (doc.containsKey("HUM_COL")) colorConfig.humColor = doc["HUM_COL"];
    if (doc.containsKey("TIMER_COL")) colorConfig.timerColor = doc["TIMER_COL"];
    if (doc.containsKey("SW_COL")) colorConfig.stopwatchColor = doc["SW_COL"];
    if (doc.containsKey("ALARMS_COL")) colorConfig.alarmsColor = doc["ALARMS_COL"];
    if (doc.containsKey("BAT_COL")) colorConfig.batColor = doc["BAT_COL"];
    if (doc.containsKey("WDCA")) colorConfig.wdcActive = doc["WDCA"];
    if (doc.containsKey("WDCI")) colorConfig.wdcInactive = doc["WDCI"];

    // App config
    if (doc.containsKey("TEFF")) appConfig.transEffect = doc["TEFF"];
    if (doc.containsKey("ATRANS")) appConfig.autoTransition = doc["ATRANS"];
    if (doc.containsKey("WD")) appConfig.showWeekday = doc["WD"];
    if (doc.containsKey("TSPEED")) appConfig.timePerTransition = doc["TSPEED"];
    if (doc.containsKey("ATIME")) appConfig.timePerApp = doc["ATIME"];
    if (doc.containsKey("BLOCKN")) appConfig.blockNavigation = doc["BLOCKN"];
    if (doc.containsKey("TIM")) appConfig.showTime = doc["TIM"];
    if (doc.containsKey("DAT")) appConfig.showDate = doc["DAT"];
    if (doc.containsKey("TEMP")) appConfig.showTemp = doc["TEMP"];
    if (doc.containsKey("HUM")) appConfig.showHum = doc["HUM"];
    if (doc.containsKey("BAT")) appConfig.showBat = doc["BAT"];
    if (doc.containsKey("STIMER")) appConfig.showTimer = doc["STIMER"];
    if (doc.containsKey("SSW")) appConfig.showStopwatch = doc["SSW"];
    if (doc.containsKey("SALARMS")) appConfig.showAlarms = doc["SALARMS"];
    if (doc.containsKey("SSPEED")) appConfig.scrollSpeed = doc["SSPEED"];
    if (doc.containsKey("TIMEDUR")) appConfig.timeDuration = doc["TIMEDUR"];
    if (doc.containsKey("DATEDUR")) appConfig.dateDuration = doc["DATEDUR"];
    if (doc.containsKey("TEMPDUR")) appConfig.tempDuration = doc["TEMPDUR"];
    if (doc.containsKey("HUMDUR")) appConfig.humDuration = doc["HUMDUR"];
    if (doc.containsKey("BATDUR")) appConfig.batDuration = doc["BATDUR"];
    if (doc.containsKey("NILAYOUT")) appConfig.nativeIconLayout = static_cast<IconLayout>(doc["NILAYOUT"].as<uint8_t>());
    if (doc.containsKey("NMODE")) appConfig.nightMode = doc["NMODE"];
    if (doc.containsKey("NSTART")) appConfig.nightStart = doc["NSTART"];
    if (doc.containsKey("NEND")) appConfig.nightEnd = doc["NEND"];
    if (doc.containsKey("NBRI")) appConfig.nightBrightness = doc["NBRI"];
    if (doc.containsKey("NCOL")) appConfig.nightColor = doc["NCOL"];
    if (doc.containsKey("NBTRANS")) appConfig.nightBlockTransition = doc["NBTRANS"];

    // Time config
    if (doc.containsKey("TMODE")) timeConfig.timeMode = doc["TMODE"];
    if (doc.containsKey("TFORMAT")) timeConfig.timeFormat = doc["TFORMAT"].as<String>();
    if (doc.containsKey("DFORMAT")) timeConfig.dateFormat = doc["DFORMAT"].as<String>();
    if (doc.containsKey("SOM")) timeConfig.startOnMonday = doc["SOM"];
    if (doc.containsKey("CEL")) timeConfig.isCelsius = doc["CEL"];

    // Sensor
    if (doc.containsKey("TOFF")) sensorConfig.tempOffset = doc["TOFF"];

    // Audio
    if (doc.containsKey("SOUND")) audioConfig.soundActive = doc["SOUND"];
    if (doc.containsKey("VOL")) audioConfig.soundVolume = doc["VOL"];

    // WiFi
    if (doc.containsKey("WSSID1")) wifiConfig.networks[0].ssid = doc["WSSID1"].as<String>();
    if (doc.containsKey("WPASS1")) wifiConfig.networks[0].password = doc["WPASS1"].as<String>();
    if (doc.containsKey("WSSID2")) wifiConfig.networks[1].ssid = doc["WSSID2"].as<String>();
    if (doc.containsKey("WPASS2")) wifiConfig.networks[1].password = doc["WPASS2"].as<String>();
    if (doc.containsKey("WSSID3")) wifiConfig.networks[2].ssid = doc["WSSID3"].as<String>();
    if (doc.containsKey("WPASS3")) wifiConfig.networks[2].password = doc["WPASS3"].as<String>();

    // Weather API
    if (doc.containsKey("WAPI_KEY")) weatherConfig.apiKey = doc["WAPI_KEY"].as<String>();
    if (doc.containsKey("WAPI_LOC")) weatherConfig.locationType = static_cast<WeatherLocationType>(doc["WAPI_LOC"].as<uint8_t>());
    if (doc.containsKey("WAPI_CITY")) weatherConfig.city = doc["WAPI_CITY"].as<String>();
    if (doc.containsKey("WAPI_LAT")) weatherConfig.latitude = doc["WAPI_LAT"];
    if (doc.containsKey("WAPI_LON")) weatherConfig.longitude = doc["WAPI_LON"];
    if (doc.containsKey("WAPI_STA")) weatherConfig.stationId = doc["WAPI_STA"].as<String>();
    if (doc.containsKey("WAPI_INT")) weatherConfig.updateInterval = doc["WAPI_INT"];
    if (doc.containsKey("WAPI_OTEMP")) weatherConfig.showOutdoorTemp = doc["WAPI_OTEMP"];
    if (doc.containsKey("WAPI_OHUM")) weatherConfig.showOutdoorHumidity = doc["WAPI_OHUM"];
    if (doc.containsKey("WAPI_PRES")) weatherConfig.showPressure = doc["WAPI_PRES"];
    if (doc.containsKey("WAPI_AQI")) weatherConfig.showAirQuality = doc["WAPI_AQI"];
    if (doc.containsKey("WAPI_ITEMP")) weatherConfig.showIndoorTemp = doc["WAPI_ITEMP"];
    if (doc.containsKey("WAPI_IHUM")) weatherConfig.showIndoorHumidity = doc["WAPI_IHUM"];
    if (doc.containsKey("WAPI_OTCOL")) weatherConfig.outdoorTempColor = doc["WAPI_OTCOL"];
    if (doc.containsKey("WAPI_OHCOL")) weatherConfig.outdoorHumColor = doc["WAPI_OHCOL"];
    if (doc.containsKey("WAPI_PCOL")) weatherConfig.pressureColor = doc["WAPI_PCOL"];
    if (doc.containsKey("WAPI_AQCOL")) weatherConfig.aqiColor = doc["WAPI_AQCOL"];
    if (doc.containsKey("WAPI_OTDUR")) weatherConfig.outdoorTempDuration = doc["WAPI_OTDUR"];
    if (doc.containsKey("WAPI_OHDUR")) weatherConfig.outdoorHumDuration = doc["WAPI_OHDUR"];
    if (doc.containsKey("WAPI_PDUR")) weatherConfig.pressureDuration = doc["WAPI_PDUR"];
    if (doc.containsKey("WAPI_AQDUR")) weatherConfig.aqiDuration = doc["WAPI_AQDUR"];
    if (doc.containsKey("WAPI_UV")) weatherConfig.showUV = doc["WAPI_UV"];
    if (doc.containsKey("WAPI_UVCOL")) weatherConfig.uvColor = doc["WAPI_UVCOL"];
    if (doc.containsKey("WAPI_UVDUR")) weatherConfig.uvDuration = doc["WAPI_UVDUR"];

    // Save to NVS
    saveSettings();
    return true;
}
