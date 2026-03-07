#include <unity.h>
#include <cstdlib>
#include <cstdint>
#include <cmath>

// Pure functions extracted from src/Functions.cpp (no Arduino dependencies)

uint32_t hexToUint32(const char *hexString)
{
    uint32_t rgbValue = (uint32_t)strtol(hexString, NULL, 16);
    return rgbValue;
}

double roundToDecimalPlaces(double value, int places)
{
    double factor = pow(10.0, places);
    return round(value * factor) / factor;
}

// --- hexToUint32 tests ---

void test_hexToUint32_red(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, hexToUint32("FF0000"));
}

void test_hexToUint32_green(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x00FF00, hexToUint32("00FF00"));
}

void test_hexToUint32_blue(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x0000FF, hexToUint32("0000FF"));
}

void test_hexToUint32_white(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFF, hexToUint32("FFFFFF"));
}

void test_hexToUint32_black(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x000000, hexToUint32("000000"));
}

void test_hexToUint32_arbitrary(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x1A2B3C, hexToUint32("1A2B3C"));
}

// --- roundToDecimalPlaces tests ---

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

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_hexToUint32_red);
    RUN_TEST(test_hexToUint32_green);
    RUN_TEST(test_hexToUint32_blue);
    RUN_TEST(test_hexToUint32_white);
    RUN_TEST(test_hexToUint32_black);
    RUN_TEST(test_hexToUint32_arbitrary);

    RUN_TEST(test_round_two_places);
    RUN_TEST(test_round_zero_places);
    RUN_TEST(test_round_one_place);
    RUN_TEST(test_round_negative_value);
    RUN_TEST(test_round_zero_value);

    return UNITY_END();
}
