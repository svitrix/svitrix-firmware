/**
 * @file MQTTManager.cpp
 * @brief Core: singleton, global variable definitions, connection lifecycle,
 *        tick loop, and publishing methods.
 *
 * Defines all shared global state (HA entity pointers, ID buffers,
 * transport objects) that other MQTTManager_*.cpp modules reference
 * via MQTTManager_internal.h extern declarations.
 */
#include "MQTTManager_internal.h"
#include <cassert>

// ── MQTT transport ──────────────────────────────────────────────────

WiFiClient espClient;               ///< TCP socket for the MQTT broker connection
HADevice device;                    ///< ArduinoHA device descriptor (MAC, name, version)
HAMqtt mqtt(espClient, device, HA_MAX_ENTITIES); ///< ArduinoHA MQTT client; capacity must exceed getTotalEntityCount()

// ── HA entity pointers ──────────────────────────────────────────────
// Allocated in setup() when HA discovery is enabled; remain nullptr otherwise.

HALight *Matrix = nullptr;        ///< Main matrix light (brightness + RGB)
HALight *Indikator1 = nullptr;    ///< Top-right indicator light (RGB)
HALight *Indikator2 = nullptr;    ///< Middle-right indicator light (RGB)
HALight *Indikator3 = nullptr;    ///< Bottom-right indicator light (RGB)
HASelect *BriMode = nullptr;      ///< Brightness mode select (Manual / Auto)
HASelect *transEffect = nullptr;  ///< Transition effect select
HASelect *bgEffect = nullptr;     ///< Background effect select
HAButton *dismiss = nullptr;      ///< Dismiss notification button
HAButton *nextApp = nullptr;      ///< Next app button
HAButton *prevApp = nullptr;      ///< Previous app button
HAButton *doUpdate = nullptr;     ///< Start firmware update button
HAButton *rebootBtn = nullptr;    ///< Reboot device button
HAButton *playSoundBtn = nullptr; ///< Play test sound button

// Audio controls
HASwitch *soundEnabled = nullptr;
HANumber *soundVolume = nullptr;
HASwitch *transition = nullptr;     ///< Auto-transition toggle
HASensor *battery = nullptr;        ///< Battery percentage (ULANZI only)
HASensor *temperature = nullptr;    ///< Temperature (°C)
HASensor *humidity = nullptr;       ///< Humidity (%)
HASensor *illuminance = nullptr;    ///< Ambient light (lx)
HASensor *uptime = nullptr;         ///< Device uptime (seconds)
HASensor *strength = nullptr;       ///< WiFi signal strength (dB)
HASensor *version = nullptr;        ///< Firmware version
HASensor *ram = nullptr;            ///< Free heap memory (bytes)
HASensor *curApp = nullptr;         ///< Current app name
HASensor *myOwnID = nullptr;        ///< Device MQTT prefix
HASensor *ipAddr = nullptr;         ///< Device IP address
HABinarySensor *btnleft = nullptr;  ///< Left hardware button state
HABinarySensor *btnmid = nullptr;   ///< Middle hardware button state
HABinarySensor *btnright = nullptr; ///< Right hardware button state

// Weather sensors (from WeatherAPI)
HASensor *outdoorTemp = nullptr;
HASensor *outdoorHum = nullptr;
HASensor *pressure = nullptr;
HASensor *aqi = nullptr;
HASensor *weatherCond = nullptr;
HASensor *uvIndex = nullptr;

// Night mode controls
HASwitch *nightModeSwitch = nullptr;
HANumber *nightBrightnessNum = nullptr;
HALight *nightColorLight = nullptr;
HASwitch *nightBlockSwitch = nullptr;

// App visibility switches
HASwitch *showTimeSwitch = nullptr;
HASwitch *showDateSwitch = nullptr;
HASwitch *showTempSwitch = nullptr;
HASwitch *showHumSwitch = nullptr;
HASwitch *showBatSwitch = nullptr;

// Display timing numbers
HANumber *timePerAppNum = nullptr;
HANumber *scrollSpeedNum = nullptr;
HANumber *timeDurationNum = nullptr;
HANumber *dateDurationNum = nullptr;

// Native app color lights
HALight *timeColorLight = nullptr;
HALight *dateColorLight = nullptr;
HALight *tempColorLight = nullptr;
HALight *humColorLight = nullptr;
HALight *batColorLight = nullptr;

// Weather app visibility switches
HASwitch *showOutTempSwitch = nullptr;
HASwitch *showOutHumSwitch = nullptr;
HASwitch *showPressureSwitch = nullptr;
HASwitch *showAqiSwitch = nullptr;
HASwitch *showUvSwitch = nullptr;

// ── HA entity ID buffers ────────────────────────────────────────────
// Each holds a unique HA entity ID built from MAC suffix (e.g. "abc123_mat").

long receivedMessages_;
bool connected;
char matID[40], ind1ID[40], ind2ID[40], ind3ID[40];
char briID[40], btnAID[40], btnBID[40], btnCID[40], rebootID[40], playSoundID[40];
char soundEnID[40], soundVolID[40];
char appID[40], tempID[40], humID[40], luxID[40];
char verID[40], ramID[40], upID[40], sigID[40];
char btnLID[40], btnMID[40], btnRID[40];
char transID[40], doUpdateID[40], batID[40];
char myID[40], sSpeed[40], effectID[40], ipAddrID[40];
char outTempID[40], outHumID[40], pressID[40], aqiID[40], weatherCondID[40], uvID[40];
char nightModeID[40], nightBriID[40], nightColID[40], nightBlockID[40];
char showTimeID[40], showDateID[40], showTempID[40], showHumID[40], showBatID[40];
char bgEffectID[40];
char timePerAppID[40], scrollSpeedID[40], timeDurID[40], dateDurID[40];
char timeColID[40], dateColID[40], tempColID[40], humColID[40], batColID[40];
char showOutTempID[40], showOutHumID[40], showPressID[40], showAqiID[40], showUvID[40];

unsigned long previousMillis_Stats;    ///< Timestamp of last stats publish (millis)
std::map<String, String> mqttValues;   ///< Cached values for subscribed external topics
std::vector<String> topicsToSubscribe; ///< Topics queued for subscription on next connect

// ── Display interfaces (set via setDisplay) ──────────────────────────

IDisplayControl *dmControl_ = nullptr;
IDisplayNavigation *dmNav_ = nullptr;
IDisplayNotifier *dmNotify_ = nullptr;

// ── Service interfaces (set via setServices) ─────────────────────────

ISound *mqttSound_ = nullptr;
IPower *mqttPower_ = nullptr;
IUpdater *mqttUpdater_ = nullptr;
IPeripheryProvider *mqttPeriphery_ = nullptr;

// ── Singleton ───────────────────────────────────────────────────────

MQTTManager_& MQTTManager_::getInstance()
{
    static MQTTManager_ instance;
    return instance;
}

MQTTManager_& MQTTManager = MQTTManager.getInstance();

void MQTTManager_::setDisplay(IDisplayControl *c, IDisplayNavigation *n, IDisplayNotifier *nt)
{
    assert(c && n && nt);
    dmControl_ = c;
    dmNav_ = n;
    dmNotify_ = nt;
}

bool MQTTManager_::hasDisplay() const
{
    return dmControl_ && dmNav_ && dmNotify_;
}

void MQTTManager_::setServices(ISound *s, IPower *p, IUpdater *u, IPeripheryProvider *pp)
{
    assert(s && p && u && pp);
    mqttSound_ = s;
    mqttPower_ = p;
    mqttUpdater_ = u;
    mqttPeriphery_ = pp;
}

bool MQTTManager_::hasServices() const
{
    return mqttSound_ && mqttPower_ && mqttUpdater_ && mqttPeriphery_;
}

// ── Connection lifecycle ────────────────────────────────────────────

/// Called by ArduinoHA when the MQTT broker connection is established.
/// Subscribes to all device topics, deferred external topics, and
/// publishes initial device state (version, effects, transitions).
void onMqttConnected()
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("MQTT Connected"));
    std::vector<String> topics = getSubscriptionTopics();
    for (size_t i = 0; i < topics.size(); i++)
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTF("Subscribe to topic %s", topics[i].c_str());
        mqtt.subscribe((mqttConfig.prefix + topics[i]).c_str());
        delay(30);
    }

    for (const auto& topic : topicsToSubscribe)
    {
        mqtt.subscribe(topic.c_str());
        if (systemConfig.debugMode)
            Serial.printf("Subscribed to topic %s\n", topic.c_str());
    }

    delay(200);
    if (haConfig.discovery)
    {
        myOwnID->setValue(mqttConfig.prefix.c_str());
        version->setValue(VERSION);

        // Publish initial state for interactive entities on (re)connect
        transition->setState(appConfig.autoTransition, true);
        BriMode->setState(brightnessConfig.autoBrightness, true);
        transEffect->setState(appConfig.transEffect, true);
        Matrix->setBrightness(brightnessConfig.brightness);
        Matrix->setState(!displayConfig.matrixOff, true);
        HALight::RGBColor color;
        color.isSet = true;
        color.red = (colorConfig.textColor >> 16) & 0xFF;
        color.green = (colorConfig.textColor >> 8) & 0xFF;
        color.blue = colorConfig.textColor & 0xFF;
        Matrix->setRGBColor(color);

        // Night mode initial state
        nightModeSwitch->setState(appConfig.nightMode, true);
        nightBrightnessNum->setState(appConfig.nightBrightness);
        HALight::RGBColor nightCol;
        nightCol.isSet = true;
        nightCol.red = (appConfig.nightColor >> 16) & 0xFF;
        nightCol.green = (appConfig.nightColor >> 8) & 0xFF;
        nightCol.blue = appConfig.nightColor & 0xFF;
        nightColorLight->setRGBColor(nightCol);
        nightBlockSwitch->setState(appConfig.nightBlockTransition, true);

        // Audio initial state
        soundEnabled->setState(audioConfig.soundActive, true);
        soundVolume->setState(audioConfig.soundVolume);

        // App visibility initial state
        showTimeSwitch->setState(appConfig.showTime, true);
        showDateSwitch->setState(appConfig.showDate, true);
        showTempSwitch->setState(appConfig.showTemp, true);
        showHumSwitch->setState(appConfig.showHum, true);
        showBatSwitch->setState(appConfig.showBat, true);

        // Background effect initial state (0 = None, 1-20 = effect index + 1)
        bgEffect->setState(displayConfig.backgroundEffect, true);

        // Display timing initial state
        timePerAppNum->setState(static_cast<float>(appConfig.timePerApp));
        scrollSpeedNum->setState(static_cast<float>(appConfig.scrollSpeed));
        timeDurationNum->setState(static_cast<float>(appConfig.timeDuration));
        dateDurationNum->setState(static_cast<float>(appConfig.dateDuration));

        // Native app colors initial state
        auto setColorLight = [](HALight *light, uint32_t color)
        {
            HALight::RGBColor c;
            c.isSet = true;
            c.red = (color >> 16) & 0xFF;
            c.green = (color >> 8) & 0xFF;
            c.blue = color & 0xFF;
            light->setCurrentRGBColor(c);
            light->setCurrentState(true);
        };
        setColorLight(timeColorLight, colorConfig.timeColor);
        setColorLight(dateColorLight, colorConfig.dateColor);
        setColorLight(tempColorLight, colorConfig.tempColor);
        setColorLight(humColorLight, colorConfig.humColor);
        setColorLight(batColorLight, colorConfig.batColor);

        // Weather app visibility initial state
        showOutTempSwitch->setState(weatherConfig.showOutdoorTemp, true);
        showOutHumSwitch->setState(weatherConfig.showOutdoorHumidity, true);
        showPressureSwitch->setState(weatherConfig.showPressure, true);
        showAqiSwitch->setState(weatherConfig.showAirQuality, true);
        showUvSwitch->setState(weatherConfig.showUV, true);
    }

    MQTTManager.publish("stats/effects", dmNav_->getEffectNames().c_str());
    MQTTManager.publish("stats/transitions", dmNav_->getTransitionNames().c_str());
    if (!haConfig.discovery)
    {
        MQTTManager.publish("stats/device", "online");
    }
    connected = true;
}

/// Register MQTT callbacks (message, connected), configure last-will,
/// and initiate the broker connection with or without credentials.
void connect()
{
    mqtt.onMessage(onMqttMessage);
    mqtt.onConnected(onMqttConnected);

    if (!haConfig.discovery)
    {
        static char topic[50];
        snprintf(topic, sizeof(topic), "%s/stats/device", mqttConfig.prefix.c_str());
        mqtt.setLastWill(topic, "offline", false);
    }

    if (mqttConfig.user == "" || mqttConfig.pass == "")
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Connecting to MQTT w/o login"));
        mqtt.begin(mqttConfig.host.c_str(), mqttConfig.port, nullptr, nullptr, systemConfig.hostname.c_str());
    }
    else
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Connecting to MQTT with login"));
        mqtt.begin(mqttConfig.host.c_str(), mqttConfig.port, mqttConfig.user.c_str(), mqttConfig.pass.c_str(), systemConfig.hostname.c_str());
    }
}

/// Subscribe to an external topic; stores "N/A" as initial value.
/// If already connected, subscribes immediately; otherwise defers.
bool MQTTManager_::subscribe(const char *topic)
{
    mqttValues[topic] = "N/A";
    if (mqtt.isConnected())
    {
        mqtt.subscribe(topic);
    }
    else
    {
        topicsToSubscribe.push_back(topic);
    }
    return true;
}

/// @return true if MQTT is connected, or if no host is configured (MQTT disabled).
bool MQTTManager_::isConnected()
{
    if (mqttConfig.host != "")
    {
        return mqtt.isConnected();
    }
    else
    {
        return true;
    }
}

/// Look up the cached value for a subscribed topic.
/// @return The last received value, or "N/A" if unknown.
String MQTTManager_::getValueForTopic(const String& topic)
{
    if (mqttValues.find(topic) != mqttValues.end())
    {
        return mqttValues[topic];
    }
    else
    {
        return "N/A";
    }
}

// ── Main loop ───────────────────────────────────────────────────────

/// Process MQTT messages and send stats at the configured interval.
/// Called from the main Arduino loop every frame.
void MQTTManager_::tick()
{
    if (mqttConfig.host != "")
    {
        mqtt.loop();
    }
    unsigned long currentMillis_Stats = millis();
    if ((currentMillis_Stats - previousMillis_Stats >= systemConfig.statsInterval) && (sensorConfig.sensorsStable))
    {
        previousMillis_Stats = currentMillis_Stats;
        sendStats();
    }
}

// ── Publishing ──────────────────────────────────────────────────────

/// Publish payload to "<mqttConfig.prefix>/<topic>". No-op if disconnected.
long MQTTManager_::getReceivedMessages() const
{
    return receivedMessages_;
}

void MQTTManager_::publish(const char *topic, const char *payload)
{
    if (!mqtt.isConnected())
        return;
    char result[100];
    snprintf(result, sizeof(result), "%s/%s", mqttConfig.prefix.c_str(), topic);
    mqtt.publish(result, payload, false);
}

/// Publish payload to "<prefix>/<topic>" with a custom prefix. No-op if disconnected.
void MQTTManager_::rawPublish(const char *prefix, const char *topic, const char *payload)
{
    if (!mqtt.isConnected())
        return;
    char result[100];
    snprintf(result, sizeof(result), "%s/%s", prefix, topic);
    mqtt.publish(result, payload, false);
}

/// Start a streaming (chunked) MQTT publish for large payloads.
void MQTTManager_::beginPublish(const char *topic, unsigned int plength, boolean retained)
{
    mqtt.beginPublish(topic, plength, retained);
}

/// Write a data chunk to the current streaming publish.
void MQTTManager_::writePayload(const char *data, const uint16_t length)
{
    mqtt.writePayload(data, length);
}

/// Finalize the current streaming publish.
void MQTTManager_::endPublish()
{
    mqtt.endPublish();
}
