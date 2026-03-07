#pragma once

#include <Arduino.h>

/**
 * @file IPeripheryProvider.h
 * @brief Interface for periphery hardware access (audio, uptime).
 *
 * Implemented by: PeripheryManager_.
 * Used by: DisplayManager_ (stops sound, reads uptime without depending on PeripheryManager directly).
 */
class IPeripheryProvider
{
public:
    virtual ~IPeripheryProvider() = default;
    virtual void stopSound() = 0;
    virtual void setVolume(uint8_t vol) = 0;
    virtual unsigned long long readUptime() = 0;
};

