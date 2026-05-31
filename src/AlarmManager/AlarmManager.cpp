#include "AlarmManager.h"
#include "AlarmLogic.h"
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

    // Snooze expiry → re-ring. Checked independently of ringing_ (which is
    // false while snoozed) so the alarm actually comes back.
    if (snoozeUntil_ > 0 && now >= snoozeUntil_)
    {
        snoozeUntil_ = 0;
        const Alarm *alarm = getAlarm(ringingAlarmId_);
        if (alarm)
            triggerAlarm(*alarm);
        return;
    }

    // Don't check for new triggers while ringing or snoozed
    if (ringing_ || snoozeUntil_ > 0)
        return;

    // Dedup by absolute epoch-minute so an alarm fires once per occurrence
    // (and again the next day), not just once ever.
    time_t currentMinute = now / 60;
    if (currentMinute == lastTriggerMinute_)
        return;

    for (const auto& alarm : alarms_)
    {
        if (alarmMatches(alarm.enabled, alarm.hour, alarm.minute,
                         alarm.days, alarm.oneTime, t))
        {
            lastTriggerMinute_ = currentMinute;
            triggerAlarm(alarm);

            // One-time alarms (reminders) auto-disable after firing.
            if (alarm.oneTime)
            {
                for (auto& a : alarms_)
                {
                    if (a.id == alarm.id)
                    {
                        a.enabled = false;
                        break;
                    }
                }
                saveAlarms();
            }
            break;
        }
    }
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

    // Show a held, red notification. The melody travels with the notification
    // and loopSound makes the notification overlay re-play it whenever the
    // buzzer goes idle, so it repeats until the alarm is dismissed or snoozed.
    if (notifier_)
    {
        StaticJsonDocument<512> doc; // room for an RTTTL melody
        doc["text"] = alarm.label.isEmpty() ? "ALARM" : alarm.label;
        doc["color"] = "#FF0000";
        doc["repeat"] = -1; // scroll the text until dismissed
        doc["hold"] = true; // stay on screen until dismissed
        if (!alarm.melody.isEmpty())
        {
            doc["rtttl"] = alarm.melody;
            doc["loopSound"] = true; // repeat the melody until dismissed/snoozed
        }

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
            a.oneTime = alarm.oneTime;
            a.snoozeMinutes = alarm.snoozeMinutes;
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
    DynamicJsonDocument doc(4096); // room for up to 10 alarms with RTTTL melodies
    JsonArray arr = doc.createNestedArray("alarms");

    for (const auto& alarm : alarms_)
    {
        JsonObject obj = arr.createNestedObject();
        obj["id"] = alarm.id;
        obj["hour"] = alarm.hour;
        obj["minute"] = alarm.minute;
        obj["days"] = alarm.days;
        obj["enabled"] = alarm.enabled;
        obj["oneTime"] = alarm.oneTime;
        obj["snooze"] = alarm.snoozeMinutes;
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

    DynamicJsonDocument doc(4096); // room for up to 10 alarms with RTTTL melodies
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
        alarm.oneTime = obj["oneTime"] | false;
        alarm.snoozeMinutes = obj["snooze"] | 5;
        alarm.label = obj["label"] | "";
        alarm.melody = obj["melody"] | "";

        if (alarm.id > 0)
        {
            alarms_.push_back(alarm);
        }
    }

    return true;
}
