#pragma once

#include <Arduino.h>

/**
 * @file INotifier.h
 * @brief Interface for MQTT publish/subscribe operations.
 *
 * Implemented by: MQTTManager_.
 * Used by: DisplayManager_ (publishes state changes without depending on MQTT directly).
 */
class INotifier
{
public:
    virtual ~INotifier() = default;
    virtual void publish(const char *topic, const char *payload) = 0;
    virtual void rawPublish(const char *prefix, const char *topic, const char *payload) = 0;
    virtual void setCurrentApp(String app) = 0;
    virtual void setIndicatorState(uint8_t indicator, bool state, uint32_t color) = 0;
    virtual bool subscribe(const char *topic) = 0;
    virtual long getReceivedMessages() const = 0;
};

