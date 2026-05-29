#include "AlarmManager.h"
#include "Globals.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

AlarmManager_::AlarmManager_() {}

AlarmManager_& AlarmManager_::getInstance()
{
    static AlarmManager_ instance;
    return instance;
}

AlarmManager_& AlarmManager = AlarmManager.getInstance();

void AlarmManager_::setSound(ISound *sound, IPeripheryProvider *periphery)
{
    assert(sound != nullptr);
    assert(periphery != nullptr);
    sound_ = sound;
    periphery_ = periphery;
}

void AlarmManager_::setNotifier(IDisplayNotifier *notifier)
{
    assert(notifier != nullptr);
    notifier_ = notifier;
}

void AlarmManager_::setTimeProvider(ITimeProvider *timeProvider)
{
    assert(timeProvider != nullptr);
    timeProvider_ = timeProvider;
}

void AlarmManager_::setup()
{
    loadAlarms();
    if (systemConfig.debugMode)
    {
        DEBUG_PRINTF("AlarmManager: loaded %d alarms\n", alarms_.size());
    }
}

void AlarmManager_::tick(time_t now)
{
    if (!timeProvider_)
        return;

    struct tm t;
    if (!timeProvider_->now(t))
        return;

    // Check snooze expiry
    if (ringing_ && snoozeUntil_ > 0 && now >= snoozeUntil_)
    {
        snoozeUntil_ = 0;
        const Alarm *alarm = getAlarm(ringingAlarmId_);
        if (alarm)
        {
            triggerAlarm(*alarm);
        }
    }

    // Don't check for new triggers while ringing or snoozed
    if (ringing_ || snoozeUntil_ > 0)
        return;

    // Create a minute identifier to prevent re-triggering
    time_t currentMinute = (t.tm_hour * 60 + t.tm_min);
    if (currentMinute == lastTriggerMinute_)
        return;

    // Check all alarms
    for (const auto& alarm : alarms_)
    {
        if (shouldTrigger(alarm, t))
        {
            lastTriggerMinute_ = currentMinute;
            triggerAlarm(alarm);
            break;
        }
    }
}

bool AlarmManager_::shouldTrigger(const Alarm& alarm, const struct tm& t) const
{
    if (!alarm.enabled)
        return false;

    if (alarm.hour != t.tm_hour || alarm.minute != t.tm_min)
        return false;

    // Check day of week (tm_wday: 0=Sun, 1=Mon, ..., 6=Sat)
    uint8_t dayMask = 1 << t.tm_wday;
    return (alarm.days & dayMask) != 0;
}

void AlarmManager_::triggerAlarm(const Alarm& alarm)
{
    ringing_ = true;
    ringingAlarmId_ = alarm.id;

    if (systemConfig.debugMode)
    {
        DEBUG_PRINTF("Alarm triggered: %s (%02d:%02d)\n",
                     alarm.label.c_str(), alarm.hour, alarm.minute);
    }

    // Play melody
    if (sound_ && !alarm.melody.isEmpty())
    {
        sound_->playRTTTLString(alarm.melody);
    }

    // Show notification
    if (notifier_)
    {
        StaticJsonDocument<256> doc;
        doc["text"] = alarm.label.isEmpty() ? "ALARM" : alarm.label;
        doc["color"] = "#FF0000";
        doc["repeat"] = -1; // Repeat until dismissed
        doc["hold"] = true;

        String json;
        serializeJson(doc, json);
        notifier_->generateNotification(0, json.c_str());
    }
}

void AlarmManager_::stopAlarm()
{
    ringing_ = false;
    ringingAlarmId_ = 0;
    snoozeUntil_ = 0;

    if (sound_)
    {
        periphery_->stopSound();
    }

    if (notifier_)
    {
        notifier_->dismissNotify();
    }
}

void AlarmManager_::snooze(uint8_t minutes)
{
    if (!ringing_)
        return;

    if (sound_)
    {
        periphery_->stopSound();
    }

    if (notifier_)
    {
        notifier_->dismissNotify();
    }

    ringing_ = false;
    snoozeUntil_ = time(nullptr) + (minutes * 60);

    if (systemConfig.debugMode)
    {
        DEBUG_PRINTF("Alarm snoozed for %d minutes\n", minutes);
    }
}

void AlarmManager_::dismiss()
{
    stopAlarm();
    if (systemConfig.debugMode)
    {
        DEBUG_PRINTLN(F("Alarm dismissed"));
    }
}

bool AlarmManager_::isRinging() const
{
    return ringing_;
}

const Alarm *AlarmManager_::getRingingAlarm() const
{
    if (!ringing_)
        return nullptr;
    return getAlarm(ringingAlarmId_);
}

bool AlarmManager_::addAlarm(const Alarm& alarm)
{
    if (alarms_.size() >= MAX_ALARMS)
        return false;

    Alarm newAlarm = alarm;
    newAlarm.id = generateId();
    alarms_.push_back(newAlarm);
    saveAlarms();
    return true;
}

bool AlarmManager_::removeAlarm(uint8_t id)
{
    for (auto it = alarms_.begin(); it != alarms_.end(); ++it)
    {
        if (it->id == id)
        {
            if (ringingAlarmId_ == id)
            {
                stopAlarm();
            }
            alarms_.erase(it);
            saveAlarms();
            return true;
        }
    }
    return false;
}

bool AlarmManager_::updateAlarm(const Alarm& alarm)
{
    for (auto& a : alarms_)
    {
        if (a.id == alarm.id)
        {
            a.hour = alarm.hour;
            a.minute = alarm.minute;
            a.days = alarm.days;
            a.enabled = alarm.enabled;
            a.label = alarm.label;
            a.melody = alarm.melody;
            saveAlarms();
            return true;
        }
    }
    return false;
}

std::vector<Alarm> AlarmManager_::getAlarms() const
{
    return alarms_;
}

const Alarm *AlarmManager_::getAlarm(uint8_t id) const
{
    for (const auto& alarm : alarms_)
    {
        if (alarm.id == id)
            return &alarm;
    }
    return nullptr;
}

uint8_t AlarmManager_::generateId()
{
    uint8_t id = nextId_++;
    if (nextId_ == 0)
        nextId_ = 1;
    return id;
}

bool AlarmManager_::saveAlarms()
{
    DynamicJsonDocument doc(2048);
    JsonArray arr = doc.createNestedArray("alarms");

    for (const auto& alarm : alarms_)
    {
        JsonObject obj = arr.createNestedObject();
        obj["id"] = alarm.id;
        obj["hour"] = alarm.hour;
        obj["minute"] = alarm.minute;
        obj["days"] = alarm.days;
        obj["enabled"] = alarm.enabled;
        obj["label"] = alarm.label;
        obj["melody"] = alarm.melody;
    }

    doc["nextId"] = nextId_;

    File file = LittleFS.open(ALARMS_FILE, "w");
    if (!file)
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Failed to open alarms.json for writing"));
        return false;
    }

    serializeJson(doc, file);
    file.close();

    if (systemConfig.debugMode)
        DEBUG_PRINTF("Saved %d alarms to %s\n", alarms_.size(), ALARMS_FILE);

    return true;
}

bool AlarmManager_::loadAlarms()
{
    if (!LittleFS.exists(ALARMS_FILE))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("No alarms.json found, starting fresh"));
        return true;
    }

    File file = LittleFS.open(ALARMS_FILE, "r");
    if (!file)
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Failed to open alarms.json for reading"));
        return false;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTF("Failed to parse alarms.json: %s\n", error.c_str());
        return false;
    }

    alarms_.clear();
    nextId_ = doc["nextId"] | 1;

    JsonArray arr = doc["alarms"];
    for (JsonObject obj : arr)
    {
        Alarm alarm;
        alarm.id = obj["id"] | 0;
        alarm.hour = obj["hour"] | 0;
        alarm.minute = obj["minute"] | 0;
        alarm.days = obj["days"] | 0x7F;
        alarm.enabled = obj["enabled"] | true;
        alarm.label = obj["label"] | "";
        alarm.melody = obj["melody"] | "";

        if (alarm.id > 0)
        {
            alarms_.push_back(alarm);
        }
    }

    return true;
}
