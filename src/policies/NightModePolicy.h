#pragma once

#include <cstdint>
#include "IDisplayPolicy.h"
#include "ITimeProvider.h"
#include "ConfigTypes.h"
#include "NightModeWindow.h"

/**
 * @file NightModePolicy.h
 * @brief Scheduled dim-and-recolor policy for nighttime use.
 *
 * Active while the wall-clock falls inside [appConfig.nightStart,
 * appConfig.nightEnd). When start > end, the window wraps across midnight.
 * Overrides brightness and text color, and optionally blocks auto-transition.
 *
 * Header-only so native_test can compile it without plumbing src/policies/
 * into the test build_src_filter. Dependencies (AppConfig + ITimeProvider)
 * are injected by reference so the policy is testable with a mock clock.
 *
 * The override* methods assume isActive() was already true per the
 * IDisplayPolicy contract — no redundant time-of-day lookup on the hot path.
 */
class NightModePolicy : public IDisplayPolicy
{
  public:
    NightModePolicy(const AppConfig& cfg, const ITimeProvider& time)
        : cfg_(cfg)
        , time_(time)
    {
    }

    bool isActive() const override
    {
        if (!cfg_.nightMode)
            return false;

        struct tm now;
        if (!time_.now(now))
            return false;

        const uint16_t currentMinutes = static_cast<uint16_t>(now.tm_hour * 60 + now.tm_min);
        return isWithinNightWindow(currentMinutes, cfg_.nightStart, cfg_.nightEnd);
    }

    bool overridesBrightness(uint8_t& out) const override
    {
        out = cfg_.nightBrightness;
        return true;
    }

    bool overridesTextColor(uint32_t& out) const override
    {
        out = cfg_.nightColor;
        return true;
    }

    bool blocksAutoTransition() const override
    {
        return cfg_.nightBlockTransition;
    }

  private:
    const AppConfig& cfg_;
    const ITimeProvider& time_;
};
