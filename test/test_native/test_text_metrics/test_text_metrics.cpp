#include <unity.h>
#include "TextUtils.h"
#include "SvitrixFont.h"

void setUp(void)
{
    setTextFont(&SvitrixFont);
}
void tearDown(void) {}

// --- getTextWidth (ASCII) ---

void test_width_empty_string(void)
{
    TEST_ASSERT_EQUAL_FLOAT(0.0f, getTextWidth("", 0, false));
}

void test_width_single_space(void)
{
    // Space xAdvance = 2
    TEST_ASSERT_EQUAL_FLOAT(2.0f, getTextWidth(" ", 0, false));
}

void test_width_letter_A(void)
{
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("A", 0, false));
}

void test_width_letter_I(void)
{
    // 'I' xAdvance = 2 (narrow)
    TEST_ASSERT_EQUAL_FLOAT(2.0f, getTextWidth("I", 0, false));
}

void test_width_letter_M(void)
{
    // 'M' xAdvance = 6 (wide)
    TEST_ASSERT_EQUAL_FLOAT(6.0f, getTextWidth("M", 0, false));
}

void test_width_letter_W(void)
{
    TEST_ASSERT_EQUAL_FLOAT(6.0f, getTextWidth("W", 0, false));
}

void test_width_hello(void)
{
    // H=4, e=4, l=4, l=4, o=4 = 20
    TEST_ASSERT_EQUAL_FLOAT(20.0f, getTextWidth("Hello", 0, false));
}

void test_width_digits(void)
{
    // 1=4, 2=4, 3=4 = 12
    TEST_ASSERT_EQUAL_FLOAT(12.0f, getTextWidth("123", 0, false));
}

void test_width_mixed(void)
{
    // H=4, i=2, !=2 = 8
    TEST_ASSERT_EQUAL_FLOAT(8.0f, getTextWidth("Hi!", 0, false));
}

// --- textCase and uppercaseLetters ---

void test_width_uppercase_forced(void)
{
    // textCase=1: 'a' → 'A' (width 4)
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("a", 1, false));
}

void test_width_uppercase_global(void)
{
    // uppercaseLetters=true, textCase=0: 'a' → 'A'
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("a", 0, true));
}

void test_width_no_uppercase(void)
{
    // 'a' lowercase xAdvance = 4
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("a", 0, false));
}

// --- UTF-8 Cyrillic ---

void test_width_cyrillic_A(void)
{
    // А (U+0410) xAdvance = 4
    TEST_ASSERT_EQUAL_FLOAT(4.0f, getTextWidth("\xD0\x90", 0, false));
}

void test_width_cyrillic_Sh(void)
{
    // Ш (U+0428) xAdvance = 6 (wide)
    TEST_ASSERT_EQUAL_FLOAT(6.0f, getTextWidth("\xD0\xA8", 0, false));
}

void test_width_cyrillic_Shch(void)
{
    // Щ (U+0429) xAdvance = 7 (extra-wide)
    TEST_ASSERT_EQUAL_FLOAT(7.0f, getTextWidth("\xD0\xA9", 0, false));
}

void test_width_cyrillic_word(void)
{
    // Привіт: П=4, р=4, и=5, в=4, і=2, т=4 = 23
    TEST_ASSERT_EQUAL_FLOAT(23.0f, getTextWidth("\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD1\x96\xD1\x82", 0, false));
}

// --- UTF-8 Latin Extended ---

void test_width_degree_C(void)
{
    // °C: °=3, C=4 = 7
    TEST_ASSERT_EQUAL_FLOAT(7.0f, getTextWidth("\xC2\xB0\x43", 0, false));
}

// --- Unknown glyph fallback ---

void test_width_unknown_char_default(void)
{
    // Unknown glyph → default width 4
    char text[] = {(char)0x01, '\0'};
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

    RUN_TEST(test_width_cyrillic_A);
    RUN_TEST(test_width_cyrillic_Sh);
    RUN_TEST(test_width_cyrillic_Shch);
    RUN_TEST(test_width_cyrillic_word);

    RUN_TEST(test_width_degree_C);

    RUN_TEST(test_width_unknown_char_default);

    return UNITY_END();
}
