#include <unity.h>
#include "TextUtils.h"

// Reset the static state between tests
void setUp(void)
{
    // Feed a dummy ASCII char to reset the c1 static variable
    utf8ascii((byte)0);
}

void tearDown(void) {}

// --- ASCII passthrough ---

void test_ascii_letter_a(void)
{
    TEST_ASSERT_EQUAL(0x41, utf8ascii((byte)0x41)); // 'A'
}

void test_ascii_letter_z(void)
{
    TEST_ASSERT_EQUAL(0x7A, utf8ascii((byte)0x7A)); // 'z'
}

void test_ascii_space(void)
{
    TEST_ASSERT_EQUAL(0x20, utf8ascii((byte)0x20)); // ' '
}

void test_ascii_digit(void)
{
    TEST_ASSERT_EQUAL(0x35, utf8ascii((byte)0x35)); // '5'
}

// --- Latin-1 Supplement (0xC3 prefix) ---

void test_c3_a_umlaut(void)
{
    // ä = 0xC3 0xA4 → should return 0xA4 | 0xC0 = 0xE4
    utf8ascii((byte)0xC3); // first byte sets c1
    byte result = utf8ascii((byte)0xA4);
    TEST_ASSERT_EQUAL_HEX8(0xE4, result);
}

void test_c3_o_umlaut(void)
{
    // ö = 0xC3 0xB6 → 0xB6 | 0xC0 = 0xF6
    utf8ascii((byte)0xC3);
    byte result = utf8ascii((byte)0xB6);
    TEST_ASSERT_EQUAL_HEX8(0xF6, result);
}

void test_c3_u_umlaut(void)
{
    // ü = 0xC3 0xBC → 0xBC | 0xC0 = 0xFC
    utf8ascii((byte)0xC3);
    byte result = utf8ascii((byte)0xBC);
    TEST_ASSERT_EQUAL_HEX8(0xFC, result);
}

void test_c3_sharp_s(void)
{
    // ß = 0xC3 0x9F → 0x9F | 0xC0 = 0xDF
    utf8ascii((byte)0xC3);
    byte result = utf8ascii((byte)0x9F);
    TEST_ASSERT_EQUAL_HEX8(0xDF, result);
}

// --- Polish (0xC3 prefix special cases) ---

void test_c3_polish_o_stroke_lower(void)
{
    // ó = 0xC3 0xB3 → 0x6F ('o')
    utf8ascii((byte)0xC3);
    byte result = utf8ascii((byte)0xB3);
    TEST_ASSERT_EQUAL_HEX8(0x6F, result);
}

void test_c3_polish_o_stroke_upper(void)
{
    // Ó = 0xC3 0x93 → 0x4F ('O')
    utf8ascii((byte)0xC3);
    byte result = utf8ascii((byte)0x93);
    TEST_ASSERT_EQUAL_HEX8(0x4F, result);
}

// --- Cyrillic (0xD0 prefix) ---

void test_cyrillic_A(void)
{
    // А = 0xD0 0x90 → 0x90 - 17 = 0x7F
    utf8ascii((byte)0xD0);
    byte result = utf8ascii((byte)0x90);
    TEST_ASSERT_EQUAL_HEX8(0x90 - 17, result);
}

void test_cyrillic_Ya(void)
{
    // Я = 0xD0 0xAF → 0xAF - 17 = 0x9E
    utf8ascii((byte)0xD0);
    byte result = utf8ascii((byte)0xAF);
    TEST_ASSERT_EQUAL_HEX8(0xAF - 17, result);
}

void test_cyrillic_a_lower(void)
{
    // а = 0xD0 0xB0 → 0xB0 - 49 = 0x7F
    utf8ascii((byte)0xD0);
    byte result = utf8ascii((byte)0xB0);
    TEST_ASSERT_EQUAL_HEX8(0xB0 - 49, result);
}

// --- Cyrillic (0xD1 prefix) ---

void test_cyrillic_d1_range(void)
{
    // Letters in 0xD1 0x80-0x8F range → ascii + 15
    utf8ascii((byte)0xD1);
    byte result = utf8ascii((byte)0x80);
    TEST_ASSERT_EQUAL_HEX8(0x80 + 15, result);
}

// --- Ukrainian special letters ---

void test_ukrainian_yo(void)
{
    // Ё = 0xD0 0x81 → 0x84
    utf8ascii((byte)0xD0);
    byte result = utf8ascii((byte)0x81);
    TEST_ASSERT_EQUAL_HEX8(0x84, result);
}

void test_ukrainian_ye(void)
{
    // Є = 0xD0 0x84 → 0xA0
    utf8ascii((byte)0xD0);
    byte result = utf8ascii((byte)0x84);
    TEST_ASSERT_EQUAL_HEX8(0xA0, result);
}

void test_ukrainian_i(void)
{
    // І = 0xD0 0x86 → 0xA1
    utf8ascii((byte)0xD0);
    byte result = utf8ascii((byte)0x86);
    TEST_ASSERT_EQUAL_HEX8(0xA1, result);
}

void test_ukrainian_yi(void)
{
    // Ї = 0xD0 0x87 → 0xEF
    utf8ascii((byte)0xD0);
    byte result = utf8ascii((byte)0x87);
    TEST_ASSERT_EQUAL_HEX8(0xEF, result);
}

// --- Unknown high bytes return 0 ---

void test_unknown_high_byte_returns_zero(void)
{
    // Unknown prefix should return 0
    utf8ascii((byte)0xE0);
    byte result = utf8ascii((byte)0x80);
    TEST_ASSERT_EQUAL_HEX8(0, result);
}

// --- String version ---

void test_string_ascii_passthrough(void)
{
    String result = utf8ascii(String("Hello"));
    TEST_ASSERT_TRUE(result == "Hello");
}

void test_string_empty(void)
{
    String result = utf8ascii(String(""));
    TEST_ASSERT_EQUAL(0, result.length());
}

// --- adversarial: interrupted sequences, state corruption ---

void test_interrupted_sequence_prefix_then_ascii(void)
{
    // Send a Cyrillic prefix 0xD0, then an ASCII char instead of continuation.
    // The ASCII char should reset state and pass through normally.
    utf8ascii((byte)0xD0); // prefix — sets c1=0xD0, returns 0
    byte result = utf8ascii((byte)0x41); // 'A' — ASCII resets c1, returns 0x41
    TEST_ASSERT_EQUAL_HEX8(0x41, result);
}

void test_double_prefix_no_corruption(void)
{
    // Two consecutive prefix bytes: 0xD0 then 0xD0 again.
    // First 0xD0 sets c1. Second 0xD0: last=0xD0, switch matches D0 case,
    // but 0xD0 is not in any valid continuation range → returns 0 (dropped).
    utf8ascii((byte)0xD0); // sets c1=0xD0
    byte result1 = utf8ascii((byte)0xD0); // last=0xD0, ascii=0xD0 → no match → 0
    TEST_ASSERT_EQUAL_HEX8(0, result1);
    // Now send a valid continuation for the SECOND 0xD0 prefix.
    byte result2 = utf8ascii((byte)0x90); // А: 0x90-17 = 0x7F
    TEST_ASSERT_EQUAL_HEX8(0x7F, result2);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // ASCII passthrough
    RUN_TEST(test_ascii_letter_a);
    RUN_TEST(test_ascii_letter_z);
    RUN_TEST(test_ascii_space);
    RUN_TEST(test_ascii_digit);

    // German umlauts (0xC3)
    RUN_TEST(test_c3_a_umlaut);
    RUN_TEST(test_c3_o_umlaut);
    RUN_TEST(test_c3_u_umlaut);
    RUN_TEST(test_c3_sharp_s);

    // Polish (0xC3 special cases)
    RUN_TEST(test_c3_polish_o_stroke_lower);
    RUN_TEST(test_c3_polish_o_stroke_upper);

    // Cyrillic (0xD0)
    RUN_TEST(test_cyrillic_A);
    RUN_TEST(test_cyrillic_Ya);
    RUN_TEST(test_cyrillic_a_lower);

    // Cyrillic (0xD1)
    RUN_TEST(test_cyrillic_d1_range);

    // Ukrainian
    RUN_TEST(test_ukrainian_yo);
    RUN_TEST(test_ukrainian_ye);
    RUN_TEST(test_ukrainian_i);
    RUN_TEST(test_ukrainian_yi);

    // Edge cases
    RUN_TEST(test_unknown_high_byte_returns_zero);

    // String
    RUN_TEST(test_string_ascii_passthrough);
    RUN_TEST(test_string_empty);

    // adversarial
    RUN_TEST(test_interrupted_sequence_prefix_then_ascii);
    RUN_TEST(test_double_prefix_no_corruption);

    return UNITY_END();
}
