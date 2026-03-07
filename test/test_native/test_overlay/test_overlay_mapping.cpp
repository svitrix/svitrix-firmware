#include <unity.h>
#include "OverlayMapping.h"

// --- overlayToString ---

void test_overlay_to_string_none(void)
{
    TEST_ASSERT_EQUAL_STRING("clear", overlayToString(NONE).c_str());
}

void test_overlay_to_string_drizzle(void)
{
    TEST_ASSERT_EQUAL_STRING("drizzle", overlayToString(DRIZZLE).c_str());
}

void test_overlay_to_string_rain(void)
{
    TEST_ASSERT_EQUAL_STRING("rain", overlayToString(RAIN).c_str());
}

void test_overlay_to_string_snow(void)
{
    TEST_ASSERT_EQUAL_STRING("snow", overlayToString(SNOW).c_str());
}

void test_overlay_to_string_storm(void)
{
    TEST_ASSERT_EQUAL_STRING("storm", overlayToString(STORM).c_str());
}

void test_overlay_to_string_thunder(void)
{
    TEST_ASSERT_EQUAL_STRING("thunder", overlayToString(THUNDER).c_str());
}

void test_overlay_to_string_frost(void)
{
    TEST_ASSERT_EQUAL_STRING("frost", overlayToString(FROST).c_str());
}

// --- overlayFromString ---

void test_overlay_from_string_clear(void)
{
    TEST_ASSERT_EQUAL(NONE, overlayFromString("clear"));
}

void test_overlay_from_string_drizzle(void)
{
    TEST_ASSERT_EQUAL(DRIZZLE, overlayFromString("drizzle"));
}

void test_overlay_from_string_rain(void)
{
    TEST_ASSERT_EQUAL(RAIN, overlayFromString("rain"));
}

void test_overlay_from_string_snow(void)
{
    TEST_ASSERT_EQUAL(SNOW, overlayFromString("snow"));
}

void test_overlay_from_string_storm(void)
{
    TEST_ASSERT_EQUAL(STORM, overlayFromString("storm"));
}

void test_overlay_from_string_thunder(void)
{
    TEST_ASSERT_EQUAL(THUNDER, overlayFromString("thunder"));
}

void test_overlay_from_string_frost(void)
{
    TEST_ASSERT_EQUAL(FROST, overlayFromString("frost"));
}

// --- case insensitivity ---

void test_overlay_from_string_uppercase(void)
{
    TEST_ASSERT_EQUAL(RAIN, overlayFromString("RAIN"));
}

void test_overlay_from_string_mixed_case(void)
{
    TEST_ASSERT_EQUAL(SNOW, overlayFromString("Snow"));
}

// --- invalid input ---

void test_overlay_from_string_invalid(void)
{
    TEST_ASSERT_EQUAL(NONE, overlayFromString("unknown"));
}

void test_overlay_from_string_empty(void)
{
    TEST_ASSERT_EQUAL(NONE, overlayFromString(""));
}

// --- round-trip ---

void test_overlay_round_trip(void)
{
    OverlayEffect effects[] = {NONE, DRIZZLE, RAIN, SNOW, STORM, THUNDER, FROST};
    for (int i = 0; i < 7; i++)
    {
        String str = overlayToString(effects[i]);
        OverlayEffect back = overlayFromString(str);
        TEST_ASSERT_EQUAL(effects[i], back);
    }
}

// --- adversarial: out-of-range enum ---

void test_overlay_to_string_invalid_enum(void)
{
    // Cast an out-of-range int to OverlayEffect — should hit default → "invalid".
    TEST_ASSERT_EQUAL_STRING("invalid", overlayToString((OverlayEffect)99).c_str());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // overlayToString
    RUN_TEST(test_overlay_to_string_none);
    RUN_TEST(test_overlay_to_string_drizzle);
    RUN_TEST(test_overlay_to_string_rain);
    RUN_TEST(test_overlay_to_string_snow);
    RUN_TEST(test_overlay_to_string_storm);
    RUN_TEST(test_overlay_to_string_thunder);
    RUN_TEST(test_overlay_to_string_frost);

    // overlayFromString
    RUN_TEST(test_overlay_from_string_clear);
    RUN_TEST(test_overlay_from_string_drizzle);
    RUN_TEST(test_overlay_from_string_rain);
    RUN_TEST(test_overlay_from_string_snow);
    RUN_TEST(test_overlay_from_string_storm);
    RUN_TEST(test_overlay_from_string_thunder);
    RUN_TEST(test_overlay_from_string_frost);

    // case insensitivity
    RUN_TEST(test_overlay_from_string_uppercase);
    RUN_TEST(test_overlay_from_string_mixed_case);

    // invalid input
    RUN_TEST(test_overlay_from_string_invalid);
    RUN_TEST(test_overlay_from_string_empty);

    // round-trip
    RUN_TEST(test_overlay_round_trip);

    // adversarial
    RUN_TEST(test_overlay_to_string_invalid_enum);

    return UNITY_END();
}
