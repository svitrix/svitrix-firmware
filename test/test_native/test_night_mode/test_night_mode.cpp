/**
 * @file test_night_mode.cpp
 * @brief Tests for isWithinNightWindow() — the pure window-membership
 *        helper backing the isNightModeActive() check in Globals.cpp.
 *
 * Covers both the non-wrapping case (start < end, daytime-style window)
 * and the wrap-across-midnight case (start > end, which is the expected
 * night-mode configuration). The stateful wrapper (NTP guard, appConfig
 * short-circuit) lives in Globals.cpp and is out of scope here since
 * Globals.cpp can't compile under native_test.
 */

#include <unity.h>
#include "NightModeWindow.h"

// ── Non-wrapping window (start < end) ───────────────────────────────

void test_non_wrapping_before_start_is_outside(void)
{
    // 09:59 before a 10:00–20:00 window
    TEST_ASSERT_FALSE(isWithinNightWindow(9 * 60 + 59, 10 * 60, 20 * 60));
}

void test_non_wrapping_at_start_is_inside(void)
{
    // Window is half-open [start, end); start itself is inside.
    TEST_ASSERT_TRUE(isWithinNightWindow(10 * 60, 10 * 60, 20 * 60));
}

void test_non_wrapping_mid_is_inside(void)
{
    TEST_ASSERT_TRUE(isWithinNightWindow(15 * 60, 10 * 60, 20 * 60));
}

void test_non_wrapping_at_end_is_outside(void)
{
    // Window is half-open [start, end); end itself is outside.
    TEST_ASSERT_FALSE(isWithinNightWindow(20 * 60, 10 * 60, 20 * 60));
}

void test_non_wrapping_after_end_is_outside(void)
{
    TEST_ASSERT_FALSE(isWithinNightWindow(22 * 60, 10 * 60, 20 * 60));
}

// ── Wrapping window (start > end, crosses midnight) ─────────────────

void test_wrapping_default_window_evening_inside(void)
{
    // Default 21:00 → 06:00. 22:30 should be inside.
    TEST_ASSERT_TRUE(isWithinNightWindow(22 * 60 + 30, 1260, 360));
}

void test_wrapping_default_window_midnight_inside(void)
{
    TEST_ASSERT_TRUE(isWithinNightWindow(0, 1260, 360));
}

void test_wrapping_default_window_early_morning_inside(void)
{
    // 05:59 should be inside (end=06:00 is exclusive).
    TEST_ASSERT_TRUE(isWithinNightWindow(5 * 60 + 59, 1260, 360));
}

void test_wrapping_default_window_at_end_is_outside(void)
{
    // 06:00 itself is outside (half-open).
    TEST_ASSERT_FALSE(isWithinNightWindow(6 * 60, 1260, 360));
}

void test_wrapping_default_window_noon_outside(void)
{
    TEST_ASSERT_FALSE(isWithinNightWindow(12 * 60, 1260, 360));
}

void test_wrapping_default_window_just_before_start_outside(void)
{
    // 20:59 is outside, 21:00 is inside.
    TEST_ASSERT_FALSE(isWithinNightWindow(20 * 60 + 59, 1260, 360));
    TEST_ASSERT_TRUE(isWithinNightWindow(21 * 60, 1260, 360));
}

// ── Edge cases ──────────────────────────────────────────────────────

void test_empty_window_is_never_active(void)
{
    // start == end: empty window by convention. Avoids ambiguity between
    // "always on" and "always off" interpretations.
    TEST_ASSERT_FALSE(isWithinNightWindow(0, 600, 600));
    TEST_ASSERT_FALSE(isWithinNightWindow(600, 600, 600));
    TEST_ASSERT_FALSE(isWithinNightWindow(1200, 600, 600));
}

void test_minute_after_midnight_boundary(void)
{
    // 00:00 with window 23:59 → 00:01 (covers 1 minute: 23:59).
    // Actually [1439, 1) wraps: inside if minutes >= 1439 || < 1.
    TEST_ASSERT_TRUE(isWithinNightWindow(0, 1439, 1));
    TEST_ASSERT_TRUE(isWithinNightWindow(1439, 1439, 1));
    TEST_ASSERT_FALSE(isWithinNightWindow(1, 1439, 1));
    TEST_ASSERT_FALSE(isWithinNightWindow(720, 1439, 1));
}

// ── Runner ──────────────────────────────────────────────────────────

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    // Non-wrapping
    RUN_TEST(test_non_wrapping_before_start_is_outside);
    RUN_TEST(test_non_wrapping_at_start_is_inside);
    RUN_TEST(test_non_wrapping_mid_is_inside);
    RUN_TEST(test_non_wrapping_at_end_is_outside);
    RUN_TEST(test_non_wrapping_after_end_is_outside);

    // Wrapping
    RUN_TEST(test_wrapping_default_window_evening_inside);
    RUN_TEST(test_wrapping_default_window_midnight_inside);
    RUN_TEST(test_wrapping_default_window_early_morning_inside);
    RUN_TEST(test_wrapping_default_window_at_end_is_outside);
    RUN_TEST(test_wrapping_default_window_noon_outside);
    RUN_TEST(test_wrapping_default_window_just_before_start_outside);

    // Edge
    RUN_TEST(test_empty_window_is_never_active);
    RUN_TEST(test_minute_after_midnight_boundary);

    return UNITY_END();
}
