#pragma once

#include <Arduino.h>

/**
 * @file IPower.h
 * @brief Interface for power management (deep sleep).
 *
 * Implemented by: PowerManager_.
 * Used by: MQTTManager, ServerManager (without depending on PowerManager directly).
 */
class IPower
{
public:
    virtual ~IPower() = default;
    virtual void sleep(uint64_t seconds) = 0;
    virtual void sleepParser(const char *json) = 0;
};
