#pragma once

#include "ITimeProvider.h"
#include "timer.h"

/**
 * @file RealTimeProvider.h
 * @brief Production ITimeProvider that wraps timer_localtime().
 *
 * Considers the clock synchronized when tm_year >= 120 (i.e. year >= 2020).
 * Before NTP syncs the ESP32 reports ~1970, which would otherwise trigger
 * scheduled modes at boot.
 */
class RealTimeProvider : public ITimeProvider
{
  public:
    bool now(struct tm& out) const override
    {
        const struct tm *t = timer_localtime();
        if (!t)
            return false;
        if (t->tm_year < 120)
            return false;
        out = *t;
        return true;
    }
};
