#include <unity.h>
#include "UnicodeFont.h"

void setUp(void) {}
void tearDown(void) {}

// ═══════════════════════════════════════════════════════════════
// utf8NextCodepoint — single-byte ASCII
// ═══════════════════════════════════════════════════════════════

void test_ascii_A(void)
{
    const char *p = "A";
    TEST_ASSERT_EQUAL(0x41, utf8NextCodepoint(p));
    TEST_ASSERT_EQUAL('\0', *p);
}

void test_ascii_z(void)
{
    const char *p = "z";
    TEST_ASSERT_EQUAL(0x7A, utf8NextCodepoint(p));
}

void test_ascii_space(void)
{
    const char *p = " ";
    TEST_ASSERT_EQUAL(0x20, utf8NextCodepoint(p));
}

void test_ascii_digit(void)
{
    const char *p = "5";
    TEST_ASSERT_EQUAL(0x35, utf8NextCodepoint(p));
}

// ═══════════════════════════════════════════════════════════════
// utf8NextCodepoint — 2-byte Latin Extended
// ═══════════════════════════════════════════════════════════════

void test_latin_a_dieresis(void) // ä = C3 A4
{
    const char *p = "\xC3\xA4";
    TEST_ASSERT_EQUAL(0x00E4, utf8NextCodepoint(p));
    TEST_ASSERT_EQUAL('\0', *p);
}

void test_latin_o_dieresis(void) // ö = C3 B6
{
    const char *p = "\xC3\xB6";
    TEST_ASSERT_EQUAL(0x00F6, utf8NextCodepoint(p));
}

void test_latin_u_dieresis(void) // ü = C3 BC
{
    const char *p = "\xC3\xBC";
    TEST_ASSERT_EQUAL(0x00FC, utf8NextCodepoint(p));
}

void test_latin_sharp_s(void) // ß = C3 9F
{
    const char *p = "\xC3\x9F";
    TEST_ASSERT_EQUAL(0x00DF, utf8NextCodepoint(p));
}

void test_latin_o_acute(void) // ó = C3 B3
{
    const char *p = "\xC3\xB3";
    TEST_ASSERT_EQUAL(0x00F3, utf8NextCodepoint(p));
}

void test_degree(void) // ° = C2 B0
{
    const char *p = "\xC2\xB0";
    TEST_ASSERT_EQUAL(0x00B0, utf8NextCodepoint(p));
}

// ═══════════════════════════════════════════════════════════════
// utf8NextCodepoint — 2-byte Cyrillic
// ═══════════════════════════════════════════════════════════════

void test_cyrillic_A(void) // А = D0 90
{
    const char *p = "\xD0\x90";
    TEST_ASSERT_EQUAL(0x0410, utf8NextCodepoint(p));
}

void test_cyrillic_Ya(void) // Я = D0 AF
{
    const char *p = "\xD0\xAF";
    TEST_ASSERT_EQUAL(0x042F, utf8NextCodepoint(p));
}

void test_cyrillic_a_lower(void) // а = D0 B0
{
    const char *p = "\xD0\xB0";
    TEST_ASSERT_EQUAL(0x0430, utf8NextCodepoint(p));
}

void test_cyrillic_r_lower(void) // р = D1 80
{
    const char *p = "\xD1\x80";
    TEST_ASSERT_EQUAL(0x0440, utf8NextCodepoint(p));
}

void test_cyrillic_Yo(void) // Ё = D0 81
{
    const char *p = "\xD0\x81";
    TEST_ASSERT_EQUAL(0x0401, utf8NextCodepoint(p));
}

void test_cyrillic_Ye(void) // Є = D0 84
{
    const char *p = "\xD0\x84";
    TEST_ASSERT_EQUAL(0x0404, utf8NextCodepoint(p));
}

void test_cyrillic_I(void) // І = D0 86
{
    const char *p = "\xD0\x86";
    TEST_ASSERT_EQUAL(0x0406, utf8NextCodepoint(p));
}

void test_cyrillic_Yi(void) // Ї = D0 87
{
    const char *p = "\xD0\x87";
    TEST_ASSERT_EQUAL(0x0407, utf8NextCodepoint(p));
}

void test_cyrillic_Ghe_upturn(void) // Ґ = D2 90
{
    const char *p = "\xD2\x90";
    TEST_ASSERT_EQUAL(0x0490, utf8NextCodepoint(p));
}

// ═══════════════════════════════════════════════════════════════
// utf8NextCodepoint — 3-byte (Euro sign)
// ═══════════════════════════════════════════════════════════════

void test_euro_sign(void) // € = E2 82 AC
{
    const char *p = "\xE2\x82\xAC";
    TEST_ASSERT_EQUAL(0x20AC, utf8NextCodepoint(p));
}

// ═══════════════════════════════════════════════════════════════
// utf8NextCodepoint — edge cases
// ═══════════════════════════════════════════════════════════════

void test_end_of_string(void)
{
    const char *p = "";
    TEST_ASSERT_EQUAL(0, utf8NextCodepoint(p));
}

void test_null_pointer(void)
{
    const char *p = nullptr;
    TEST_ASSERT_EQUAL(0, utf8NextCodepoint(p));
}

void test_invalid_byte(void)
{
    const char *p = "\xFF";
    uint16_t cp = utf8NextCodepoint(p);
    TEST_ASSERT_EQUAL(0xFFFD, cp);
}

void test_truncated_sequence(void)
{
    const char data[] = {(char)0xC3, '\0'};
    const char *p = data;
    uint16_t cp = utf8NextCodepoint(p);
    TEST_ASSERT_EQUAL(0xFFFD, cp);
}

// ═══════════════════════════════════════════════════════════════
// utf8NextCodepoint — multi-character iteration
// ═══════════════════════════════════════════════════════════════

void test_iterate_ascii_string(void)
{
    const char *p = "Hi!";
    TEST_ASSERT_EQUAL('H', utf8NextCodepoint(p));
    TEST_ASSERT_EQUAL('i', utf8NextCodepoint(p));
    TEST_ASSERT_EQUAL('!', utf8NextCodepoint(p));
    TEST_ASSERT_EQUAL(0, utf8NextCodepoint(p));
}

void test_iterate_mixed_string(void)
{
    const char *p = "A\xD0\x90"; // "AА"
    TEST_ASSERT_EQUAL(0x0041, utf8NextCodepoint(p));
    TEST_ASSERT_EQUAL(0x0410, utf8NextCodepoint(p));
    TEST_ASSERT_EQUAL(0, utf8NextCodepoint(p));
}

// ═══════════════════════════════════════════════════════════════
// utf8Length
// ═══════════════════════════════════════════════════════════════

void test_utf8_length_ascii(void)
{
    TEST_ASSERT_EQUAL(5, utf8Length("Hello"));
}

void test_utf8_length_cyrillic(void)
{
    // Привіт = 6 characters, 12 bytes
    TEST_ASSERT_EQUAL(6, utf8Length("\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD1\x96\xD1\x82"));
}

void test_utf8_length_empty(void)
{
    TEST_ASSERT_EQUAL(0, utf8Length(""));
}

void test_utf8_length_mixed(void)
{
    // A°C = 3 characters
    TEST_ASSERT_EQUAL(3, utf8Length("A\xC2\xB0\x43"));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_ascii_A);
    RUN_TEST(test_ascii_z);
    RUN_TEST(test_ascii_space);
    RUN_TEST(test_ascii_digit);

    RUN_TEST(test_latin_a_dieresis);
    RUN_TEST(test_latin_o_dieresis);
    RUN_TEST(test_latin_u_dieresis);
    RUN_TEST(test_latin_sharp_s);
    RUN_TEST(test_latin_o_acute);
    RUN_TEST(test_degree);

    RUN_TEST(test_cyrillic_A);
    RUN_TEST(test_cyrillic_Ya);
    RUN_TEST(test_cyrillic_a_lower);
    RUN_TEST(test_cyrillic_r_lower);
    RUN_TEST(test_cyrillic_Yo);
    RUN_TEST(test_cyrillic_Ye);
    RUN_TEST(test_cyrillic_I);
    RUN_TEST(test_cyrillic_Yi);
    RUN_TEST(test_cyrillic_Ghe_upturn);

    RUN_TEST(test_euro_sign);

    RUN_TEST(test_end_of_string);
    RUN_TEST(test_null_pointer);
    RUN_TEST(test_invalid_byte);
    RUN_TEST(test_truncated_sequence);

    RUN_TEST(test_iterate_ascii_string);
    RUN_TEST(test_iterate_mixed_string);

    RUN_TEST(test_utf8_length_ascii);
    RUN_TEST(test_utf8_length_cyrillic);
    RUN_TEST(test_utf8_length_empty);
    RUN_TEST(test_utf8_length_mixed);

    return UNITY_END();
}
