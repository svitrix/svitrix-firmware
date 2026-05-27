#pragma once

#include <ctime>

/**
 * @file IRtcProvider.h
 * @brief Abstraction over a hardware RTC (Real-Time Clock) for keeping time
 *        across power cycles and when NTP is unavailable.
 */
class IRtcProvider
{
public:
    virtual ~IRtcProvider() = default;

    /// Initialize the RTC hardware. Returns true if the RTC is detected and
    /// operational, false otherwise.
    virtual bool begin() = 0;

    /// Check if the RTC oscillator is running. Returns false if the clock-halt
    /// bit is set (e.g., dead battery on DS1307).
    virtual bool isRunning() = 0;

    /// Get the current time from the RTC as Unix epoch (seconds since 1970).
    /// Returns 0 if the RTC is not available or not running.
    virtual time_t getTime() = 0;

    /// Set the RTC time from Unix epoch. Returns true on success.
    virtual bool setTime(time_t epoch) = 0;
};
