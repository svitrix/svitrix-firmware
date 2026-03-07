/**
 * @file MQTTManager.h
 * @brief MQTT communication and Home Assistant integration manager.
 *
 * Singleton manager that handles all MQTT communication:
 *   - Broker connection lifecycle (connect, reconnect, last-will)
 *   - Incoming message dispatch via MessageRouter
 *   - Home Assistant auto-discovery entity registration
 *   - HA callback handling (lights, buttons, switches, selects)
 *   - Periodic stats publishing and sensor value updates
 *   - Hardware button state reporting to HA binary sensors
 *
 * Split across multiple files:
 *   MQTTManager.cpp              — core: singleton, connection, tick, publish
 *   MQTTManager_Messages.cpp     — incoming message reception and dispatch
 *   MQTTManager_Callbacks.cpp    — 7 ArduinoHA callback handlers
 *   MQTTManager_Discovery.cpp    — HA entity creation (setup)
 *   MQTTManager_StateUpdates.cpp — stats, button, indicator publishing
 *
 * Implements INotifier (publish/subscribe) and IButtonReporter (button state).
 */
#pragma once

#include <Arduino.h>
#include <map>
#include "IButtonReporter.h"
#include "INotifier.h"

/// Singleton MQTT manager handling broker communication and HA integration.
class IDisplayControl;
class IDisplayNavigation;
class IDisplayNotifier;
class ISound;
class IPower;
class IUpdater;
class IPeripheryProvider;

class MQTTManager_ : public IButtonReporter, public INotifier
{
  private:
    MQTTManager_() = default;
    MQTTManager_(const MQTTManager_&) = delete;
    MQTTManager_& operator=(const MQTTManager_&) = delete;
    MQTTManager_(MQTTManager_&&) = delete;
    MQTTManager_& operator=(MQTTManager_&&) = delete;

  public:
    /// @return The singleton MQTTManager instance.
    static MQTTManager_& getInstance();

    /// Inject display interfaces for decoupled DisplayManager access.
    void setDisplay(IDisplayControl *c, IDisplayNavigation *n, IDisplayNotifier *nt);

    /// Check whether all display interfaces have been injected.
    bool hasDisplay() const;

    /// Inject service interfaces for decoupled access to sound, power, OTA, uptime.
    void setServices(ISound *s, IPower *p, IUpdater *u, IPeripheryProvider *pp);

    /// Check whether all service interfaces have been injected.
    bool hasServices() const;

    /// Initialize HA discovery entities (if enabled) and connect to broker.
    void setup();

    /// Main loop: process MQTT messages and periodically send stats.
    void tick();

    /// Publish a payload to a topic with a custom prefix.
    /// @param prefix  MQTT topic prefix (e.g. "homeassistant")
    /// @param topic   Topic suffix (e.g. "stats/device")
    /// @param payload Message payload string
    void rawPublish(const char *prefix, const char *topic, const char *payload) override;

    /// Publish a payload to a topic under the configured device prefix.
    /// @param topic   Topic suffix appended to mqttConfig.prefix
    /// @param payload Message payload string
    void publish(const char *topic, const char *payload) override;

    /// Publish the current app name to HA and MQTT stats topic.
    /// Deduplicated: skips publish if app name hasn't changed.
    /// @param appName Name of the currently active app
    void setCurrentApp(String appName) override;

    /// Collect device stats and update all HA sensor values.
    void sendStats();

    /// Report a hardware button state change to HA binary sensors.
    /// Debounced: only publishes when state differs from last known.
    /// @param btn   Button index (0 = left, 1 = select, 2 = right)
    /// @param state true = pressed, false = released
    void sendButton(byte btn, bool state) override;

    /// Update an indicator LED's color and state in HA.
    /// @param indicator Indicator number (1, 2, or 3)
    /// @param state     true = on, false = off
    /// @param color     Packed RGB color (0xRRGGBB)
    void setIndicatorState(uint8_t indicator, bool state, uint32_t color) override;

    /// Start a streaming MQTT publish for large payloads.
    /// @param topic    Full MQTT topic
    /// @param plength  Total payload length in bytes
    /// @param retained Whether the message should be retained
    void beginPublish(const char *topic, unsigned int plength, boolean retained);

    /// Write a chunk of data to the current streaming publish.
    /// @param data   Pointer to payload chunk
    /// @param length Chunk length in bytes
    void writePayload(const char *data, const uint16_t length);

    /// Finalize the current streaming publish.
    void endPublish();

    /// Subscribe to an external MQTT topic and store its value.
    /// If not yet connected, the subscription is deferred until connection.
    /// @param topic Full MQTT topic to subscribe to
    /// @return Always true.
    bool subscribe(const char *topic) override;

    /// Return the total number of MQTT messages received.
    long getReceivedMessages() const override;

    /// Check whether the MQTT client is connected to the broker.
    /// @return true if connected, or if no MQTT host is configured.
    bool isConnected();

    /// Retrieve the last known value for a subscribed topic.
    /// @param topic The full MQTT topic to look up.
    /// @return The stored value, or "N/A" if the topic is unknown.
    String getValueForTopic(const String& topic);
};

/// Global singleton reference. Use as: MQTTManager.publish(...).
extern MQTTManager_& MQTTManager;
