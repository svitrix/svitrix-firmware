#pragma once

/**
 * @file IButtonHandler.h
 * @brief Interface for button event handling.
 *
 * Implemented by: DisplayManager_, MenuManager_.
 * Used by: PeripheryManager_ (dispatches button events to registered handlers).
 */
class IButtonHandler
{
public:
    virtual ~IButtonHandler() = default;
    virtual void leftButton() = 0;
    virtual void rightButton() = 0;
    virtual void selectButton() = 0;
    virtual void selectButtonLong() = 0;
};

