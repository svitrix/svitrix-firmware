#include <unity.h>
#include "ColorUtils.h"

void test_crgb_hex_red(void)
{
    CRGB color(255, 0, 0);
    String result = CRGBtoHex(color);
    TEST_ASSERT_EQUAL_STRING("#FF0000", result.c_str());
}

void test_crgb_hex_green(void)
{
    CRGB color(0, 255, 0);
    String result = CRGBtoHex(color);
    TEST_ASSERT_EQUAL_STRING("#00FF00", result.c_str());
}

void test_crgb_hex_blue(void)
{
    CRGB color(0, 0, 255);
    String result = CRGBtoHex(color);
    TEST_ASSERT_EQUAL_STRING("#0000FF", result.c_str());
}

void test_crgb_hex_white(void)
{
    CRGB color(255, 255, 255);
    String result = CRGBtoHex(color);
    TEST_ASSERT_EQUAL_STRING("#FFFFFF", result.c_str());
}

void test_crgb_hex_black(void)
{
    CRGB color(0, 0, 0);
    String result = CRGBtoHex(color);
    TEST_ASSERT_EQUAL_STRING("#000000", result.c_str());
}

void test_crgb_hex_arbitrary(void)
{
    CRGB color(0x1A, 0x2B, 0x3C);
    String result = CRGBtoHex(color);
    TEST_ASSERT_EQUAL_STRING("#1A2B3C", result.c_str());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_crgb_hex_red);
    RUN_TEST(test_crgb_hex_green);
    RUN_TEST(test_crgb_hex_blue);
    RUN_TEST(test_crgb_hex_white);
    RUN_TEST(test_crgb_hex_black);
    RUN_TEST(test_crgb_hex_arbitrary);

    return UNITY_END();
}
