#pragma once
#include <Arduino.h>

/// Interface for display power, brightness, and settings management.
/// Consumers: MenuManager, ServerManager, MQTTManager.
class IDisplayControl
{
public:
    virtual ~IDisplayControl() = default;
    virtual void setBrightness(int bri) = 0;
    virtual void setPower(bool state) = 0;
    virtual void powerStateParse(const char *json) = 0;
    virtual bool setAutoTransition(bool active) = 0;
    virtual void applyAllSettings() = 0;
    virtual void setNewSettings(const char *json) = 0;
    virtual String getSettings() = 0;
    virtual String getStats() = 0;
    virtual bool moodlight(const char *json) = 0;
    virtual String ledsAsJson() = 0;
};
