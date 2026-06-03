#pragma once
#include <FastLED.h>

/**
 * @file IMatrixHost.h
 * @brief Interface for MatrixDisplayUi to call back into the display host.
 *
 * Breaks the circular dependency between MatrixDisplayUi and DisplayManager.
 * Implemented by: DisplayManager_.
 * Used by: MatrixDisplayUi (transitions, gamma, app lifecycle).
 */
class IMatrixHost
{
public:
    virtual ~IMatrixHost() = default;
    virtual CRGB *getLeds() = 0;
    virtual void gammaCorrection() = 0;
    virtual void sendAppLoop() = 0;
    virtual bool setAutoTransition(bool active) = 0;
    /// Called when auto-transition timer expires. Returns the next app index
    /// for playlist mode, or -1 to use default sequential behavior.
    virtual int8_t resolveNextApp(int8_t currentApp, int8_t direction) = 0;
};
