/**
 * @file test_night_mode_policy.cpp
 * @brief Tests for NightModePolicy — the IDisplayPolicy implementation that
 *        schedules a dim / recolor override by time-of-day.
 *
 * A mock ITimeProvider lets these tests exercise the full policy (including
 * the appConfig.nightMode short-circuit and the pre-NTP guard) without
 * pulling Arduino/NTP into the native_test build.
 */

#include <unity.h>
#include "NightModePolicy.h"
#include "ConfigTypes.h"

// ── Mock ITimeProvider ───────────────────────────────────────────────

class FakeTimeProvider : public ITimeProvider
{
public:
    bool synced = true;
    int hour = 0;
    int minute = 0;

    bool now(struct tm& out) const override
    {
        if (!synced)
            return false;
        out = {};
        out.tm_hour = hour;
        out.tm_min = minute;
        out.tm_year = 126; // 2026, well past the 2020 threshold
        return true;
    }

    void set(int h, int m)
    {
        hour = h;
        minute = m;
    }
};

// ── Fixture helpers ──────────────────────────────────────────────────

static AppConfig makeDefaultCfg()
{
    AppConfig cfg{};
    cfg.nightMode = true;
    cfg.nightStart = 1260;      // 21:00
    cfg.nightEnd = 360;         // 06:00
    cfg.nightBrightness = 5;
    cfg.nightColor = 0xFF0000;
    cfg.nightBlockTransition = true;
    return cfg;
}

// ── Tests: active-flag behavior ──────────────────────────────────────

void test_inactive_when_nightMode_disabled(void)
{
    AppConfig cfg = makeDefaultCfg();
    cfg.nightMode = false;
    FakeTimeProvider time;
    time.set(23, 0); // deep in the default window
    NightModePolicy policy(cfg, time);

    TEST_ASSERT_FALSE(policy.isActive());
}

void test_inactive_when_time_not_synced(void)
{
    AppConfig cfg = makeDefaultCfg();
    FakeTimeProvider time;
    time.synced = false;
    NightModePolicy policy(cfg, time);

    TEST_ASSERT_FALSE(policy.isActive());
}

void test_active_inside_wrapping_window(void)
{
    AppConfig cfg = makeDefaultCfg();
    FakeTimeProvider time;
    NightModePolicy policy(cfg, time);

    time.set(22, 30);
    TEST_ASSERT_TRUE(policy.isActive());
    time.set(0, 0);
    TEST_ASSERT_TRUE(policy.isActive());
    time.set(5, 59);
    TEST_ASSERT_TRUE(policy.isActive());
}

void test_inactive_outside_wrapping_window(void)
{
    AppConfig cfg = makeDefaultCfg();
    FakeTimeProvider time;
    NightModePolicy policy(cfg, time);

    time.set(6, 0);    // end is exclusive
    TEST_ASSERT_FALSE(policy.isActive());
    time.set(12, 0);
    TEST_ASSERT_FALSE(policy.isActive());
    time.set(20, 59);
    TEST_ASSERT_FALSE(policy.isActive());
}

void test_active_inside_non_wrapping_window(void)
{
    AppConfig cfg = makeDefaultCfg();
    cfg.nightStart = 600;  // 10:00
    cfg.nightEnd = 1200;   // 20:00
    FakeTimeProvider time;
    NightModePolicy policy(cfg, time);

    time.set(15, 30);
    TEST_ASSERT_TRUE(policy.isActive());
}

// ── Tests: override payloads ─────────────────────────────────────────

void test_override_brightness_value(void)
{
    AppConfig cfg = makeDefaultCfg();
    cfg.nightBrightness = 12;
    FakeTimeProvider time;
    NightModePolicy policy(cfg, time);

    uint8_t out = 0;
    TEST_ASSERT_TRUE(policy.overridesBrightness(out));
    TEST_ASSERT_EQUAL_UINT8(12, out);
}

void test_override_text_color_value(void)
{
    AppConfig cfg = makeDefaultCfg();
    cfg.nightColor = 0x00FF00;
    FakeTimeProvider time;
    NightModePolicy policy(cfg, time);

    uint32_t out = 0;
    TEST_ASSERT_TRUE(policy.overridesTextColor(out));
    TEST_ASSERT_EQUAL_HEX32(0x00FF00, out);
}

void test_block_transition_honours_config(void)
{
    AppConfig cfg = makeDefaultCfg();
    FakeTimeProvider time;
    NightModePolicy policy(cfg, time);

    cfg.nightBlockTransition = true;
    TEST_ASSERT_TRUE(policy.blocksAutoTransition());

    cfg.nightBlockTransition = false;
    TEST_ASSERT_FALSE(policy.blocksAutoTransition());
}

// ── Runner ───────────────────────────────────────────────────────────

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_inactive_when_nightMode_disabled);
    RUN_TEST(test_inactive_when_time_not_synced);
    RUN_TEST(test_active_inside_wrapping_window);
    RUN_TEST(test_inactive_outside_wrapping_window);
    RUN_TEST(test_active_inside_non_wrapping_window);
    RUN_TEST(test_override_brightness_value);
    RUN_TEST(test_override_text_color_value);
    RUN_TEST(test_block_transition_honours_config);
    return UNITY_END();
}
