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
HAMqtt mqtt(espClient, device, 26); ///< ArduinoHA MQTT client (max 26 HA entities)

// ── HA entity pointers ──────────────────────────────────────────────
// Allocated in setup() when HA discovery is enabled; remain nullptr otherwise.

HALight *Matrix = nullptr;          ///< Main matrix light (brightness + RGB)
HALight *Indikator1 = nullptr;      ///< Top-right indicator light (RGB)
HALight *Indikator2 = nullptr;      ///< Middle-right indicator light (RGB)
HALight *Indikator3 = nullptr;      ///< Bottom-right indicator light (RGB)
HASelect *BriMode = nullptr;        ///< Brightness mode select (Manual / Auto)
HASelect *transEffect = nullptr;    ///< Transition effect select
HAButton *dismiss = nullptr;        ///< Dismiss notification button
HAButton *nextApp = nullptr;        ///< Next app button
HAButton *prevApp = nullptr;        ///< Previous app button
HAButton *doUpdate = nullptr;       ///< Start firmware update button
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

// ── HA entity ID buffers ────────────────────────────────────────────
// Each holds a unique HA entity ID built from MAC suffix (e.g. "abc123_mat").

long receivedMessages_;
bool connected;
char matID[40], ind1ID[40], ind2ID[40], ind3ID[40];
char briID[40], btnAID[40], btnBID[40], btnCID[40];
char appID[40], tempID[40], humID[40], luxID[40];
char verID[40], ramID[40], upID[40], sigID[40];
char btnLID[40], btnMID[40], btnRID[40];
char transID[40], doUpdateID[40], batID[40];
char myID[40], sSpeed[40], effectID[40], ipAddrID[40];

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
