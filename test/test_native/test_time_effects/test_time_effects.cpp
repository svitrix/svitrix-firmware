#include <unity.h>
#include "TimeEffects.h"

// --- fadeColorAt ---

void test_fade_at_zero_phase(void)
{
    // At interval boundary (sin=0 → phase=0.5), color is half brightness
    // At t=0, sin(0)=0, phase = (0+1)*0.5 = 0.5
    uint32_t result = fadeColorAt(0xFF0000, 1000, 0);
    uint8_t r = (result >> 16) & 0xFF;
    TEST_ASSERT_INT_WITHIN(5, 127, r);
}

void test_fade_at_quarter_period(void)
{
    // At t=interval/4, sin(PI/2)=1, phase = (1+1)*0.5 = 1.0 → full color
    uint32_t result = fadeColorAt(0xFF0000, 1000, 250);
    uint8_t r = (result >> 16) & 0xFF;
    TEST_ASSERT_EQUAL_UINT8(255, r);
}

void test_fade_at_three_quarter_period(void)
{
    // At t=3*interval/4, sin(3*PI/2)=-1, phase = (-1+1)*0.5 = 0.0 → black
    uint32_t result = fadeColorAt(0xFF0000, 1000, 750);
    uint8_t r = (result >> 16) & 0xFF;
    TEST_ASSERT_INT_WITHIN(2, 0, r);
}

void test_fade_preserves_color_channels(void)
{
    // All channels should fade equally at phase=0.5
    uint32_t result = fadeColorAt(0xFFFFFF, 1000, 0); // phase=0.5
    uint8_t r = (result >> 16) & 0xFF;
    uint8_t g = (result >> 8) & 0xFF;
    uint8_t b = result & 0xFF;
    TEST_ASSERT_INT_WITHIN(5, 127, r);
    TEST_ASSERT_INT_WITHIN(5, 127, g);
    TEST_ASSERT_INT_WITHIN(5, 127, b);
}

void test_fade_black_stays_black(void)
{
    // Black should stay black at any phase
    uint32_t result = fadeColorAt(0x000000, 1000, 250);
    TEST_ASSERT_EQUAL_HEX32(0x000000, result);
}

// --- textEffectAt ---

void test_effect_no_fade_no_blink(void)
{
    // No fade, no blink → return original color
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, textEffectAt(0xFF0000, 0, 0, 500));
}

void test_effect_fade_at_quarter(void)
{
    // Fade active, at quarter period → full color
    uint32_t result = textEffectAt(0xFF0000, 1000, 0, 250);
    uint8_t r = (result >> 16) & 0xFF;
    TEST_ASSERT_EQUAL_UINT8(255, r);
}

void test_effect_blink_visible(void)
{
    // Blink period=1000, at t=750 → 750 % 1000 = 750 > 500 → visible
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, textEffectAt(0xFF0000, 0, 1000, 750));
}

void test_effect_blink_hidden(void)
{
    // Blink period=1000, at t=250 → 250 % 1000 = 250 < 500 → hidden (black)
    TEST_ASSERT_EQUAL_HEX32(0x000000, textEffectAt(0xFF0000, 0, 1000, 250));
}

void test_effect_fade_takes_priority_over_blink(void)
{
    // Both fade and blink set: fade should take priority
    uint32_t result = textEffectAt(0xFF0000, 1000, 1000, 250);
    uint8_t r = (result >> 16) & 0xFF;
    // Fade at quarter period → full brightness
    TEST_ASSERT_EQUAL_UINT8(255, r);
}

void test_effect_blink_boundary(void)
{
    // Exactly at blink/2 boundary
    uint32_t result = textEffectAt(0xFF0000, 0, 1000, 500);
    // 500 % 1000 = 500 > 500 is false → hidden
    TEST_ASSERT_EQUAL_HEX32(0x000000, result);
}

// --- adversarial: NaN, division by zero, degenerate periods ---

void test_fade_interval_zero_no_crash(void)
{
    // interval=0 → division by zero → sin(Inf) → NaN → UB on uint8_t cast.
    // Must return color unchanged, not crash or produce garbage.
    uint32_t result = fadeColorAt(0xFF0000, 0, 500);
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, result);
}

void test_effect_blink_one_returns_color(void)
{
    // blink=1 → blink/2=0 (integer division) → currentMillis%1=0 → 0>0 false → always OFF.
    // Fixed: blink < 2 treated as "no blink" → returns color unchanged.
    uint32_t result = textEffectAt(0xFF0000, 0, 1, 500);
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, result);
}

void test_effect_blink_two_toggles(void)
{
    // blink=2 is the minimum valid blink period. blink/2=1.
    // t=0: 0%2=0, 0>1 false → OFF
    uint32_t off = textEffectAt(0xFF0000, 0, 2, 0);
    TEST_ASSERT_EQUAL_HEX32(0x000000, off);
    // t=1: 1%2=1, 1>1 false → OFF (boundary)
    uint32_t boundary = textEffectAt(0xFF0000, 0, 2, 1);
    TEST_ASSERT_EQUAL_HEX32(0x000000, boundary);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // fadeColorAt
    RUN_TEST(test_fade_at_zero_phase);
    RUN_TEST(test_fade_at_quarter_period);
    RUN_TEST(test_fade_at_three_quarter_period);
    RUN_TEST(test_fade_preserves_color_channels);
    RUN_TEST(test_fade_black_stays_black);

    // textEffectAt
    RUN_TEST(test_effect_no_fade_no_blink);
    RUN_TEST(test_effect_fade_at_quarter);
    RUN_TEST(test_effect_blink_visible);
    RUN_TEST(test_effect_blink_hidden);
    RUN_TEST(test_effect_fade_takes_priority_over_blink);
    RUN_TEST(test_effect_blink_boundary);

    // adversarial
    RUN_TEST(test_fade_interval_zero_no_crash);
    RUN_TEST(test_effect_blink_one_returns_color);
    RUN_TEST(test_effect_blink_two_toggles);

    return UNITY_END();
}
