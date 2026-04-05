#include <unity.h>
#include "UnicodeFont.h"
#include "SvitrixFont.h"

void setUp(void) {}
void tearDown(void) {}

// ═══════════════════════════════════════════════════════════════
// findGlyph — ASCII
// ═══════════════════════════════════════════════════════════════

void test_find_space(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0020);
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(0x0020, g->codepoint);
    TEST_ASSERT_EQUAL(2, g->xAdvance);
}

void test_find_A(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 'A');
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(0x0041, g->codepoint);
    TEST_ASSERT_EQUAL(4, g->xAdvance);
}

void test_find_I_narrow(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 'I');
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(2, g->xAdvance);
}

void test_find_M_wide(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 'M');
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(6, g->xAdvance);
}

void test_find_tilde(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, '~');
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(0x007E, g->codepoint);
}

// ═══════════════════════════════════════════════════════════════
// findGlyph — Cyrillic uppercase
// ═══════════════════════════════════════════════════════════════

void test_find_cyrillic_A(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0410); // А
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(0x0410, g->codepoint);
    TEST_ASSERT_EQUAL(4, g->xAdvance);
}

void test_find_cyrillic_Sh(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0428); // Ш
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(6, g->xAdvance);
}

void test_find_cyrillic_Shch(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0429); // Щ
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(7, g->xAdvance);
}

void test_find_cyrillic_Ya(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x042F); // Я
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(0x042F, g->codepoint);
}

// ═══════════════════════════════════════════════════════════════
// findGlyph — Cyrillic lowercase (aliases)
// ═══════════════════════════════════════════════════════════════

void test_find_cyrillic_a_lower(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0430); // а
    TEST_ASSERT_NOT_NULL(g);
    // Should share bitmap with uppercase А
    const UniGlyph *upper = findGlyph(SvitrixFont, 0x0410);
    TEST_ASSERT_NOT_NULL(upper);
    TEST_ASSERT_EQUAL(upper->bitmapOffset, g->bitmapOffset);
}

void test_find_cyrillic_ya_lower(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x044F); // я
    TEST_ASSERT_NOT_NULL(g);
    const UniGlyph *upper = findGlyph(SvitrixFont, 0x042F);
    TEST_ASSERT_EQUAL(upper->bitmapOffset, g->bitmapOffset);
}

// ═══════════════════════════════════════════════════════════════
// findGlyph — Ukrainian special characters
// ═══════════════════════════════════════════════════════════════

void test_find_ukrainian_Ghe(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0490); // Ґ
    TEST_ASSERT_NOT_NULL(g);
}

void test_find_ukrainian_ghe_lower(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0491); // ґ
    TEST_ASSERT_NOT_NULL(g);
    const UniGlyph *upper = findGlyph(SvitrixFont, 0x0490);
    TEST_ASSERT_EQUAL(upper->bitmapOffset, g->bitmapOffset);
}

void test_find_ukrainian_Ye(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0404); // Є
    TEST_ASSERT_NOT_NULL(g);
}

void test_find_ukrainian_ye_lower(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0454); // є
    TEST_ASSERT_NOT_NULL(g);
}

void test_find_ukrainian_I(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0406); // І
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(2, g->xAdvance); // Same as Latin I
}

void test_find_ukrainian_i_lower(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0456); // і
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(2, g->xAdvance);
}

void test_find_ukrainian_Yi(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0407); // Ї
    TEST_ASSERT_NOT_NULL(g);
}

void test_find_ukrainian_yi_lower(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x0457); // ї
    TEST_ASSERT_NOT_NULL(g);
}

// ═══════════════════════════════════════════════════════════════
// findGlyph — Latin-1 Supplement
// ═══════════════════════════════════════════════════════════════

void test_find_degree(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x00B0); // °
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL(3, g->xAdvance);
}

void test_find_a_dieresis(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x00E4); // ä
    TEST_ASSERT_NOT_NULL(g);
}

// ═══════════════════════════════════════════════════════════════
// findGlyph — not found
// ═══════════════════════════════════════════════════════════════

void test_find_nonexistent(void)
{
    const UniGlyph *g = findGlyph(SvitrixFont, 0x4E2D); // 中 (Chinese)
    TEST_ASSERT_NULL(g);
}

// ═══════════════════════════════════════════════════════════════
// getGlyphAdvance
// ═══════════════════════════════════════════════════════════════

void test_advance_known(void)
{
    TEST_ASSERT_EQUAL(4, getGlyphAdvance(SvitrixFont, 'A', 4));
}

void test_advance_unknown_returns_default(void)
{
    TEST_ASSERT_EQUAL(4, getGlyphAdvance(SvitrixFont, 0x4E2D, 4));
}

// ═══════════════════════════════════════════════════════════════
// getUnicodeTextWidth
// ═══════════════════════════════════════════════════════════════

void test_unicode_width_ascii(void)
{
    // "Hi!" = H(4) + i(2) + !(2) = 8
    TEST_ASSERT_EQUAL_FLOAT(8.0f, getUnicodeTextWidth(SvitrixFont, "Hi!", 0, false));
}

void test_unicode_width_cyrillic(void)
{
    // "АБ" = А(4) + Б(4) = 8
    TEST_ASSERT_EQUAL_FLOAT(8.0f, getUnicodeTextWidth(SvitrixFont, "\xD0\x90\xD0\x91", 0, false));
}

void test_unicode_width_empty(void)
{
    TEST_ASSERT_EQUAL_FLOAT(0.0f, getUnicodeTextWidth(SvitrixFont, "", 0, false));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // ASCII
    RUN_TEST(test_find_space);
    RUN_TEST(test_find_A);
    RUN_TEST(test_find_I_narrow);
    RUN_TEST(test_find_M_wide);
    RUN_TEST(test_find_tilde);

    // Cyrillic uppercase
    RUN_TEST(test_find_cyrillic_A);
    RUN_TEST(test_find_cyrillic_Sh);
    RUN_TEST(test_find_cyrillic_Shch);
    RUN_TEST(test_find_cyrillic_Ya);

    // Cyrillic lowercase
    RUN_TEST(test_find_cyrillic_a_lower);
    RUN_TEST(test_find_cyrillic_ya_lower);

    // Ukrainian
    RUN_TEST(test_find_ukrainian_Ghe);
    RUN_TEST(test_find_ukrainian_ghe_lower);
    RUN_TEST(test_find_ukrainian_Ye);
    RUN_TEST(test_find_ukrainian_ye_lower);
    RUN_TEST(test_find_ukrainian_I);
    RUN_TEST(test_find_ukrainian_i_lower);
    RUN_TEST(test_find_ukrainian_Yi);
    RUN_TEST(test_find_ukrainian_yi_lower);

    // Latin-1
    RUN_TEST(test_find_degree);
    RUN_TEST(test_find_a_dieresis);

    // Not found
    RUN_TEST(test_find_nonexistent);

    // getGlyphAdvance
    RUN_TEST(test_advance_known);
    RUN_TEST(test_advance_unknown_returns_default);

    // getUnicodeTextWidth
    RUN_TEST(test_unicode_width_ascii);
    RUN_TEST(test_unicode_width_cyrillic);
    RUN_TEST(test_unicode_width_empty);

    return UNITY_END();
}
