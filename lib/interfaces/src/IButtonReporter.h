#pragma once

#include <Arduino.h>

/**
 * @file IButtonReporter.h
 * @brief Interface for reporting physical button state changes.
 *
 * Implemented by: MQTTManager_, ServerManager_.
 * Used by: PeripheryManager_ (reports button presses to registered reporters).
 */
class IButtonReporter
{
public:
    virtual ~IButtonReporter() = default;
    virtual void sendButton(byte btn, bool state) = 0;
};

