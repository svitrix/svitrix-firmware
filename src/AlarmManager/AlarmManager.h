#pragma once

#include "IAlarmProvider.h"
#include "ISound.h"
#include "IPeripheryProvider.h"
#include "IDisplayNotifier.h"
#include "ITimeProvider.h"
#include <vector>
#include <cassert>

/**
 * @file AlarmManager.h
 * @brief Local alarm manager for autonomous mode.
 *
 * Manages up to 10 alarms with persistence in /alarms.json.
 * Uses ISound for melody playback and IDisplayNotifier for visual alerts.
 */
class AlarmManager_ : public IAlarmProvider
{
public:
    static constexpr uint8_t MAX_ALARMS = 10;
    static constexpr uint8_t DEFAULT_SNOOZE_MINUTES = 5;
    static constexpr const char* ALARMS_FILE = "/alarms.json";

    static AlarmManager_& getInstance();

    // Setter injection
    void setSound(ISound* sound, IPeripheryProvider* periphery);
    void setNotifier(IDisplayNotifier* notifier);
    void setTimeProvider(ITimeProvider* timeProvider);
    bool hasServices() const { return sound_ && periphery_ && notifier_ && timeProvider_; }

    // Setup - load alarms from LittleFS
    void setup();

    // IAlarmProvider implementation
    void tick(time_t now) override;
    bool addAlarm(const Alarm& alarm) override;
    bool removeAlarm(uint8_t id) override;
    bool updateAlarm(const Alarm& alarm) override;
    std::vector<Alarm> getAlarms() const override;
    const Alarm* getAlarm(uint8_t id) const override;
    void snooze(uint8_t minutes = DEFAULT_SNOOZE_MINUTES) override;
    void dismiss() override;
    bool isRinging() const override;
    const Alarm* getRingingAlarm() const override;

    // Persistence
    bool saveAlarms();
    bool loadAlarms();

private:
    AlarmManager_();
    AlarmManager_(const AlarmManager_&) = delete;
    AlarmManager_& operator=(const AlarmManager_&) = delete;

    ISound* sound_ = nullptr;
    IPeripheryProvider* periphery_ = nullptr;
    IDisplayNotifier* notifier_ = nullptr;
    ITimeProvider* timeProvider_ = nullptr;

    std::vector<Alarm> alarms_;
    uint8_t nextId_ = 1;

    // Ringing state
    bool ringing_ = false;
    uint8_t ringingAlarmId_ = 0;
    time_t snoozeUntil_ = 0;
    time_t lastTriggerMinute_ = 0;  // Prevent re-triggering same minute

    uint8_t generateId();
    bool shouldTrigger(const Alarm& alarm, const struct tm& t) const;
    void triggerAlarm(const Alarm& alarm);
    void stopAlarm();
};

extern AlarmManager_& AlarmManager;
