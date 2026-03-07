/**
 * @file MQTTManager_StateUpdates.cpp
 * @brief Outgoing state updates: stats publishing, button/indicator HA sync.
 *
 * Contains sendStats (periodic sensor value updates to HA),
 * sendButton (hardware button state to HA binary sensors),
 * setIndicatorState (indicator LED color/state to HA lights),
 * and setCurrentApp (current app name publishing).
 */
#include "MQTTManager_internal.h"

// ── Stats publishing ────────────────────────────────────────────────

/// Collect system stats and update all HA sensor values.
/// When HA discovery is enabled and connected, pushes: battery,
/// temperature, humidity, illuminance, brightness mode/level,
/// matrix color/state, WiFi RSSI, free RAM, uptime, transition
/// state, and IP address to their respective HA sensor entities.
/// Always publishes the aggregated stats JSON to the MQTT stats topic.
void MQTTManager_::sendStats()
{
    if (haConfig.discovery && mqtt.isConnected())
    {
        char buffer[8];
        snprintf(buffer, 5, "%d", batteryConfig.percent);
        battery->setValue(buffer);
        if (sensorConfig.sensorReading)
        {
            snprintf(buffer, sizeof(buffer), "%.*f", timeConfig.tempDecimalPlaces, sensorConfig.currentTemp);
            temperature->setValue(buffer);
            snprintf(buffer, 5, "%.0f", sensorConfig.currentHum);
            humidity->setValue(buffer);
        }

        snprintf(buffer, 5, "%.0f", sensorConfig.currentLux);
        illuminance->setValue(buffer);
        BriMode->setState(brightnessConfig.autoBrightness, false);
        Matrix->setBrightness(brightnessConfig.brightness);
        Matrix->setState(!displayConfig.matrixOff, false);
        HALight::RGBColor color;
        color.isSet = true;
        color.red = (colorConfig.textColor >> 16) & 0xFF;
        color.green = (colorConfig.textColor >> 8) & 0xFF;
        color.blue = colorConfig.textColor & 0xFF;
        Matrix->setRGBColor(color);
        int8_t rssiValue = WiFi.RSSI();
        char rssiString[4];
        snprintf(rssiString, sizeof(rssiString), "%d", rssiValue);
        strength->setValue(rssiString);

        char rambuffer[10];
        int freeHeapBytes = ESP.getFreeHeap();
        itoa(freeHeapBytes, rambuffer, 10);
        ram->setValue(rambuffer);
        char uptimeStr[25];
        sprintf(uptimeStr, "%ld", mqttPeriphery_->readUptime());
        uptime->setValue(uptimeStr);
        transition->setState(appConfig.autoTransition, false);
        ipAddr->setValue(ServerManager.myIP.toString().c_str());
    }
    publish(StatsTopic, dmControl_->getStats().c_str());
}

// ── Current app publishing ──────────────────────────────────────────

/// Publish the current app name to HA and MQTT (deduplicated).
/// Skips publishing if the app name hasn't changed since last call.
/// @param appName Name of the currently active app.
void MQTTManager_::setCurrentApp(String appName)
{
    static String lastApp = "";

    if (lastApp == appName)
        return;

    if (systemConfig.debugMode)
        DEBUG_PRINTF("Publish current app %s", appName.c_str());
    if (haConfig.discovery && mqtt.isConnected())
        curApp->setValue(appName.c_str());

    publish("stats/currentApp", appName.c_str());
    lastApp = appName;
}

// ── Button state reporting ──────────────────────────────────────────

/// Publish hardware button press/release to HA binary sensors and MQTT.
/// Debounced: only publishes when state differs from last known per button.
/// @param btn   Button index: 0 = left, 1 = select/middle, 2 = right.
/// @param state true = pressed, false = released.
void MQTTManager_::sendButton(byte btn, bool state)
{
    static bool btn0State, btn1State, btn2State;

    switch (btn)
    {
    case 0:
        if (btn0State != state)
        {
            if (haConfig.discovery && mqtt.isConnected())
                btnleft->setState(state, false);
            btn0State = state;
            publish(ButtonLeftTopic, state ? State1 : State0);
        }
        break;
    case 1:
        if (btn1State != state)
        {
            if (haConfig.discovery && mqtt.isConnected())
                btnmid->setState(state, false);
            btn1State = state;
            publish(ButtonSelectTopic, state ? State1 : State0);
        }
        break;
    case 2:
        if (btn2State != state)
        {
            if (haConfig.discovery && mqtt.isConnected())
                btnright->setState(state, false);
            btn2State = state;
            publish(ButtonRightTopic, state ? State1 : State0);
        }
        break;
    default:
        break;
    }
}

// ── Indicator state reporting ───────────────────────────────────────

/// Update an indicator LED's color and on/off state in HA.
/// Only operates when HA discovery is enabled and MQTT is connected.
/// @param indicator Indicator number: 1 = top-right, 2 = middle-right, 3 = bottom-right.
/// @param state     true = on, false = off.
/// @param color     Packed RGB color as 0xRRGGBB.
void MQTTManager_::setIndicatorState(uint8_t indicator, bool state, uint32_t color)
{
    if (haConfig.discovery && mqtt.isConnected())
    {
        HALight::RGBColor c;
        c.isSet = true;
        c.red = (color >> 16) & 0xFF;
        c.green = (color >> 8) & 0xFF;
        c.blue = color & 0xFF;

        switch (indicator)
        {
        case 1:
            Indikator1->setRGBColor(c);
            Indikator1->setState(state);
            break;
        case 2:
            Indikator2->setRGBColor(c);
            Indikator2->setState(state);
            break;
        case 3:
            Indikator3->setRGBColor(c);
            Indikator3->setState(state);
            break;
        default:
            break;
        }
    }
}
