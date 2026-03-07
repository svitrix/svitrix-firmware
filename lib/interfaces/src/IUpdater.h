#pragma once

#include <Arduino.h>

/**
 * @file IUpdater.h
 * @brief Interface for firmware update operations.
 *
 * Implemented by: UpdateManager_.
 * Used by: MQTTManager, ServerManager, MenuManager (without depending on UpdateManager directly).
 */
class IUpdater
{
public:
    virtual ~IUpdater() = default;
    virtual bool checkUpdate(bool withScreen) = 0;
    virtual void updateFirmware() = 0;
};
