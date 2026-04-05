/**
 * @file DisplayManager_CustomApps.cpp
 * @brief Custom app lifecycle, app vector management, and persistence.
 *
 * Handles the full lifecycle of user-defined apps: JSON parsing
 * (generateCustomPage), callback slot allocation (pushCustomApp),
 * LittleFS persistence (load/save/delete), lifetime expiry, MQTT
 * placeholder subscriptions, and app vector reordering.
 * Also manages native app registration (Time, Date, Temp, Hum, Bat)
 * and app switching/navigation commands.
 */
#include "DisplayManager.h"
#include "DisplayManager_internal.h"
#include "Globals.h"
#include "INotifier.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "Apps.h"
#include "Overlays.h"
#include "effects.h"
#include "ColorUtils.h"
#include "Functions.h"
#include "base64.hpp"
#include "AppRegistry.h"

/// Creates a lambda callback for a new custom app and inserts it into the Apps vector.
/// @param name  Unique app identifier (used as key in customApps map).
/// @param position  Insert position in the Apps vector, or -1 to append.
void pushCustomApp(String name, int position)
{
    if (customApps.count(name) == 0)
    {
        AppCallback callback = [name](FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state,
                                      int16_t x, int16_t y, GifPlayer *gifPlayer)
        {
            ShowCustomApp(name, matrix, state, x, y, gifPlayer);
        };

        auto entry = std::make_pair(name, callback);

        if (position < 0) // Insert at the end of the vector
        {
            Apps.push_back(entry);
        }
        else if (position < (int)Apps.size()) // Insert at a specific position
        {
            Apps.insert(Apps.begin() + position, entry);
        }
        else // Invalid position, Insert at the end of the vector
        {
            Apps.push_back(entry);
        }

        ui->setApps(Apps);
        DisplayManager.setAutoTransition(true);
    }
}

/// Deletes the persisted JSON file for a custom app from LittleFS (/CUSTOMAPPS/<name>.json).
bool deleteCustomAppFile(const String& name)
{
    // Create the file name based on the app name
    String fileName = "/CUSTOMAPPS/" + name + ".json";

    // Check if the file exists
    if (!LittleFS.exists(fileName))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN("File does not exist: " + fileName + ". No need to delete it");
        return false;
    }

    // Delete the file
    if (LittleFS.remove(fileName))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN("File removed successfully: " + fileName);
        return true;
    }
    else
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN("Failed to remove file: " + fileName);
        return false;
    }
}

/// Fully removes a custom app: erases from Apps vector, customApps map, and LittleFS.
/// Uses prefix matching to also remove multi-page variants (e.g. "myapp0", "myapp1").
/// @param setApps  If true, immediately updates the UI app list.
void removeCustomAppFromApps(const String& name, bool setApps)
{
    // Remove apps from Apps list
    auto it = Apps.begin();
    while (it != Apps.end())
    {
        if (it->first.startsWith(name))
        {
            it = Apps.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Remove apps from customApps map
    auto mapIt = customApps.begin();
    while (mapIt != customApps.end())
    {
        if (mapIt->first.startsWith(name))
        {
            mapIt = customApps.erase(mapIt);
        }
        else
        {
            ++mapIt;
        }
    }

    if (setApps)
        ui->setApps(Apps);
    DisplayManager.setAutoTransition(true);
    deleteCustomAppFile(name);
    DisplayManager.setAppTime(appConfig.timePerApp);
}

/// Parses a JSON array of text fragments [{t:"hello", c:"#FF0000"}, ...] into parallel
/// color and text vectors. Fragments without a "c" key inherit standardColor.
bool parseFragmentsText(const JsonArray& fragmentArray, std::vector<uint32_t>& colors, std::vector<String>& fragments, uint32_t standardColor)
{
    colors.clear();
    fragments.clear();

    for (JsonObject fragmentObj : fragmentArray)
    {
        String textFragment = fragmentObj["t"];
        uint32_t color;
        if (fragmentObj.containsKey("c"))
        {
            auto fragColor = fragmentObj["c"];
            color = getColorFromJsonVariant(fragColor, standardColor);
        }
        else
        {
            color = standardColor;
        }

        fragments.push_back(textFragment);
        colors.push_back(color);
    }
    return true;
}

/// Scans text for {{topic}} placeholders and subscribes to each MQTT topic found.
void subscribeToPlaceholders(String text)
{
    int start = 0;
    while ((start = text.indexOf("{{", start)) != -1)
    {
        int end = text.indexOf("}}", start);
        if (end == -1)
        {
            break;
        }
        String placeholder = text.substring(start + 2, end);
        String topic = placeholder;

        DisplayManager.subscribeViaMqtt(topic.c_str());

        start = end + 2;
    }
}

/// Resets scroll/icon state on all custom apps except the currently visible one.
/// Called after each app transition to prepare apps for their next display cycle.
void ResetCustomApps()
{
    if (customApps.empty())
    {
        return;
    }

    for (auto it = customApps.begin(); it != customApps.end(); ++it)
    {
        CustomApp& app = it->second;
        if (app.name != currentCustomApp)
        {
            app.iconWasPushed = false;
            app.scrollposition = (app.icon ? 9 : 0) + app.textOffset;
            app.iconPosition = 0;
            app.scrollDelay = 0;
            app.currentRepeat = 0;
            app.icon.close();
            app.currentFrame = 0;
        }
    }
}

/// Checks if the custom app at the given position has exceeded its lifetime.
/// lifetimeMode 0: removes the app entirely. lifetimeMode 1: marks lifeTimeEnd flag.
void checkLifetime(uint8_t pos)
{
    if (customApps.empty())
    {
        return;
    }

    if (pos >= Apps.size())
    {
        pos = 0;
    }

    String appName = Apps[pos].first;
    auto appIt = customApps.find(appName);

    if (appIt != customApps.end())
    {
        CustomApp& app = appIt->second;

        if (app.lifetime > 0 && (millis() - app.lastUpdate) / 1000 >= app.lifetime)
        {
            if (app.lifetimeMode == 0)
            {
                if (systemConfig.debugMode)
                    DEBUG_PRINTLN("Removing " + appName + " -> Lifetime over");
                removeCustomAppFromApps(appName, false);

                if (systemConfig.debugMode)
                    DEBUG_PRINTLN("Set new Apploop");
                ui->setApps(Apps);
            }
            else if (app.lifetimeMode == 1)
            {
                app.lifeTimeEnd = true;
            }
        }
    }
}

/// Returns the native app pair (name, callback) for built-in apps: Time, Date, Temperature, Humidity, Battery.
/// Returns ("", nullptr) if the name doesn't match any native app.
std::pair<String, AppCallback> getNativeAppByName(const String& appName)
{
    if (appName == "Time")
    {
        return std::make_pair("Time", TimeApp);
    }
    else if (appName == "Date")
    {
        return std::make_pair("Date", DateApp);
    }
    else if (appName == "Temperature")
    {
        return std::make_pair("Temperature", TempApp);
    }
    else if (appName == "Humidity")
    {
        return std::make_pair("Humidity", HumApp);
    }
#ifdef ULANZI
    else if (appName == "Battery")
    {
        return std::make_pair("Battery", BatApp);
    }
#endif
    return std::make_pair("", nullptr);
}

/// Top-level parser for custom app JSON. Handles empty payload (app removal),
/// single object, or array of objects (multi-page apps: name0, name1, ...).
/// @param preventSave  If true, skips LittleFS persistence (used during loadCustomApps).
bool DisplayManager_::parseCustomPage(const String& name, const char *json, bool preventSave)
{
    if ((strcmp(json, "") == 0) || (strcmp(json, "{}") == 0))
    {
        removeCustomAppFromApps(name, true);
        return true;
    }

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        DEBUG_PRINTLN(error.c_str());
        doc.clear();
        return false;
    }

    if (doc.is<JsonObject>())
    {
        JsonObject rootObj = doc.as<JsonObject>();
        return generateCustomPage(name, rootObj, preventSave);
    }
    else if (doc.is<JsonArray>())
    {
        JsonArray customPagesArray = doc.as<JsonArray>();
        int cpIndex = 0;
        for (JsonObject customPageObject : customPagesArray)
        {
            generateCustomPage(name + String(cpIndex), customPageObject, preventSave);
            ++cpIndex;
        }
    }

    doc.clear();
    return true;
}

/// Creates or updates a custom app from a parsed JSON object.
/// Populates all CustomApp fields: text, icon (file/base64/GIF), colors, effects,
/// chart data, progress bar, scroll settings, lifetime, and draw instructions.
/// The app is inserted into the Apps vector and customApps map.
bool DisplayManager_::generateCustomPage(const String& name, JsonObject doc, bool preventSave)
{
    // pushCustomApp must be called BEFORE customApps[name] — it checks
    // customApps.count(name)==0 to decide whether to allocate a callback slot.
    int pos = doc.containsKey("pos") ? doc["pos"].as<uint8_t>() : -1;
    pushCustomApp(name, pos - 1);

    // Work directly with the map entry — operator[] default-constructs if missing.
    // This avoids copying the ~1.2KB CustomApp struct twice per update.
    CustomApp& customApp = customApps[name];

    // Save to LittleFS if requested
    if (doc.containsKey("save") && preventSave == false)
    {
        bool saveApp = doc["save"].as<bool>();
        if (saveApp)
        {
            if (!LittleFS.exists("/CUSTOMAPPS"))
            {
                LittleFS.mkdir("/CUSTOMAPPS");
            }

            File file = LittleFS.open("/CUSTOMAPPS/" + name + ".json", "w");
            if (!file)
            {
                return false;
            }

            serializeJson(doc, file);
            file.close();
        }
    }

    // --- Shared parsing (common with generateNotification) ---
    customApp.background = parseBackground(doc);
    parseProgressBar(doc, customApp.progress, customApp.pColor, customApp.pbColor);
    parseChartData(doc, customApp.barData, customApp.barSize, customApp.lineData, customApp.lineSize, customApp.barBG);
    parseGradient(doc, customApp.gradient);
    parseCommonAppFields(doc, customApp.effect, customApp.overlay,
                         customApp.rainbow, customApp.pushIcon, customApp.textCase,
                         customApp.iconOffset, customApp.textOffset, customApp.scrollSpeed,
                         customApp.topText, customApp.fade, customApp.blink, customApp.center,
                         customApp.noScrolling, customApp.repeat, customApp.drawInstructions);

    // Color with hasCustomColor tracking (CustomApp-specific)
    if (doc.containsKey("color"))
    {
        customApp.hasCustomColor = true;
        customApp.color = parseAppColor(doc);
    }
    else
    {
        customApp.hasCustomColor = false;
        customApp.color = colorConfig.textColor;
    }

    // Text/fragments with MQTT placeholder subscription (CustomApp-specific)
    parseTextOrFragments(doc, customApp.text, customApp.colors, customApp.fragments, customApp.color);
    if (customApp.fragments.empty() && !customApp.text.isEmpty())
    {
        subscribeToPlaceholders(customApp.text);
    }

    // --- CustomApp-specific fields ---
    customApp.name = name;
    customApp.duration = doc.containsKey("duration") ? doc["duration"].as<long>() * 1000 : 0;
    customApp.bounce = doc.containsKey("bounce") ? doc["bounce"].as<bool>() : false;

    if (doc.containsKey("lifetime"))
    {
        customApp.lifetime = doc["lifetime"];
        customApp.lifetimeMode = doc.containsKey("lifetimeMode") ? doc["lifetimeMode"] : 0;
    }
    else
    {
        customApp.lifetime = 0;
    }

    // Icon handling (CustomApp tracks iconName for change detection)
    if (doc.containsKey("icon"))
    {
        String newIconName = doc["icon"].as<String>();

        if (newIconName.length() > 64)
        {
            decodeBase64Icon(newIconName, customApp.jpegDataBuffer);
            customApp.isGif = false;
            customApp.icon.close();
            customApp.iconName = "";
            customApp.iconPosition = 0;
            customApp.currentFrame = 0;
        }
        else if (customApp.iconName != newIconName)
        {
            customApp.jpegDataBuffer.clear();
            customApp.iconName = newIconName;
            customApp.iconNotFound = false;
            customApp.icon.close();
            customApp.iconPosition = 0;
            customApp.currentFrame = 0;
        }
    }
    else
    {
        customApp.jpegDataBuffer.clear();
        customApp.icon.close();
        customApp.iconName = "";
        customApp.iconPosition = 0;
        customApp.currentFrame = 0;
    }

    if (currentCustomApp != name)
    {
        customApp.scrollposition = 9 + customApp.textOffset;
    }

    customApp.lastUpdate = millis();
    customApp.lifeTimeEnd = false;

    return true;
}

/// Loads all persisted custom apps from /CUSTOMAPPS/*.json on LittleFS at boot.
void DisplayManager_::loadCustomApps()
{
    File root = LittleFS.open("/CUSTOMAPPS");

    if (!root)
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN("/CUSTOMAPPS is not a directory");
        root.close();
        return;
    }

    File file = root.openNextFile();

    while (file)
    {
        if (file.isDirectory())
        {
            file = root.openNextFile();
            continue;
        }

        String fileName = file.name();
        String json = file.readString();
        file.close();

        String name = fileName.substring(fileName.lastIndexOf('/') + 1, fileName.lastIndexOf('.')); // remove path and .json extension
        parseCustomPage(name, json.c_str(), true);

        file = root.openNextFile();
    }
    root.close();
}

/// Registers or removes built-in apps (Time, Date, Temperature, Humidity, Battery)
/// based on current appConfig show flags. Called after settings change.
void DisplayManager_::loadNativeApps()
{
    // Define a helper function to check and update an app
    auto updateApp = [&](const String& name, AppCallback callback, bool show, size_t position)
    {
        auto it = std::find_if(Apps.begin(), Apps.end(), [&](const std::pair<String, AppCallback>& app)
                               { return app.first == name; });
        if (it != Apps.end())
        {
            if (!show)
            {
                Apps.erase(it);
            }
        }
        else
        {
            if (show)
            {
                if (position >= Apps.size())
                {
                    Apps.push_back(std::make_pair(name, callback));
                }
                else
                {
                    Apps.insert(Apps.begin() + position, std::make_pair(name, callback));
                }
            }
        }
    };

    updateApp("Time", TimeApp, appConfig.showTime, 0);
    updateApp("Date", DateApp, appConfig.showDate, 1);

    if (sensorConfig.sensorReading)
    {
        updateApp("Temperature", TempApp, appConfig.showTemp, 2);
        updateApp("Humidity", HumApp, appConfig.showHum, 3);
    }
#ifdef ULANZI
    updateApp("Battery", BatApp, appConfig.showBat, 4);
#endif

    ui->setApps(Apps);
    setAutoTransition(true);
}

/// Switches to a named app. JSON: {"name":"AppName", "fast":true/false}.
/// fast=true does instant switch, fast=false uses animated transition.
bool DisplayManager_::switchToApp(const char *json)
{
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        doc.clear();
        return false;
    }

    String name = doc["name"].as<String>();
    bool fast = doc["fast"] | false;
    doc.clear();
    int index = findAppIndexByName(name);
    if (index > -1)
    {
        if (fast)
        {
            ui->switchToApp(index);
            return true;
        }
        else
        {
            ui->transitionToApp(index);
            return true;
        }
    }
    else
    {
        return false;
    }
}

/// Updates the app vector from JSON: show/hide/reposition apps.
/// Accepts a single object or array of [{name, show, pos}, ...].
/// Persists the new order and publishes the updated loop via MQTT.
void DisplayManager_::updateAppVector(const char *json)
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("New apps vector received"));
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(json);
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        doc.clear();
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Failed to parse json"));
        return;
    }

    JsonArray appArray;
    DynamicJsonDocument wrapDoc(0); // only used if input is a single object
    if (doc.is<JsonObject>())
    {
        // Wrap single object in a temporary array without destroying the original
        wrapDoc = DynamicJsonDocument(2048);
        JsonArray tempArray = wrapDoc.to<JsonArray>();
        tempArray.add(doc.as<JsonObject>());
        appArray = tempArray;
    }
    else if (doc.is<JsonArray>())
    {
        appArray = doc.as<JsonArray>();
    }

    for (JsonObject appObj : appArray)
    {
        String appName = appObj["name"].as<String>();
        bool show = appObj["show"].as<bool>();
        int position = appObj.containsKey("pos") ? appObj["pos"].as<int>() : Apps.size();

        auto appIt = std::find_if(Apps.begin(), Apps.end(), [&appName](const std::pair<String, AppCallback>& app)
                                  { return app.first == appName; });

        std::pair<String, AppCallback> nativeApp = getNativeAppByName(appName);

        if (!show)
        {
            if (appIt != Apps.end())
            {
                Apps.erase(appIt);
            }
        }
        else
        {
            if (nativeApp.second != nullptr)
            {
                if (appIt != Apps.end())
                {
                    Apps.erase(appIt);
                }
                position = position < 0 ? 0 : position >= Apps.size() ? Apps.size()
                                                                      : position;
                Apps.insert(Apps.begin() + position, nativeApp);
            }
            else
            {
                if (appIt != Apps.end() && appObj.containsKey("pos"))
                {
                    std::pair<String, AppCallback> app = *appIt;
                    Apps.erase(appIt);
                    position = position < 0 ? 0 : position >= Apps.size() ? Apps.size()
                                                                          : position;
                    Apps.insert(Apps.begin() + position, app);
                }
            }
        }
    }

    // Set the updated apps vector in the UI and save settings
    ui->setApps(Apps);
    saveSettings();
    sendAppLoop();
    setAutoTransition(appConfig.autoTransition);
    doc.clear();
}

String DisplayManager_::getAppsAsJson()
{
    std::vector<String> names;
    names.reserve(Apps.size());
    for (size_t i = 0; i < Apps.size(); i++)
    {
        names.push_back(Apps[i].first);
    }
    return serializeAppList(names);
}

void DisplayManager_::sendAppLoop()
{
    if (notifier_)
        notifier_->publish("stats/loop", getAppsAsJson().c_str());
}

/// Updates the text color for all custom apps that don't have a custom color set.
void DisplayManager_::setCustomAppColors(uint32_t color)
{
    for (auto it = customApps.begin(); it != customApps.end(); ++it)
    {
        CustomApp& app = it->second;
        if (!app.hasCustomColor)
        {
            app.color = color;
        }
    }
}

/// Returns JSON array of all apps with their icon names: [{"name":"Time","icon":""},...]
String DisplayManager_::getAppsWithIcon()
{
    DynamicJsonDocument jsonDocument(1024);
    JsonArray jsonArray = jsonDocument.to<JsonArray>();
    for (const auto& app : Apps)
    {
        JsonObject appObject = jsonArray.createNestedObject();
        appObject["name"] = app.first;

        CustomApp *customApp = getCustomAppByName(app.first);
        if (customApp != nullptr)
        {
            appObject["icon"] = customApp->iconName;
        }
    }
    String jsonString;
    serializeJson(jsonArray, jsonString);
    return jsonString;
}

/// Reorders the app vector to match the given JSON array of app names.
/// Apps not in the array are dropped. UI state is force-reset after reorder.
void DisplayManager_::reorderApps(const String& jsonString)
{
    StaticJsonDocument<2048> jsonDocument;
    DeserializationError error = deserializeJson(jsonDocument, jsonString);
    if (error)
    {
        return;
    }

    JsonArray jsonArray = jsonDocument.as<JsonArray>();
    std::vector<std::pair<String, AppCallback>> reorderedApps;
    for (const String& appName : jsonArray)
    {
        for (const auto& app : Apps)
        {
            if (app.first == appName)
            {
                reorderedApps.push_back(app);
                break;
            }
        }
    }
    Apps = reorderedApps;
    ui->setApps(Apps);
    ui->forceResetState();
}
