#include <unity.h>
#include "SensorCalc.h"

// --- calculateBatteryPercent ---

void test_battery_at_min(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, calculateBatteryPercent(475, 475, 665));
}

void test_battery_at_max(void)
{
    TEST_ASSERT_EQUAL_UINT8(100, calculateBatteryPercent(665, 475, 665));
}

void test_battery_midpoint(void)
{
    uint16_t mid = (475 + 665) / 2; // 570
    uint8_t result = calculateBatteryPercent(mid, 475, 665);
    TEST_ASSERT_UINT8_WITHIN(2, 50, result);
}

void test_battery_below_min_clamps(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, calculateBatteryPercent(400, 475, 665));
}

void test_battery_above_max_clamps(void)
{
    TEST_ASSERT_EQUAL_UINT8(100, calculateBatteryPercent(800, 475, 665));
}

void test_battery_custom_range(void)
{
    // Range [0..1000], value 500 → 50%
    TEST_ASSERT_EQUAL_UINT8(50, calculateBatteryPercent(500, 0, 1000));
}

// --- calculateBrightness ---

void test_brightness_ldr_zero(void)
{
    uint8_t result = calculateBrightness(0, 1.0f, 3.0f, 2, 160);
    TEST_ASSERT_EQUAL_UINT8(2, result);
}

void test_brightness_ldr_max(void)
{
    uint8_t result = calculateBrightness(1023, 1.0f, 3.0f, 2, 160);
    TEST_ASSERT_EQUAL_UINT8(160, result);
}

void test_brightness_nonlinear_gamma3(void)
{
    // With gamma=3.0, midpoint LDR should produce brightness much lower than midpoint
    // because gamma curve is steep at the bottom
    uint8_t result = calculateBrightness(512, 1.0f, 3.0f, 2, 160);
    uint8_t linear_mid = (2 + 160) / 2; // 81
    TEST_ASSERT_TRUE(result < linear_mid);
}

void test_brightness_linear_gamma1(void)
{
    // With gamma=1.0, result should be near-linear
    uint8_t result = calculateBrightness(512, 1.0f, 1.0f, 0, 100);
    TEST_ASSERT_UINT8_WITHIN(3, 50, result);
}

void test_brightness_factor_zero(void)
{
    // ldrFactor=0 means all readings become 0% → minBrightness
    uint8_t result = calculateBrightness(1023, 0.0f, 3.0f, 2, 160);
    TEST_ASSERT_EQUAL_UINT8(2, result);
}

void test_brightness_output_in_range(void)
{
    // All LDR values should produce brightness in [minBrightness..maxBrightness]
    for (uint16_t ldr = 0; ldr <= 1023; ldr += 100)
    {
        uint8_t result = calculateBrightness(ldr, 1.0f, 3.0f, 2, 160);
        TEST_ASSERT_TRUE(result >= 2);
        TEST_ASSERT_TRUE(result <= 160);
    }
}

// --- applyLdrInversion ---

void test_ldr_no_inversion(void)
{
    TEST_ASSERT_EQUAL_UINT16(500, applyLdrInversion(500, false));
}

void test_ldr_inversion_zero(void)
{
    TEST_ASSERT_EQUAL_UINT16(1023, applyLdrInversion(0, true));
}

void test_ldr_inversion_max(void)
{
    TEST_ASSERT_EQUAL_UINT16(0, applyLdrInversion(1023, true));
}

void test_ldr_inversion_mid(void)
{
    TEST_ASSERT_EQUAL_UINT16(523, applyLdrInversion(500, true));
}

// --- applySensorOffset ---

void test_offset_positive(void)
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.5f, applySensorOffset(23.0f, 2.5f));
}

void test_offset_negative(void)
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 14.0f, applySensorOffset(23.0f, -9.0f));
}

void test_offset_zero(void)
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 23.0f, applySensorOffset(23.0f, 0.0f));
}

void test_offset_negative_temp(void)
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, -12.0f, applySensorOffset(-3.0f, -9.0f));
}

// --- scaleVolume ---

void test_volume_zero(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, scaleVolume(0, 30, 255));
}

void test_volume_max(void)
{
    TEST_ASSERT_EQUAL_UINT8(255, scaleVolume(30, 30, 255));
}

void test_volume_mid(void)
{
    uint8_t result = scaleVolume(15, 30, 255);
    TEST_ASSERT_UINT8_WITHIN(2, 127, result);
}

void test_volume_one(void)
{
    TEST_ASSERT_EQUAL_UINT8(8, scaleVolume(1, 30, 255));
}

// --- adversarial: division by zero, overflow, degenerate inputs ---

void test_battery_equal_range_no_crash(void)
{
    // minBattery == maxBattery → map() macro divides by zero without guard.
    // Must return 0 safely, not crash.
    TEST_ASSERT_EQUAL_UINT8(0, calculateBatteryPercent(500, 500, 500));
}

void test_battery_inverted_range_no_crash(void)
{
    // minBattery > maxBattery → degenerate. Must return 0.
    TEST_ASSERT_EQUAL_UINT8(0, calculateBatteryPercent(500, 700, 400));
}

void test_brightness_high_factor_clamped(void)
{
    // ldrFactor=2.0 → brightnessPercent=200% before clamp.
    // Without clamp: pow(200,3)/pow(100,2)=800, map(800,0,100,2,160)=1266 → uint8_t wraps.
    // With clamp: must stay within [minBrightness..maxBrightness].
    uint8_t result = calculateBrightness(1023, 2.0f, 3.0f, 2, 160);
    TEST_ASSERT_EQUAL_UINT8(160, result);
}

void test_brightness_high_factor_in_range_sweep(void)
{
    // All LDR values with ldrFactor=1.5 must stay in [2..160].
    for (uint16_t ldr = 0; ldr <= 1023; ldr += 100)
    {
        uint8_t result = calculateBrightness(ldr, 1.5f, 3.0f, 2, 160);
        TEST_ASSERT_TRUE(result >= 2);
        TEST_ASSERT_TRUE(result <= 160);
    }
}

void test_volume_input_max_zero_no_crash(void)
{
    // inputMax=0 → division by zero without guard. Must return 0.
    TEST_ASSERT_EQUAL_UINT8(0, scaleVolume(15, 0, 255));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // calculateBatteryPercent
    RUN_TEST(test_battery_at_min);
    RUN_TEST(test_battery_at_max);
    RUN_TEST(test_battery_midpoint);
    RUN_TEST(test_battery_below_min_clamps);
    RUN_TEST(test_battery_above_max_clamps);
    RUN_TEST(test_battery_custom_range);

    // calculateBrightness
    RUN_TEST(test_brightness_ldr_zero);
    RUN_TEST(test_brightness_ldr_max);
    RUN_TEST(test_brightness_nonlinear_gamma3);
    RUN_TEST(test_brightness_linear_gamma1);
    RUN_TEST(test_brightness_factor_zero);
    RUN_TEST(test_brightness_output_in_range);

    // applyLdrInversion
    RUN_TEST(test_ldr_no_inversion);
    RUN_TEST(test_ldr_inversion_zero);
    RUN_TEST(test_ldr_inversion_max);
    RUN_TEST(test_ldr_inversion_mid);

    // applySensorOffset
    RUN_TEST(test_offset_positive);
    RUN_TEST(test_offset_negative);
    RUN_TEST(test_offset_zero);
    RUN_TEST(test_offset_negative_temp);

    // scaleVolume
    RUN_TEST(test_volume_zero);
    RUN_TEST(test_volume_max);
    RUN_TEST(test_volume_mid);
    RUN_TEST(test_volume_one);

    // adversarial
    RUN_TEST(test_battery_equal_range_no_crash);
    RUN_TEST(test_battery_inverted_range_no_crash);
    RUN_TEST(test_brightness_high_factor_clamped);
    RUN_TEST(test_brightness_high_factor_in_range_sweep);
    RUN_TEST(test_volume_input_max_zero_no_crash);

    return UNITY_END();
}
