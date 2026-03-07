#include <unity.h>
#include "ColorUtils.h"

// --- hexToUint32 ---

void test_hex_red(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, hexToUint32("FF0000"));
}

void test_hex_green(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x00FF00, hexToUint32("00FF00"));
}

void test_hex_blue(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x0000FF, hexToUint32("0000FF"));
}

void test_hex_white(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFF, hexToUint32("FFFFFF"));
}

void test_hex_black(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x000000, hexToUint32("000000"));
}

void test_hex_arbitrary(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x1A2B3C, hexToUint32("1A2B3C"));
}

void test_hex_lowercase(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xABCDEF, hexToUint32("abcdef"));
}

// --- kelvinToRGB ---

void test_kelvin_1000_warm(void)
{
    CRGB c = kelvinToRGB(1000);
    // Very warm: red should be high, blue should be 0
    TEST_ASSERT_EQUAL_UINT8(255, c.r);
    TEST_ASSERT_EQUAL_UINT8(0, c.b);
}

void test_kelvin_6600_daylight(void)
{
    CRGB c = kelvinToRGB(6600);
    // Daylight: all channels should be near 255
    TEST_ASSERT_GREATER_THAN(200, c.r);
    TEST_ASSERT_GREATER_THAN(200, c.g);
    TEST_ASSERT_EQUAL_UINT8(255, c.b);
}

void test_kelvin_10000_cool(void)
{
    CRGB c = kelvinToRGB(10000);
    // Cool white: blue should be max
    TEST_ASSERT_EQUAL_UINT8(255, c.b);
    // Red should be lower than blue
    TEST_ASSERT_LESS_THAN(c.b, c.r);
}

void test_kelvin_2700_incandescent(void)
{
    CRGB c = kelvinToRGB(2700);
    // Incandescent: red max, green medium, blue low
    TEST_ASSERT_EQUAL_UINT8(255, c.r);
    TEST_ASSERT_GREATER_THAN(100, c.g);
    TEST_ASSERT_LESS_THAN(200, c.b);
}

// --- hsvToRgb ---

void test_hsv_red(void)
{
    // H=0 should produce red
    uint32_t c = hsvToRgb(0, 255, 255);
    uint8_t r = (c >> 16) & 0xFF;
    TEST_ASSERT_GREATER_THAN(200, r);
}

void test_hsv_zero_saturation(void)
{
    // S=0 should produce grey/white
    uint32_t c = hsvToRgb(0, 0, 255);
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8) & 0xFF;
    uint8_t b = c & 0xFF;
    TEST_ASSERT_EQUAL_UINT8(r, g);
    TEST_ASSERT_EQUAL_UINT8(g, b);
}

void test_hsv_zero_value(void)
{
    // V=0 should produce black
    uint32_t c = hsvToRgb(128, 255, 0);
    TEST_ASSERT_EQUAL_HEX32(0x000000, c);
}

// --- interpolateColor ---

void test_interpolate_start(void)
{
    // t=0 should return color1
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, interpolateColor(0xFF0000, 0x0000FF, 0.0f));
}

void test_interpolate_end(void)
{
    // t=1 should return color2
    TEST_ASSERT_EQUAL_HEX32(0x0000FF, interpolateColor(0xFF0000, 0x0000FF, 1.0f));
}

void test_interpolate_midpoint(void)
{
    // t=0.5 between red and blue
    uint32_t mid = interpolateColor(0xFF0000, 0x0000FF, 0.5f);
    uint8_t r = (mid >> 16) & 0xFF;
    uint8_t b = mid & 0xFF;
    // Both should be around 127-128
    TEST_ASSERT_INT_WITHIN(5, 127, r);
    TEST_ASSERT_INT_WITHIN(5, 127, b);
}

void test_interpolate_same_color(void)
{
    TEST_ASSERT_EQUAL_HEX32(0xABCDEF, interpolateColor(0xABCDEF, 0xABCDEF, 0.5f));
}

void test_interpolate_negative_t(void)
{
    // t < 0 should clamp to color1
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, interpolateColor(0xFF0000, 0x0000FF, -0.5f));
}

void test_interpolate_over_one_t(void)
{
    // t > 1 should clamp to color2
    TEST_ASSERT_EQUAL_HEX32(0x0000FF, interpolateColor(0xFF0000, 0x0000FF, 1.5f));
}

void test_interpolate_black_to_white(void)
{
    uint32_t mid = interpolateColor(0x000000, 0xFFFFFF, 0.5f);
    uint8_t r = (mid >> 16) & 0xFF;
    uint8_t g = (mid >> 8) & 0xFF;
    uint8_t b = mid & 0xFF;
    TEST_ASSERT_INT_WITHIN(5, 127, r);
    TEST_ASSERT_INT_WITHIN(5, 127, g);
    TEST_ASSERT_INT_WITHIN(5, 127, b);
}

// --- adversarial: NaN, negative kelvin, channel direction ---

void test_kelvin_zero_no_crash(void)
{
    // kelvin=0 → temperature=0 → log(0)=-Inf → NaN without guard.
    // Must produce valid RGB, not crash.
    CRGB c = kelvinToRGB(0);
    TEST_ASSERT_TRUE(c.r <= 255);
    TEST_ASSERT_TRUE(c.g <= 255);
    TEST_ASSERT_TRUE(c.b <= 255);
}

void test_kelvin_negative_no_crash(void)
{
    // kelvin=-500 → temperature=-5 → log(negative)=NaN → UB on uint8_t cast.
    // Must produce valid RGB.
    CRGB c = kelvinToRGB(-500);
    TEST_ASSERT_TRUE(c.r <= 255);
    TEST_ASSERT_TRUE(c.g <= 255);
    TEST_ASSERT_TRUE(c.b <= 255);
}

void test_interpolate_white_to_black_midpoint(void)
{
    // High-to-low interpolation: all channels decrease from 255 to 0.
    // Verifies uint8_t subtraction doesn't underflow (relies on int promotion).
    uint32_t mid = interpolateColor(0xFFFFFF, 0x000000, 0.5f);
    uint8_t r = (mid >> 16) & 0xFF;
    uint8_t g = (mid >> 8) & 0xFF;
    uint8_t b = mid & 0xFF;
    TEST_ASSERT_INT_WITHIN(2, 127, r);
    TEST_ASSERT_INT_WITHIN(2, 127, g);
    TEST_ASSERT_INT_WITHIN(2, 127, b);
}

void test_interpolate_high_to_low_quarter(void)
{
    // t=0.25: from white(255) toward black(0).
    // Expected per channel: 255 + (0-255)*0.25 = 255-63.75 = 191.25 → 191
    uint32_t result = interpolateColor(0xFFFFFF, 0x000000, 0.25f);
    uint8_t r = (result >> 16) & 0xFF;
    TEST_ASSERT_INT_WITHIN(2, 191, r);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // hexToUint32
    RUN_TEST(test_hex_red);
    RUN_TEST(test_hex_green);
    RUN_TEST(test_hex_blue);
    RUN_TEST(test_hex_white);
    RUN_TEST(test_hex_black);
    RUN_TEST(test_hex_arbitrary);
    RUN_TEST(test_hex_lowercase);

    // kelvinToRGB
    RUN_TEST(test_kelvin_1000_warm);
    RUN_TEST(test_kelvin_6600_daylight);
    RUN_TEST(test_kelvin_10000_cool);
    RUN_TEST(test_kelvin_2700_incandescent);

    // hsvToRgb
    RUN_TEST(test_hsv_red);
    RUN_TEST(test_hsv_zero_saturation);
    RUN_TEST(test_hsv_zero_value);

    // interpolateColor
    RUN_TEST(test_interpolate_start);
    RUN_TEST(test_interpolate_end);
    RUN_TEST(test_interpolate_midpoint);
    RUN_TEST(test_interpolate_same_color);
    RUN_TEST(test_interpolate_negative_t);
    RUN_TEST(test_interpolate_over_one_t);
    RUN_TEST(test_interpolate_black_to_white);

    // adversarial
    RUN_TEST(test_kelvin_zero_no_crash);
    RUN_TEST(test_kelvin_negative_no_crash);
    RUN_TEST(test_interpolate_white_to_black_midpoint);
    RUN_TEST(test_interpolate_high_to_low_quarter);

    return UNITY_END();
}
