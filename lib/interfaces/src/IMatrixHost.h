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
};
