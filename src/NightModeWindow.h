/**
 * @file NightModeWindow.h
 * @brief Pure window-membership helper for scheduled night mode.
 *
 * Kept header-only and dependency-free so it can be compiled into
 * native_test without pulling Arduino/Preferences. The stateful
 * wrapper lives in Globals.cpp as isNightModeActive().
 */
#pragma once

#include <cstdint>

/// Returns true iff the given minute-of-day falls within the half-open
/// window [start, end). When start > end, the window wraps across
/// midnight (e.g. start=1260 (21:00), end=360 (06:00)).
/// When start == end, the window is empty (always false).
inline bool isWithinNightWindow(uint16_t currentMinutes, uint16_t start, uint16_t end)
{
    if (start == end)
        return false;
    if (start > end)
        return currentMinutes >= start || currentMinutes < end;
    return currentMinutes >= start && currentMinutes < end;
}
