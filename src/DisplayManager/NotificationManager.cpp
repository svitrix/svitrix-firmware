/**
 * @file NotificationManager.cpp
 * @brief Notification and indicator implementation.
 *
 * Extracted from DisplayManager_Notifications.cpp + DisplayManager_Indicators.cpp
 * as part of Phase 11.2 (split DisplayManager_ god-object).
 *
 * Accesses shared globals (notifications deque, ui, currentApp) via
 * DisplayManager_internal.h — unchanged from the original code.
 */
#include "NotificationManager.h"
#include "DisplayManager_internal.h"
#include "Globals.h"
#include "INotifier.h"
#include "IPeripheryProvider.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <HTTPClient.h>
#include "Overlays.h"
#include "effects.h"
#include "ColorUtils.h"
#include "Functions.h"

// ═══════════════════════════════════════════════════════════════════════
// Dependency injection
// ═══════════════════════════════════════════════════════════════════════

void NotificationManager_::setDependencies(INotifier *n, IPeripheryProvider *p)
{
    notifier_ = n;
    periphery_ = p;
}

// ═══════════════════════════════════════════════════════════════════════
// Notifications (from DisplayManager_Notifications.cpp)
// ═══════════════════════════════════════════════════════════════════════

/// Parses a notification JSON and pushes it onto the notification stack.
/// Supports text, icon (file/base64), colors, effects, sound, charts, progress bar,
/// hold mode, wakeup, and client forwarding (MQTT or HTTP).
/// @param source  0=MQTT, 1=HTTP — determines how client forwarding works.
/// @param json    Full notification JSON payload.
bool NotificationManager_::generateNotification(uint8_t source, const char *json)
{
    // source: 0=MQTT, 1=HTTP
    DynamicJsonDocument doc(6144);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        DEBUG_PRINTLN(error.c_str());
        doc.clear();
        return false;
    }

    Notification newNotification;
    JsonObject obj = doc.as<JsonObject>();

    // --- Shared parsing (common with generateCustomPage) ---
    newNotification.background = parseBackground(obj);
    parseProgressBar(obj, newNotification.progress, newNotification.pColor, newNotification.pbColor);
    parseChartData(obj, newNotification.barData, newNotification.barSize, newNotification.lineData, newNotification.lineSize, newNotification.barBG);
    parseGradient(obj, newNotification.gradient);
    parseCommonAppFields(obj, newNotification.effect, newNotification.overlay,
                         newNotification.rainbow, newNotification.pushIcon, newNotification.textCase,
                         newNotification.iconOffset, newNotification.textOffset, newNotification.scrollSpeed,
                         newNotification.topText, newNotification.fade, newNotification.blink, newNotification.center,
                         newNotification.noScrolling, newNotification.repeat, newNotification.drawInstructions);
    newNotification.color = parseAppColor(obj);
    parseTextOrFragments(obj, newNotification.text, newNotification.colors, newNotification.fragments, newNotification.color);

    // --- Notification-specific fields ---
    newNotification.loopSound = doc.containsKey("loopSound") ? doc["loopSound"].as<bool>() : false;
    newNotification.sound = doc.containsKey("sound") ? doc["sound"].as<String>() : "";
    newNotification.rtttl = doc.containsKey("rtttl") ? doc["rtttl"].as<String>() : "";
    newNotification.duration = doc.containsKey("duration") ? doc["duration"].as<long>() * 1000 : appConfig.timePerApp;
    newNotification.hold = doc.containsKey("hold") ? doc["hold"].as<bool>() : false;
    newNotification.wakeup = doc.containsKey("wakeup") ? doc["wakeup"].as<bool>() : false;

    // Scroll state init
    newNotification.scrollposition = 9 + newNotification.textOffset;
    newNotification.iconWasPushed = false;
    newNotification.iconPosition = 0;
    newNotification.scrollDelay = 0;

    // Icon handling (Notification looks up files from LittleFS)
    if (doc.containsKey("icon"))
    {
        String iconValue = doc["icon"].as<String>();

        if (iconValue.length() > 64)
        {
            decodeBase64Icon(iconValue, newNotification.jpegDataBuffer);
            newNotification.isGif = false;
        }
        else
        {
            newNotification.jpegDataBuffer.clear();
            if (LittleFS.exists("/ICONS/" + iconValue + ".jpg"))
            {
                newNotification.isGif = false;
                newNotification.icon = LittleFS.open("/ICONS/" + iconValue + ".jpg");
            }
            else if (LittleFS.exists("/ICONS/" + iconValue + ".gif"))
            {
                newNotification.isGif = true;
                newNotification.icon = LittleFS.open("/ICONS/" + iconValue + ".gif");
            }
            else
            {
                fs::File nullPointer;
                newNotification.icon = nullPointer;
                newNotification.isGif = false;
            }
        }
    }
    else
    {
        fs::File nullPointer;
        newNotification.icon = nullPointer;
        newNotification.jpegDataBuffer.clear();
        newNotification.isGif = false;
    }

    if (doc.containsKey("clients"))
    {
        JsonArray ClientNames = doc["clients"];
        doc.remove("clients");
        String modifiedJson;
        serializeJson(doc, modifiedJson);
        for (JsonVariant c : ClientNames)
        {
            String client = c.as<String>();
            if (source == 0)
            {
                if (notifier_)
                    notifier_->rawPublish(client.c_str(), "notify", modifiedJson.c_str());
            }
            else
            {
                HTTPClient http;
                http.setConnectTimeout(200);
                http.setTimeout(300);
                http.begin("http://" + client + "/api/notify");
                http.POST(modifiedJson);
                http.end();
            }
        }
    }
    newNotification.startime = millis();
    currentApp = "Notification";
    if (notifier_)
        notifier_->setCurrentApp(currentApp);

    bool stack = doc.containsKey("stack") ? doc["stack"] : true;

    if (stack)
    {
        notifications.push_back(newNotification);
    }
    else
    {
        if (notifications.empty())
        {
            notifications.push_back(newNotification);
        }
        else
        {
            notifications[0] = newNotification;
        }
    }

    doc.clear();
    return true;
}

/// Dismisses the front notification: closes its icon file, stops sound,
/// and shifts the next notification (if stacked) to the front.
/// Restores matrixOff brightness if the dismissed notification had wakeup=true.
void NotificationManager_::dismissNotify()
{
    bool wakeup = false;
    if (!notifications.empty())
    {
        if (notifications.size() >= 2)
        {
            notifications[1].startime = millis();
        }
        wakeup = notifications[0].wakeup;
        notifications[0].icon.close();
        notifications.pop_front();
        if (periphery_)
            periphery_->stopSound();
    }
    if (notifications.empty())
    {
        if (wakeup && displayConfig.matrixOff)
        {
            matrix->setBrightness(0);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════
// Indicators (from DisplayManager_Indicators.cpp)
// ═══════════════════════════════════════════════════════════════════════

void NotificationManager_::setIndicator1Color(uint32_t color)
{
    ui->setIndicator1Color(color);
}

void NotificationManager_::setIndicator1State(bool state)
{
    ui->setIndicator1State(state);
}

void NotificationManager_::setIndicator2Color(uint32_t color)
{
    ui->setIndicator2Color(color);
}

void NotificationManager_::setIndicator2State(bool state)
{
    ui->setIndicator2State(state);
}

void NotificationManager_::setIndicator3Color(uint32_t color)
{
    ui->setIndicator3Color(color);
}

void NotificationManager_::setIndicator3State(bool state)
{
    ui->setIndicator3State(state);
}

/// Parses indicator JSON command for indicator 1-3.
/// Empty/"{}" clears the indicator. Otherwise expects {"color":..., "blink":N, "fade":N}.
/// Color=0 turns the indicator off; any other color turns it on.
/// Publishes the new indicator state via MQTT after applying changes.
bool NotificationManager_::indicatorParser(uint8_t indicator, const char *json)
{
    if (strcmp(json, "") == 0 || strcmp(json, "{}") == 0)
    {
        setIndicatorState(indicator, false);
        setIndicatorFade(indicator, 0);
        setIndicatorBlink(indicator, 0);
        if (notifier_)
            notifier_->setIndicatorState(indicator, getIndicatorState(indicator), getIndicatorColor(indicator));
        return true;
    }

    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return false;

    if (doc.containsKey("color"))
    {
        uint32_t col = getColorFromJsonVariant(doc["color"], colorConfig.textColor);
        if (col > 0)
        {
            setIndicatorState(indicator, true);
            setIndicatorColor(indicator, col);
        }
        else
        {
            setIndicatorState(indicator, false);
        }
    }

    setIndicatorBlink(indicator, doc.containsKey("blink") ? doc["blink"].as<int>() : 0);
    setIndicatorFade(indicator, doc.containsKey("fade") ? doc["fade"].as<int>() : 0);

    doc.clear();
    if (notifier_)
        notifier_->setIndicatorState(indicator, getIndicatorState(indicator), getIndicatorColor(indicator));
    return true;
}
