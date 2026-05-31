#pragma once

/**
 * @file AlarmLogic.h
 * @brief Pure alarm scheduling logic (no hardware, no Arduino String).
 *
 * Extracted from AlarmManager so the trigger/next-alarm decisions are unit
 * testable. Operates on plain alarm timing fields rather than the Alarm
 * struct, keeping this service free of any interface/Arduino dependency.
 *
 * Used by: AlarmManager (trigger detection, next-alarm sensor).
 * Tests: test/test_native/test_alarm_logic/
 *
 * Day bitmask convention matches struct Alarm: bit 0 = Sunday ... bit 6 =
 * Saturday, i.e. `days & (1 << tm_wday)` (tm_wday: 0=Sun..6=Sat).
 */

#include <ctime>
#include <cstdint>

/// Does an alarm with these properties fire exactly at local time @p t?
/// One-time alarms ignore the day mask (they fire at the next hh:mm once).
bool alarmMatches(bool enabled, uint8_t hour, uint8_t minute,
                  uint8_t days, bool oneTime, const struct tm& t);

/// Minutes from @p t until this alarm's next occurrence.
///   - 0      → it matches right now
///   - >0     → minutes until the next fire (within a week for recurring,
///              within 24h for one-time)
///   - -1     → it can never fire (disabled, or recurring with no days set)
int minutesUntilNext(bool enabled, uint8_t hour, uint8_t minute,
                     uint8_t days, bool oneTime, const struct tm& t);
