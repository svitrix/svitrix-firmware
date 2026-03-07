#include <unity.h>
#include "MeanFilterLib.h"

void test_single_value(void)
{
    MeanFilter<uint16_t> f(3);
    TEST_ASSERT_EQUAL_UINT16(42, f.AddValue(42));
}

void test_average_two_values(void)
{
    MeanFilter<uint16_t> f(3);
    f.AddValue(10);
    TEST_ASSERT_EQUAL_UINT16(15, f.AddValue(20)); // (10+20)/2 = 15
}

void test_full_window(void)
{
    MeanFilter<uint16_t> f(3);
    f.AddValue(10);
    f.AddValue(20);
    TEST_ASSERT_EQUAL_UINT16(20, f.AddValue(30)); // (10+20+30)/3 = 20
}

void test_sliding_window_evicts_oldest(void)
{
    MeanFilter<uint16_t> f(3);
    f.AddValue(10);
    f.AddValue(20);
    f.AddValue(30);
    // Window: [10, 20, 30] → add 60, evicts 10 → [20, 30, 60]
    TEST_ASSERT_EQUAL_UINT16(36, f.AddValue(60)); // (20+30+60)/3 = 36
}

void test_sliding_window_continues(void)
{
    MeanFilter<uint16_t> f(3);
    f.AddValue(10);
    f.AddValue(20);
    f.AddValue(30);
    f.AddValue(60); // [20, 30, 60]
    // Add 90, evicts 20 → [30, 60, 90]
    TEST_ASSERT_EQUAL_UINT16(60, f.AddValue(90)); // (30+60+90)/3 = 60
}

void test_get_filtered_matches_last_add(void)
{
    MeanFilter<uint16_t> f(3);
    f.AddValue(10);
    f.AddValue(20);
    uint16_t result = f.AddValue(30);
    TEST_ASSERT_EQUAL_UINT16(result, f.GetFiltered());
}

void test_window_size_one(void)
{
    MeanFilter<uint16_t> f(1);
    TEST_ASSERT_EQUAL_UINT16(100, f.AddValue(100));
    TEST_ASSERT_EQUAL_UINT16(200, f.AddValue(200));
    TEST_ASSERT_EQUAL_UINT16(50, f.AddValue(50));
}

void test_constant_input(void)
{
    MeanFilter<uint16_t> f(5);
    for (int i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQUAL_UINT16(42, f.AddValue(42));
    }
}

void test_zero_values(void)
{
    MeanFilter<uint16_t> f(3);
    TEST_ASSERT_EQUAL_UINT16(0, f.AddValue(0));
    TEST_ASSERT_EQUAL_UINT16(0, f.AddValue(0));
    TEST_ASSERT_EQUAL_UINT16(0, f.AddValue(0));
}

void test_large_window(void)
{
    MeanFilter<uint16_t> f(10);
    uint16_t sum = 0;
    for (uint16_t i = 1; i <= 10; i++)
    {
        sum += i;
        TEST_ASSERT_EQUAL_UINT16(sum / i, f.AddValue(i));
    }
    // Window full [1..10], avg = 55/10 = 5
    TEST_ASSERT_EQUAL_UINT16(5, f.GetFiltered());
}

void test_wrap_around_multiple_cycles(void)
{
    MeanFilter<uint16_t> f(3);
    // Fill and cycle through multiple wrap-arounds
    for (int cycle = 0; cycle < 5; cycle++)
    {
        f.AddValue(10);
        f.AddValue(20);
        f.AddValue(30);
    }
    // Last window: [10, 20, 30]
    TEST_ASSERT_EQUAL_UINT16(20, f.GetFiltered());
}

void test_int_type(void)
{
    MeanFilter<int> f(3);
    f.AddValue(-10);
    f.AddValue(20);
    TEST_ASSERT_EQUAL_INT(6, f.AddValue(10)); // (-10+20+10)/3 = 6 int division
}

void test_step_response(void)
{
    MeanFilter<uint16_t> f(3);
    // Start with zeros
    f.AddValue(0);
    f.AddValue(0);
    f.AddValue(0);
    // Step to 100
    TEST_ASSERT_EQUAL_UINT16(33, f.AddValue(100));  // (0+0+100)/3 = 33
    TEST_ASSERT_EQUAL_UINT16(66, f.AddValue(100));   // (0+100+100)/3 = 66
    TEST_ASSERT_EQUAL_UINT16(100, f.AddValue(100));  // (100+100+100)/3 = 100
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_single_value);
    RUN_TEST(test_average_two_values);
    RUN_TEST(test_full_window);
    RUN_TEST(test_sliding_window_evicts_oldest);
    RUN_TEST(test_sliding_window_continues);
    RUN_TEST(test_get_filtered_matches_last_add);
    RUN_TEST(test_window_size_one);
    RUN_TEST(test_constant_input);
    RUN_TEST(test_zero_values);
    RUN_TEST(test_large_window);
    RUN_TEST(test_wrap_around_multiple_cycles);
    RUN_TEST(test_int_type);
    RUN_TEST(test_step_response);

    return UNITY_END();
}
