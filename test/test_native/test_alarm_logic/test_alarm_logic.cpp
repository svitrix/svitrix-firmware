#include <unity.h>
#include "AlarmLogic.h"

// Day bits (bit0 = Sunday)
enum { SUN = 0x01, MON = 0x02, TUE = 0x04, WED = 0x08, THU = 0x10, FRI = 0x20, SAT = 0x40 };
static const uint8_t ALL_DAYS = 0x7F;

// Build a local-time tm with the given weekday/hour/minute (others zeroed).
static struct tm makeTime(int wday, int hour, int min)
{
    struct tm t = {};
    t.tm_wday = wday; // 0=Sun..6=Sat
    t.tm_hour = hour;
    t.tm_min = min;
    return t;
}

// ── alarmMatches ────────────────────────────────────────────────────

void test_match_disabled_never_fires(void)
{
    struct tm t = makeTime(3, 7, 30); // Wed 07:30
    TEST_ASSERT_FALSE(alarmMatches(false, 7, 30, ALL_DAYS, false, t));
}

void test_match_wrong_minute(void)
{
    struct tm t = makeTime(3, 7, 31);
    TEST_ASSERT_FALSE(alarmMatches(true, 7, 30, ALL_DAYS, false, t));
}

void test_match_recurring_day_set(void)
{
    struct tm wed = makeTime(3, 7, 30);
    TEST_ASSERT_TRUE(alarmMatches(true, 7, 30, WED, false, wed));
}

void test_match_recurring_day_not_set(void)
{
    struct tm tue = makeTime(2, 7, 30);
    TEST_ASSERT_FALSE(alarmMatches(true, 7, 30, WED, false, tue)); // only Wed enabled
}

void test_match_onetime_ignores_day_mask(void)
{
    struct tm tue = makeTime(2, 7, 30);
    // days==0 but oneTime → still fires at hh:mm
    TEST_ASSERT_TRUE(alarmMatches(true, 7, 30, 0, true, tue));
}

void test_match_weekday_specific(void)
{
    TEST_ASSERT_TRUE(alarmMatches(true, 6, 0, MON, false, makeTime(1, 6, 0)));  // Mon
    TEST_ASSERT_FALSE(alarmMatches(true, 6, 0, MON, false, makeTime(2, 6, 0))); // Tue
}

// ── minutesUntilNext ────────────────────────────────────────────────

void test_until_disabled(void)
{
    TEST_ASSERT_EQUAL(-1, minutesUntilNext(false, 7, 0, ALL_DAYS, false, makeTime(3, 6, 0)));
}

void test_until_recurring_no_days(void)
{
    TEST_ASSERT_EQUAL(-1, minutesUntilNext(true, 7, 0, 0, false, makeTime(3, 6, 0)));
}

void test_until_matches_now(void)
{
    TEST_ASSERT_EQUAL(0, minutesUntilNext(true, 10, 0, WED, false, makeTime(3, 10, 0)));
}

void test_until_recurring_later_today(void)
{
    // Wed 10:00 now, alarm Wed 14:00 → 240 min
    TEST_ASSERT_EQUAL(240, minutesUntilNext(true, 14, 0, WED, false, makeTime(3, 10, 0)));
}

void test_until_recurring_passed_today_next_week(void)
{
    // Wed 10:00 now, alarm Wed 08:00 (only Wed) → next week: 7*1440 - 120 = 9960
    TEST_ASSERT_EQUAL(9960, minutesUntilNext(true, 8, 0, WED, false, makeTime(3, 10, 0)));
}

void test_until_recurring_tomorrow(void)
{
    // Wed 10:00 now, alarm Thu 08:00 → 1440 - 120 = 1320
    TEST_ASSERT_EQUAL(1320, minutesUntilNext(true, 8, 0, THU, false, makeTime(3, 10, 0)));
}

void test_until_recurring_picks_soonest(void)
{
    // Wed 10:00 now, alarm 08:00 on Mon+Thu → Thu (1320) is sooner than Mon (7080)
    TEST_ASSERT_EQUAL(1320, minutesUntilNext(true, 8, 0, MON | THU, false, makeTime(3, 10, 0)));
}

void test_until_onetime_later_today(void)
{
    TEST_ASSERT_EQUAL(240, minutesUntilNext(true, 14, 0, 0, true, makeTime(3, 10, 0)));
}

void test_until_onetime_passed_tomorrow(void)
{
    // 10:00 now, one-time 08:00 → next day: 1440 - 120 = 1320
    TEST_ASSERT_EQUAL(1320, minutesUntilNext(true, 8, 0, 0, true, makeTime(3, 10, 0)));
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_match_disabled_never_fires);
    RUN_TEST(test_match_wrong_minute);
    RUN_TEST(test_match_recurring_day_set);
    RUN_TEST(test_match_recurring_day_not_set);
    RUN_TEST(test_match_onetime_ignores_day_mask);
    RUN_TEST(test_match_weekday_specific);

    RUN_TEST(test_until_disabled);
    RUN_TEST(test_until_recurring_no_days);
    RUN_TEST(test_until_matches_now);
    RUN_TEST(test_until_recurring_later_today);
    RUN_TEST(test_until_recurring_passed_today_next_week);
    RUN_TEST(test_until_recurring_tomorrow);
    RUN_TEST(test_until_recurring_picks_soonest);
    RUN_TEST(test_until_onetime_later_today);
    RUN_TEST(test_until_onetime_passed_tomorrow);

    return UNITY_END();
}
