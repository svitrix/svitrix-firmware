#include <unity.h>
#include "GammaUtils.h"

// --- boundary values ---

void test_gamma_at_min_brightness(void)
{
    // brightness=2 is the lower bound → gamma should be near out_min (0.535)
    float gamma = calculateGamma(2.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.05, 0.535, gamma);
}

void test_gamma_at_max_brightness(void)
{
    // brightness=180 is the upper bound → gamma should be near out_max (2.3)
    float gamma = calculateGamma(180.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.05, 2.3, gamma);
}

void test_gamma_at_midpoint(void)
{
    // brightness=90 (midpoint) → gamma should be near mid_point_out (1.9)
    float gamma = calculateGamma(90.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.2, 1.9, gamma);
}

// --- out-of-range inputs ---

void test_gamma_below_range_returns_min(void)
{
    // brightness=0 < in_min=2 → returns out_min (0.535)
    float gamma = calculateGamma(0.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.535, gamma);
}

void test_gamma_above_range_returns_max(void)
{
    // brightness=255 > in_max=180 → returns out_max (2.3)
    float gamma = calculateGamma(255.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 2.3, gamma);
}

void test_gamma_negative_returns_min(void)
{
    // Negative brightness → logMap guard returns out_min
    float gamma = calculateGamma(-10.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.535, gamma);
}

// --- monotonicity: gamma increases with brightness ---

void test_gamma_monotonically_increasing(void)
{
    float prev = calculateGamma(2.0f);
    for (float bri = 10.0f; bri <= 180.0f; bri += 10.0f)
    {
        float curr = calculateGamma(bri);
        TEST_ASSERT_TRUE_MESSAGE(curr >= prev, "Gamma must increase with brightness");
        prev = curr;
    }
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // boundary values
    RUN_TEST(test_gamma_at_min_brightness);
    RUN_TEST(test_gamma_at_max_brightness);
    RUN_TEST(test_gamma_at_midpoint);

    // out-of-range
    RUN_TEST(test_gamma_below_range_returns_min);
    RUN_TEST(test_gamma_above_range_returns_max);
    RUN_TEST(test_gamma_negative_returns_min);

    // monotonicity
    RUN_TEST(test_gamma_monotonically_increasing);

    return UNITY_END();
}
