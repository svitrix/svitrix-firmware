#pragma once
#include <Arduino.h>

/// Interface for notifications and LED indicator control.
/// Consumers: ServerManager, MQTTManager.
class IDisplayNotifier
{
public:
    virtual ~IDisplayNotifier() = default;
    virtual bool generateNotification(uint8_t source, const char *json) = 0;
    virtual void dismissNotify() = 0;
    virtual bool indicatorParser(uint8_t indicator, const char *json) = 0;
    virtual void setIndicator1Color(uint32_t color) = 0;
    virtual void setIndicator1State(bool state) = 0;
    virtual void setIndicator2Color(uint32_t color) = 0;
    virtual void setIndicator2State(bool state) = 0;
    virtual void setIndicator3Color(uint32_t color) = 0;
    virtual void setIndicator3State(bool state) = 0;
};
