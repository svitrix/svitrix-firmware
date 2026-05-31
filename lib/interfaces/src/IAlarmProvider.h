#pragma once

#include <cstdint>
#include <ctime>
#include <vector>
#include <WString.h>

/**
 * @file IAlarmProvider.h
 * @brief Interface for local alarm management in autonomous mode.
 */

struct Alarm
{
    uint8_t id;
    uint8_t hour;
    uint8_t minute;
    uint8_t days;          // Bitmask: Sun=0x01, Mon=0x02, Tue=0x04, Wed=0x08, Thu=0x10, Fri=0x20, Sat=0x40
    bool enabled;
    bool oneTime;          // Fire once, then auto-disable (reminder). Day mask ignored when set.
    uint8_t snoozeMinutes; // Per-alarm snooze duration (minutes)
    String label;
    String melody;         // RTTTL string or melody filename

    Alarm() : id(0), hour(0), minute(0), days(0x7F), enabled(true),
              oneTime(false), snoozeMinutes(5), label(""), melody("") {}
};

class IAlarmProvider
{
public:
    virtual ~IAlarmProvider() = default;

    /// Called every second from main loop to check and trigger alarms
    virtual void tick(time_t now) = 0;

    /// Add a new alarm. Returns true on success, false if max alarms reached.
    virtual bool addAlarm(const Alarm& alarm) = 0;

    /// Remove alarm by ID. Returns true if found and removed.
    virtual bool removeAlarm(uint8_t id) = 0;

    /// Update existing alarm. Returns true if found and updated.
    virtual bool updateAlarm(const Alarm& alarm) = 0;

    /// Get all configured alarms
    virtual std::vector<Alarm> getAlarms() const = 0;

    /// Get alarm by ID. Returns nullptr if not found.
    virtual const Alarm* getAlarm(uint8_t id) const = 0;

    /// Snooze currently ringing alarm for specified minutes (default 5)
    virtual void snooze(uint8_t minutes = 5) = 0;

    /// Dismiss currently ringing alarm
    virtual void dismiss() = 0;

    /// Check if an alarm is currently ringing
    virtual bool isRinging() const = 0;

    /// Get the currently ringing alarm (nullptr if none)
    virtual const Alarm* getRingingAlarm() const = 0;
};
