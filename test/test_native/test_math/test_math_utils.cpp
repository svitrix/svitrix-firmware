#include <unity.h>
#include "MathUtils.h"

// --- roundToDecimalPlaces ---

void test_round_two_places(void)
{
    TEST_ASSERT_EQUAL_FLOAT(3.14, roundToDecimalPlaces(3.14159, 2));
}

void test_round_zero_places(void)
{
    TEST_ASSERT_EQUAL_FLOAT(3.0, roundToDecimalPlaces(3.14159, 0));
}

void test_round_one_place(void)
{
    TEST_ASSERT_EQUAL_FLOAT(3.1, roundToDecimalPlaces(3.14159, 1));
}

void test_round_negative_value(void)
{
    TEST_ASSERT_EQUAL_FLOAT(-2.72, roundToDecimalPlaces(-2.718, 2));
}

void test_round_zero_value(void)
{
    TEST_ASSERT_EQUAL_FLOAT(0.0, roundToDecimalPlaces(0.0, 3));
}

void test_round_three_places(void)
{
    TEST_ASSERT_EQUAL_FLOAT(3.142, roundToDecimalPlaces(3.14159, 3));
}

void test_round_large_number(void)
{
    TEST_ASSERT_EQUAL_FLOAT(12345.68, roundToDecimalPlaces(12345.6789, 2));
}

void test_round_half_up(void)
{
    TEST_ASSERT_EQUAL_FLOAT(2.5, roundToDecimalPlaces(2.45, 1));
}

void test_round_very_small(void)
{
    TEST_ASSERT_EQUAL_FLOAT(0.001, roundToDecimalPlaces(0.0009999, 3));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_round_two_places);
    RUN_TEST(test_round_zero_places);
    RUN_TEST(test_round_one_place);
    RUN_TEST(test_round_negative_value);
    RUN_TEST(test_round_zero_value);
    RUN_TEST(test_round_three_places);
    RUN_TEST(test_round_large_number);
    RUN_TEST(test_round_half_up);
    RUN_TEST(test_round_very_small);

    return UNITY_END();
}
