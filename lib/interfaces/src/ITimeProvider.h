#pragma once

#include <ctime>

/**
 * @file ITimeProvider.h
 * @brief Abstraction over the system wall-clock for policies that schedule
 *        by time-of-day. Lets consumers (e.g. NightModePolicy) be tested
 *        with a mock instead of depending on timer_localtime() directly.
 */
class ITimeProvider
{
public:
    virtual ~ITimeProvider() = default;

    /// Populate `out` with the current local time if the clock is considered
    /// synchronized (e.g. NTP succeeded at least once). When the system is
    /// still on the Unix epoch or the underlying source is unavailable, this
    /// returns false and `out` is left untouched.
    ///
    /// @return true if `out` holds a trusted wall-clock; false otherwise.
    virtual bool now(struct tm& out) const = 0;
};
