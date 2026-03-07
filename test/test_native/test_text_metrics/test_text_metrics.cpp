#include <unity.h>
#include "TextUtils.h"

// --- getTextWidth ---

void test_width_empty_string(void)
{
    TEST_ASSERT_EQUAL_FLOAT(0.0f, getTextWidth("", 0, false));
}

void test_width_single_space(void)
{
    // Space (32) has width 2 in CharMap
    TEST_ASSERT_EQUAL_FLOAT(2.0f, getTextWidth(" ", 0, false));
}

void test_width_letter_A(void)
{
    // 'A' (65) has width 4
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("A", 0, false));
}

void test_width_letter_I(void)
{
    // 'I' (73) has width 2 — narrow character
    TEST_ASSERT_EQUAL_FLOAT(2.0f, getTextWidth("I", 0, false));
}

void test_width_letter_M(void)
{
    // 'M' (77) has width 6 — wide character
    TEST_ASSERT_EQUAL_FLOAT(6.0f, getTextWidth("M", 0, false));
}

void test_width_letter_W(void)
{
    // 'W' (87) has width 6
    TEST_ASSERT_EQUAL_FLOAT(6.0f, getTextWidth("W", 0, false));
}

void test_width_hello(void)
{
    // H=4, e=4, l=4, l=4, o=4 = 20
    TEST_ASSERT_EQUAL_FLOAT(20.0f, getTextWidth("Hello", 0, false));
}

void test_width_digits(void)
{
    // '1'=4, '2'=4, '3'=4 = 12
    TEST_ASSERT_EQUAL_FLOAT(12.0f, getTextWidth("123", 0, false));
}

void test_width_mixed(void)
{
    // 'H'=4, 'i'=2, '!'=2 = 8
    TEST_ASSERT_EQUAL_FLOAT(8.0f, getTextWidth("Hi!", 0, false));
}

// --- textCase and uppercaseLetters ---

void test_width_uppercase_forced(void)
{
    // textCase=1 forces uppercase. 'a' becomes 'A' (width 4)
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("a", 1, false));
}

void test_width_uppercase_global(void)
{
    // uppercaseLetters=true, textCase=0 forces uppercase
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("a", 0, true));
}

void test_width_no_uppercase(void)
{
    // uppercaseLetters=false, textCase=0 keeps lowercase
    // 'a' (97) has width 4 in CharMap
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("a", 0, false));
}

// --- Special width chars (not in CharMap) ---

void test_width_unknown_char_default(void)
{
    // Characters not in CharMap and not in special lists get width 4
    char text[2] = {(char)0x01, '\0'};
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth(text, 0, false));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_width_empty_string);
    RUN_TEST(test_width_single_space);
    RUN_TEST(test_width_letter_A);
    RUN_TEST(test_width_letter_I);
    RUN_TEST(test_width_letter_M);
    RUN_TEST(test_width_letter_W);
    RUN_TEST(test_width_hello);
    RUN_TEST(test_width_digits);
    RUN_TEST(test_width_mixed);

    RUN_TEST(test_width_uppercase_forced);
    RUN_TEST(test_width_uppercase_global);
    RUN_TEST(test_width_no_uppercase);

    RUN_TEST(test_width_unknown_char_default);

    return UNITY_END();
}
