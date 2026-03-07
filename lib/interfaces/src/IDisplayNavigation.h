#pragma once
#include <Arduino.h>

/// Interface for app lifecycle and navigation.
/// Consumers: MenuManager, ServerManager, MQTTManager.
class IDisplayNavigation
{
public:
    virtual ~IDisplayNavigation() = default;
    virtual void nextApp() = 0;
    virtual void previousApp() = 0;
    virtual bool switchToApp(const char *json) = 0;
    virtual void updateAppVector(const char *json) = 0;
    virtual void reorderApps(const String &jsonString) = 0;
    virtual String getAppsAsJson() = 0;
    virtual String getAppsWithIcon() = 0;
    virtual bool parseCustomPage(const String &name, const char *json, bool preventSave) = 0;
    virtual String getEffectNames() = 0;
    virtual String getTransitionNames() = 0;
    virtual void loadNativeApps() = 0;
    virtual void setCustomAppColors(uint32_t color) = 0;
};
