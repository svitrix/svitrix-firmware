#include <unity.h>
#include "LayoutEngine.h"

// --- computeLayout: Left ---

void test_left_icon_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_EQUAL_INT16(0, m.iconX);
}

void test_left_text_start_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_EQUAL_INT16(9, m.textStartX);
}

void test_left_available_width(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_EQUAL_UINT16(24, m.textAvailableWidth);
}

void test_left_has_icon(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 20);
    TEST_ASSERT_TRUE(m.hasIcon);
    TEST_ASSERT_FALSE(m.iconOnRight);
}

void test_left_text_center(void)
{
    // text 10px wide in 24px space: (24-10)/2 + 9 = 16
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 10);
    TEST_ASSERT_EQUAL_INT16(16, m.textCenterX);
}

// --- computeLayout: Right ---

void test_right_icon_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_EQUAL_INT16(24, m.iconX);
}

void test_right_text_start_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_EQUAL_INT16(0, m.textStartX);
}

void test_right_available_width(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_EQUAL_UINT16(24, m.textAvailableWidth);
}

void test_right_has_icon(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 20);
    TEST_ASSERT_TRUE(m.hasIcon);
    TEST_ASSERT_TRUE(m.iconOnRight);
}

void test_right_text_center(void)
{
    // text 10px wide in 24px space: (24-10)/2 = 7
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 10);
    TEST_ASSERT_EQUAL_INT16(7, m.textCenterX);
}

// --- computeLayout: None ---

void test_none_has_no_icon(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 20);
    TEST_ASSERT_FALSE(m.hasIcon);
    TEST_ASSERT_EQUAL_INT16(-1, m.iconX);
}

void test_none_text_start_x(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 20);
    TEST_ASSERT_EQUAL_INT16(0, m.textStartX);
}

void test_none_available_width(void)
{
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 20);
    TEST_ASSERT_EQUAL_UINT16(32, m.textAvailableWidth);
}

void test_none_text_center(void)
{
    // text 10px wide in 32px space: (32-10)/2 = 11
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::None, 10);
    TEST_ASSERT_EQUAL_INT16(11, m.textCenterX);
}

// --- Custom icon width ---

void test_custom_icon_width_left(void)
{
    // GIF with 10px width: iconX=0, textStartX=11, avail=32-10=22
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Left, 15, 10);
    TEST_ASSERT_EQUAL_INT16(0, m.iconX);
    TEST_ASSERT_EQUAL_INT16(11, m.textStartX);
    TEST_ASSERT_EQUAL_UINT16(22, m.textAvailableWidth);
}

void test_custom_icon_width_right(void)
{
    // GIF with 10px width: iconX=32-10=22, textStartX=0, avail=32-10=22
    LayoutMetrics m = LayoutEngine::computeLayout(IconLayout::Right, 15, 10);
    TEST_ASSERT_EQUAL_INT16(22, m.iconX);
    TEST_ASSERT_EQUAL_INT16(0, m.textStartX);
    TEST_ASSERT_EQUAL_UINT16(22, m.textAvailableWidth);
}

// --- Default is Left ---

void test_default_enum_value(void)
{
    IconLayout layout = {};
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layout));
}

// --- layoutToString / layoutFromString ---

void test_layout_to_string_left(void)
{
    TEST_ASSERT_EQUAL_STRING("left", layoutToString(IconLayout::Left).c_str());
}

void test_layout_to_string_right(void)
{
    TEST_ASSERT_EQUAL_STRING("right", layoutToString(IconLayout::Right).c_str());
}

void test_layout_to_string_none(void)
{
    TEST_ASSERT_EQUAL_STRING("none", layoutToString(IconLayout::None).c_str());
}

void test_layout_from_string_left(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layoutFromString("left")));
}

void test_layout_from_string_right(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Right),
                            static_cast<uint8_t>(layoutFromString("right")));
}

void test_layout_from_string_none(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::None),
                            static_cast<uint8_t>(layoutFromString("none")));
}

void test_layout_from_string_unknown_defaults_left(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layoutFromString("garbage")));
}

void test_layout_from_string_empty_defaults_left(void)
{
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(IconLayout::Left),
                            static_cast<uint8_t>(layoutFromString("")));
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Left
    RUN_TEST(test_left_icon_x);
    RUN_TEST(test_left_text_start_x);
    RUN_TEST(test_left_available_width);
    RUN_TEST(test_left_has_icon);
    RUN_TEST(test_left_text_center);

    // Right
    RUN_TEST(test_right_icon_x);
    RUN_TEST(test_right_text_start_x);
    RUN_TEST(test_right_available_width);
    RUN_TEST(test_right_has_icon);
    RUN_TEST(test_right_text_center);

    // None
    RUN_TEST(test_none_has_no_icon);
    RUN_TEST(test_none_text_start_x);
    RUN_TEST(test_none_available_width);
    RUN_TEST(test_none_text_center);

    // Custom icon width
    RUN_TEST(test_custom_icon_width_left);
    RUN_TEST(test_custom_icon_width_right);

    // Default
    RUN_TEST(test_default_enum_value);

    // String conversion
    RUN_TEST(test_layout_to_string_left);
    RUN_TEST(test_layout_to_string_right);
    RUN_TEST(test_layout_to_string_none);
    RUN_TEST(test_layout_from_string_left);
    RUN_TEST(test_layout_from_string_right);
    RUN_TEST(test_layout_from_string_none);
    RUN_TEST(test_layout_from_string_unknown_defaults_left);
    RUN_TEST(test_layout_from_string_empty_defaults_left);

    return UNITY_END();
}
