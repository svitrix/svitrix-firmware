#include <unity.h>
#include "UnicodeFont.h"
#include "SvitrixFont.h"
#include <cstring>

// ── Simple pixel buffer for capturing renderGlyph() output ─────────
struct PixelBuf
{
    static constexpr int kMax = 256;
    int16_t xs[kMax], ys[kMax];
    int count = 0;

    void reset()
    {
        count = 0;
    }

    bool has(int16_t x, int16_t y) const
    {
        for (int i = 0; i < count; i++)
            if (xs[i] == x && ys[i] == y)
                return true;
        return false;
    }
};

static void collectPixel(int16_t x, int16_t y, void *ud)
{
    PixelBuf *buf = static_cast<PixelBuf *>(ud);
    if (buf->count < PixelBuf::kMax)
    {
        buf->xs[buf->count] = x;
        buf->ys[buf->count] = y;
        buf->count++;
    }
}

static PixelBuf pixels;

void setUp(void)
{
    pixels.reset();
}
void tearDown(void) {}

// ═══════════════════════════════════════════════════════════════════
// renderGlyph — basic functionality
// ═══════════════════════════════════════════════════════════════════

void test_render_returns_advance(void)
{
    uint8_t adv = renderGlyph(SvitrixFont, 'A', 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(4, adv);
}

void test_render_narrow_char_advance(void)
{
    uint8_t adv = renderGlyph(SvitrixFont, 'I', 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(2, adv);
}

void test_render_wide_char_advance(void)
{
    uint8_t adv = renderGlyph(SvitrixFont, 'M', 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(6, adv);
}

void test_render_unknown_returns_zero(void)
{
    uint8_t adv = renderGlyph(SvitrixFont, 0x4E2D, 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(0, adv);
    TEST_ASSERT_EQUAL(0, pixels.count);
}

void test_render_space_produces_no_pixels(void)
{
    // Space glyph: 1px height, 8px width bitmap but only 0x00 byte = no set bits
    renderGlyph(SvitrixFont, ' ', 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(0, pixels.count);
}

// ═══════════════════════════════════════════════════════════════════
// renderGlyph — pixel output verification
// ═══════════════════════════════════════════════════════════════════

void test_render_period_single_pixel(void)
{
    // Period '.' is 1px wide, 1px tall at bottom — simplest glyph
    renderGlyph(SvitrixFont, '.', 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(1, pixels.count);
    // yOffset=-1 means 1px up from baseline, so y = 5 + (-1) = 4
    TEST_ASSERT_EQUAL(0, pixels.xs[0]);
    TEST_ASSERT_EQUAL(4, pixels.ys[0]);
}

void test_render_exclamation(void)
{
    // '!' is narrow (xAdvance=2), 5px tall — vertical line + dot
    renderGlyph(SvitrixFont, '!', 0, 5, collectPixel, &pixels);
    TEST_ASSERT_TRUE(pixels.count > 0);
    TEST_ASSERT_TRUE(pixels.count <= 5); // at most 5 pixels in a 1-wide, 5-tall glyph
}

void test_render_cursor_offset(void)
{
    // Render '.' at cursor (10, 7) — pixel should be at (10, 6)
    renderGlyph(SvitrixFont, '.', 10, 7, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(1, pixels.count);
    TEST_ASSERT_EQUAL(10, pixels.xs[0]);
    TEST_ASSERT_EQUAL(6, pixels.ys[0]);
}

void test_render_A_has_pixels(void)
{
    renderGlyph(SvitrixFont, 'A', 0, 5, collectPixel, &pixels);
    // 'A' in 3x5 font should produce several pixels
    TEST_ASSERT_TRUE(pixels.count >= 5);
    TEST_ASSERT_TRUE(pixels.count <= 15); // 3*5 max
}

void test_render_A_pixels_within_bounds(void)
{
    renderGlyph(SvitrixFont, 'A', 0, 5, collectPixel, &pixels);
    const UniGlyph *g = findGlyph(SvitrixFont, 'A');
    int8_t xo = SvitrixFont.xOffset;
    uint8_t w = SvitrixFont.bitmapWidth;
    uint8_t h = glyphHeight(g);
    int8_t yo = glyphYOffset(g);
    for (int i = 0; i < pixels.count; i++)
    {
        // x should be within [xOffset, xOffset + width - 1]
        TEST_ASSERT_TRUE(pixels.xs[i] >= xo);
        TEST_ASSERT_TRUE(pixels.xs[i] < xo + w);
        // y should be within [5 + yOffset, 5 + yOffset + height - 1]
        TEST_ASSERT_TRUE(pixels.ys[i] >= 5 + yo);
        TEST_ASSERT_TRUE(pixels.ys[i] < 5 + yo + h);
    }
}

// ═══════════════════════════════════════════════════════════════════
// renderGlyph — Cyrillic glyphs render correctly
// ═══════════════════════════════════════════════════════════════════

void test_render_cyrillic_A(void)
{
    uint8_t adv = renderGlyph(SvitrixFont, 0x0410, 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(4, adv);
    TEST_ASSERT_TRUE(pixels.count >= 5);
}

void test_render_cyrillic_Sh_wide(void)
{
    uint8_t adv = renderGlyph(SvitrixFont, 0x0428, 0, 5, collectPixel, &pixels);
    TEST_ASSERT_EQUAL(6, adv); // Ш is wide
    TEST_ASSERT_TRUE(pixels.count >= 5);
}

void test_render_cyrillic_lower_aliases_to_upper(void)
{
    // Render uppercase А
    PixelBuf upper;
    renderGlyph(SvitrixFont, 0x0410, 0, 5, collectPixel, &upper);

    // Render lowercase а
    PixelBuf lower;
    lower.count = 0;
    renderGlyph(SvitrixFont, 0x0430, 0, 5, [](int16_t x, int16_t y, void *ud)
                {
                    PixelBuf *b = static_cast<PixelBuf *>(ud);
                    if (b->count < PixelBuf::kMax)
                    {
                        b->xs[b->count] = x;
                        b->ys[b->count] = y;
                        b->count++;
                    } }, &lower);

    // Same bitmap → same pixel count and positions
    TEST_ASSERT_EQUAL(upper.count, lower.count);
    for (int i = 0; i < upper.count; i++)
    {
        TEST_ASSERT_EQUAL(upper.xs[i], lower.xs[i]);
        TEST_ASSERT_EQUAL(upper.ys[i], lower.ys[i]);
    }
}

// ═══════════════════════════════════════════════════════════════════
// renderGlyph — Latin Extended aliases render same as base
// ═══════════════════════════════════════════════════════════════════

void test_render_c_caron_same_as_C(void)
{
    PixelBuf base;
    renderGlyph(SvitrixFont, 'C', 0, 5, collectPixel, &base);

    PixelBuf alias;
    alias.count = 0;
    renderGlyph(SvitrixFont, 0x010C, 0, 5, // Č
                [](int16_t x, int16_t y, void *ud)
                {
                    PixelBuf *b = static_cast<PixelBuf *>(ud);
                    if (b->count < PixelBuf::kMax)
                    {
                        b->xs[b->count] = x;
                        b->ys[b->count] = y;
                        b->count++;
                    } }, &alias);

    TEST_ASSERT_EQUAL(base.count, alias.count);
}

// ═══════════════════════════════════════════════════════════════════
// renderGlyph — string rendering (multiple glyphs)
// ═══════════════════════════════════════════════════════════════════

void test_render_string_sequential(void)
{
    // Render "Hi" and verify second glyph is offset by first glyph's advance
    PixelBuf pxH, pxI;
    uint8_t advH = renderGlyph(SvitrixFont, 'H', 0, 5, collectPixel, &pxH);
    pxI.count = 0;
    renderGlyph(SvitrixFont, 'i', advH, 5, [](int16_t x, int16_t y, void *ud)
                {
                    PixelBuf *b = static_cast<PixelBuf *>(ud);
                    if (b->count < PixelBuf::kMax)
                    {
                        b->xs[b->count] = x;
                        b->ys[b->count] = y;
                        b->count++;
                    } }, &pxI);

    // 'H' advance is 4, so 'i' pixels should start at x >= 4
    TEST_ASSERT_EQUAL(4, advH);
    for (int i = 0; i < pxI.count; i++)
    {
        TEST_ASSERT_TRUE(pxI.xs[i] >= 4);
    }
}

void test_render_degree_C_utf8(void)
{
    // Render "°C" as UTF-8 codepoints
    const char *text = "\xC2\xB0\x43"; // °C
    const char *p = text;
    int16_t cx = 0;

    uint16_t cp1 = utf8NextCodepoint(p);
    TEST_ASSERT_EQUAL(0x00B0, cp1); // °
    uint8_t adv1 = renderGlyph(SvitrixFont, cp1, cx, 5, collectPixel, &pixels);
    TEST_ASSERT_TRUE(adv1 > 0);
    cx += adv1;

    uint16_t cp2 = utf8NextCodepoint(p);
    TEST_ASSERT_EQUAL(0x0043, cp2); // C
    PixelBuf px2;
    px2.count = 0;
    uint8_t adv2 = renderGlyph(SvitrixFont, cp2, cx, 5, [](int16_t x, int16_t y, void *ud)
                               {
                                    PixelBuf *b = static_cast<PixelBuf *>(ud);
                                    if (b->count < PixelBuf::kMax)
                                    {
                                        b->xs[b->count] = x;
                                        b->ys[b->count] = y;
                                        b->count++;
                                    } }, &px2);

    TEST_ASSERT_EQUAL(4, adv2);
    // 'C' pixels should be offset by degree advance
    for (int i = 0; i < px2.count; i++)
    {
        TEST_ASSERT_TRUE(px2.xs[i] >= cx);
    }
}

// ═══════════════════════════════════════════════════════════════════
// renderGlyph — all ASCII printable glyphs render without crash
// ═══════════════════════════════════════════════════════════════════

void test_render_all_ascii_no_crash(void)
{
    for (uint16_t cp = 0x20; cp <= 0x7E; cp++)
    {
        pixels.reset();
        uint8_t adv = renderGlyph(SvitrixFont, cp, 0, 5, collectPixel, &pixels);
        TEST_ASSERT_TRUE(adv > 0);
    }
}

void test_render_all_cyrillic_upper_no_crash(void)
{
    for (uint16_t cp = 0x0410; cp <= 0x042F; cp++)
    {
        pixels.reset();
        uint8_t adv = renderGlyph(SvitrixFont, cp, 0, 5, collectPixel, &pixels);
        TEST_ASSERT_TRUE(adv > 0);
    }
}

void test_render_all_cyrillic_lower_no_crash(void)
{
    for (uint16_t cp = 0x0430; cp <= 0x044F; cp++)
    {
        pixels.reset();
        uint8_t adv = renderGlyph(SvitrixFont, cp, 0, 5, collectPixel, &pixels);
        TEST_ASSERT_TRUE(adv > 0);
    }
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Basic functionality
    RUN_TEST(test_render_returns_advance);
    RUN_TEST(test_render_narrow_char_advance);
    RUN_TEST(test_render_wide_char_advance);
    RUN_TEST(test_render_unknown_returns_zero);
    RUN_TEST(test_render_space_produces_no_pixels);

    // Pixel verification
    RUN_TEST(test_render_period_single_pixel);
    RUN_TEST(test_render_exclamation);
    RUN_TEST(test_render_cursor_offset);
    RUN_TEST(test_render_A_has_pixels);
    RUN_TEST(test_render_A_pixels_within_bounds);

    // Cyrillic
    RUN_TEST(test_render_cyrillic_A);
    RUN_TEST(test_render_cyrillic_Sh_wide);
    RUN_TEST(test_render_cyrillic_lower_aliases_to_upper);

    // Latin Extended aliases
    RUN_TEST(test_render_c_caron_same_as_C);

    // String rendering
    RUN_TEST(test_render_string_sequential);
    RUN_TEST(test_render_degree_C_utf8);

    // Bulk: all glyphs render without crash
    RUN_TEST(test_render_all_ascii_no_crash);
    RUN_TEST(test_render_all_cyrillic_upper_no_crash);
    RUN_TEST(test_render_all_cyrillic_lower_no_crash);

    return UNITY_END();
}
