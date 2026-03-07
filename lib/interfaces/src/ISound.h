#pragma once

#include <Arduino.h>

/**
 * @file ISound.h
 * @brief Interface for sound playback (RTTTL, parsed JSON, R2D2).
 *
 * Implemented by: PeripheryManager_.
 * Used by: MQTTManager, ServerManager (without depending on PeripheryManager directly).
 */
class ISound
{
public:
    virtual ~ISound() = default;
    virtual const char *playRTTTLString(String rtttl) = 0;
    virtual bool parseSound(const char *json) = 0;
    virtual void r2d2(const char *msg) = 0;
};
