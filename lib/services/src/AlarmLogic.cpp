#include "AlarmLogic.h"

bool alarmMatches(bool enabled, uint8_t hour, uint8_t minute,
                  uint8_t days, bool oneTime, const struct tm& t)
{
    if (!enabled)
        return false;
    if (hour != t.tm_hour || minute != t.tm_min)
        return false;
    if (oneTime)
        return true; // fires at hh:mm regardless of weekday
    return (days & (1 << t.tm_wday)) != 0;
}

int minutesUntilNext(bool enabled, uint8_t hour, uint8_t minute,
                     uint8_t days, bool oneTime, const struct tm& t)
{
    if (!enabled)
        return -1;

    const int nowMins = t.tm_hour * 60 + t.tm_min; // minute of the day
    const int alarmMins = hour * 60 + minute;

    if (oneTime)
    {
        int delta = alarmMins - nowMins;
        if (delta < 0)
            delta += 24 * 60; // tomorrow
        return delta;
    }

    if (days == 0)
        return -1; // recurring alarm with no active days never fires

    // Find the soonest matching weekday within the next 7 days.
    int best = -1;
    for (int offset = 0; offset < 7; offset++)
    {
        int day = (t.tm_wday + offset) % 7;
        if ((days & (1 << day)) == 0)
            continue;

        int delta = offset * 24 * 60 + (alarmMins - nowMins);
        if (delta < 0)
            delta += 7 * 24 * 60; // already passed today → same weekday next week
        if (best < 0 || delta < best)
            best = delta;
    }
    return best;
}
