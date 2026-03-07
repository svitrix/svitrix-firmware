#include <unity.h>
#include "MathUtils.h"

// logMap(x, in_min, in_max, out_min, out_max, mid_point_out)
// Maps x from [in_min..in_max] to [out_min..out_max] with logarithmic curve
// mid_point_out controls the midpoint of the output range

void test_logmap_at_in_min(void)
{
    // x == in_min should return out_min
    float result = logMap(2.0f, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.535f, result);
}

void test_logmap_at_in_max(void)
{
    // x == in_max should return out_max
    float result = logMap(180.0f, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 2.3f, result);
}

void test_logmap_below_in_min(void)
{
    // x < in_min should clamp to out_min
    float result = logMap(0.0f, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.535f, result);
}

void test_logmap_above_in_max(void)
{
    // x > in_max should clamp to out_max
    float result = logMap(500.0f, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.3f, result);
}

void test_logmap_output_within_range(void)
{
    // Any x in range should produce output in [out_min..out_max]
    for (float x = 2.0f; x <= 180.0f; x += 10.0f)
    {
        float result = logMap(x, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
        TEST_ASSERT_TRUE(result >= 0.535f - 0.001f);
        TEST_ASSERT_TRUE(result <= 2.3f + 0.001f);
    }
}

void test_logmap_monotonically_increasing(void)
{
    // logMap should be monotonically increasing
    float prev = logMap(2.0f, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
    for (float x = 10.0f; x <= 180.0f; x += 10.0f)
    {
        float curr = logMap(x, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
        TEST_ASSERT_TRUE(curr >= prev);
        prev = curr;
    }
}

void test_logmap_midpoint_in_upper_half(void)
{
    // mid_point_out=1.9 is close to out_max=2.3, so the curve rises fast
    // At midpoint of input (x=91), output should already be well above the
    // linear midpoint (0.535+2.3)/2 = 1.4175
    float mid_x = (2.0f + 180.0f) / 2.0f; // 91
    float result = logMap(mid_x, 2.0f, 180.0f, 0.535f, 2.3f, 1.9f);
    float linear_mid = (0.535f + 2.3f) / 2.0f;
    TEST_ASSERT_TRUE(result > linear_mid);
    TEST_ASSERT_TRUE(result < 2.3f);
}

void test_logmap_simple_range(void)
{
    // Simple test: map [0..100] to [0..10] with midpoint 5
    float at_min = logMap(0.0f, 0.0f, 100.0f, 0.0f, 10.0f, 5.0f);
    float at_max = logMap(100.0f, 0.0f, 100.0f, 0.0f, 10.0f, 5.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, at_min);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 10.0f, at_max);
}

// --- adversarial: division by zero / degenerate ranges ---

void test_logmap_equal_range_no_crash(void)
{
    // in_min == in_max → log(1)=0 → would divide by zero without guard.
    // Must return out_min safely.
    float result = logMap(5.0f, 5.0f, 5.0f, 0.0f, 10.0f, 5.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, result);
}

void test_logmap_inverted_range_no_crash(void)
{
    // in_min > in_max → degenerate. Must return out_min.
    float result = logMap(50.0f, 100.0f, 10.0f, 0.0f, 10.0f, 5.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, result);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_logmap_at_in_min);
    RUN_TEST(test_logmap_at_in_max);
    RUN_TEST(test_logmap_below_in_min);
    RUN_TEST(test_logmap_above_in_max);
    RUN_TEST(test_logmap_output_within_range);
    RUN_TEST(test_logmap_monotonically_increasing);
    RUN_TEST(test_logmap_midpoint_in_upper_half);
    RUN_TEST(test_logmap_simple_range);

    // adversarial
    RUN_TEST(test_logmap_equal_range_no_crash);
    RUN_TEST(test_logmap_inverted_range_no_crash);

    return UNITY_END();
}
