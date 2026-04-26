/**
 * @file test_format_validator.cpp
 * @brief Tests for FormatStringValidator — printf-format whitelist.
 *
 * The validator is the security gate that prevents user-controlled
 * strings (e.g. DataFetcher displayFormat from POST /api/datafetcher)
 * from being passed to snprintf as a format string.
 *
 * Rules enforced:
 *   - At most ONE non-%% conversion specifier
 *   - Conversion char must be in whitelist: d i u o x X f g e E s
 *   - Optional flags  (+-# 0 space), width (1-2 digits), precision (.0-99)
 *   - %% literal allowed
 *   - %n, %p, %c, %a, %A rejected
 *   - %*  / %.* (variable width/precision) rejected — extra varargs
 *   - %1$ (positional args) rejected
 */
#include <unity.h>
#include "FormatStringValidator.h"

void setUp(void) {}
void tearDown(void) {}

// ── Accept: trivial cases ───────────────────────────────────────────

void test_empty_is_safe(void)
{
    TEST_ASSERT_TRUE(isSafeSingleArgFormat(""));
}

void test_plain_text_no_percent(void)
{
    TEST_ASSERT_TRUE(isSafeSingleArgFormat("hello world"));
}

void test_literal_double_percent(void)
{
    TEST_ASSERT_TRUE(isSafeSingleArgFormat("100%%"));
}

void test_literal_percent_then_format(void)
{
    TEST_ASSERT_TRUE(isSafeSingleArgFormat("%%%d"));
}

// ── Accept: bare conversions ────────────────────────────────────────

void test_bare_d(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%d")); }
void test_bare_i(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%i")); }
void test_bare_u(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%u")); }
void test_bare_o(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%o")); }
void test_bare_x(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%x")); }
void test_bare_X(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%X")); }
void test_bare_f(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%f")); }
void test_bare_g(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%g")); }
void test_bare_e(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%e")); }
void test_bare_E(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%E")); }
void test_bare_s(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%s")); }

// ── Accept: flags, width, precision ─────────────────────────────────

void test_width_only(void)            { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%5d")); }
void test_left_align_flag(void)       { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%-5d")); }
void test_sign_flag(void)             { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%+d")); }
void test_zero_pad(void)              { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%05d")); }
void test_space_flag(void)            { TEST_ASSERT_TRUE(isSafeSingleArgFormat("% d")); }
void test_alt_form_flag(void)         { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%#x")); }
void test_precision_only(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%.2f")); }
void test_width_and_precision(void)   { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%5.2f")); }
void test_flag_width_precision(void)  { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%-5.2f")); }
void test_zero_precision(void)        { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%.0f")); }
void test_two_digit_width(void)       { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%99d")); }

// ── Accept: real-world DataFetcher patterns ─────────────────────────

void test_currency_prefix(void)       { TEST_ASSERT_TRUE(isSafeSingleArgFormat("$%.0f")); }
void test_currency_suffix(void)       { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%.0f USD")); }
void test_text_around_format(void)    { TEST_ASSERT_TRUE(isSafeSingleArgFormat("BTC: $%.2f")); }
void test_temperature(void)           { TEST_ASSERT_TRUE(isSafeSingleArgFormat("%.1f C")); }
void test_string_with_label(void)     { TEST_ASSERT_TRUE(isSafeSingleArgFormat("Name: %s")); }

// ── Reject: dangerous conversion specifiers ─────────────────────────

void test_reject_n_write_specifier(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%n"));
}

void test_reject_p_pointer(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%p"));
}

void test_reject_c_char(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%c"));
}

void test_reject_a_hexfloat(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%a"));
}

void test_reject_A_hexfloat(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%A"));
}

void test_reject_n_in_long_string(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("Hello %n World"));
}

// ── Reject: more than one varargs slot ──────────────────────────────

void test_reject_two_d(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%d%d"));
}

void test_reject_two_s(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%s%s"));
}

void test_reject_d_then_s(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%d %s"));
}

void test_reject_three_args(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%s%s%s%s%s"));
}

// ── Reject: variable width / positional ─────────────────────────────

void test_reject_star_width(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%*d"));
}

void test_reject_star_precision(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%.*d"));
}

void test_reject_positional_arg(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%1$d"));
}

// ── Reject: incomplete / malformed specifiers ───────────────────────

void test_reject_bare_percent_at_end(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%"));
}

void test_reject_percent_then_digit_no_conversion(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%5"));
}

void test_reject_percent_dot_no_conversion(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%.5"));
}

void test_reject_flag_only(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%-"));
}

void test_reject_unknown_letter(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%z"));
}

void test_reject_length_modifier(void)
{
    // Length modifiers (h, l, ll, L, j, z, t) are not in the whitelist —
    // the validator pairs them with a conversion in the next char, but we
    // block them at face value to keep the surface tiny.
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%ld"));
}

// ── Reject: extreme width (DoS via heap allocation in printf path) ──

void test_reject_three_digit_width(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%999d"));
}

void test_reject_three_digit_precision(void)
{
    TEST_ASSERT_FALSE(isSafeSingleArgFormat("%.999f"));
}

// ── Defensive: NULL pointer ─────────────────────────────────────────

void test_null_pointer_is_safe(void)
{
    // Treating a null pointer as empty makes callers' lives easier.
    TEST_ASSERT_TRUE(isSafeSingleArgFormat(nullptr));
}

// ── Test runner ─────────────────────────────────────────────────────

int main(int /*argc*/, char ** /*argv*/)
{
    UNITY_BEGIN();

    // Trivial accepts
    RUN_TEST(test_empty_is_safe);
    RUN_TEST(test_plain_text_no_percent);
    RUN_TEST(test_literal_double_percent);
    RUN_TEST(test_literal_percent_then_format);

    // Bare conversions
    RUN_TEST(test_bare_d);
    RUN_TEST(test_bare_i);
    RUN_TEST(test_bare_u);
    RUN_TEST(test_bare_o);
    RUN_TEST(test_bare_x);
    RUN_TEST(test_bare_X);
    RUN_TEST(test_bare_f);
    RUN_TEST(test_bare_g);
    RUN_TEST(test_bare_e);
    RUN_TEST(test_bare_E);
    RUN_TEST(test_bare_s);

    // Flags / width / precision
    RUN_TEST(test_width_only);
    RUN_TEST(test_left_align_flag);
    RUN_TEST(test_sign_flag);
    RUN_TEST(test_zero_pad);
    RUN_TEST(test_space_flag);
    RUN_TEST(test_alt_form_flag);
    RUN_TEST(test_precision_only);
    RUN_TEST(test_width_and_precision);
    RUN_TEST(test_flag_width_precision);
    RUN_TEST(test_zero_precision);
    RUN_TEST(test_two_digit_width);

    // Real-world patterns
    RUN_TEST(test_currency_prefix);
    RUN_TEST(test_currency_suffix);
    RUN_TEST(test_text_around_format);
    RUN_TEST(test_temperature);
    RUN_TEST(test_string_with_label);

    // Dangerous conversions
    RUN_TEST(test_reject_n_write_specifier);
    RUN_TEST(test_reject_p_pointer);
    RUN_TEST(test_reject_c_char);
    RUN_TEST(test_reject_a_hexfloat);
    RUN_TEST(test_reject_A_hexfloat);
    RUN_TEST(test_reject_n_in_long_string);

    // Multiple varargs
    RUN_TEST(test_reject_two_d);
    RUN_TEST(test_reject_two_s);
    RUN_TEST(test_reject_d_then_s);
    RUN_TEST(test_reject_three_args);

    // Variable / positional
    RUN_TEST(test_reject_star_width);
    RUN_TEST(test_reject_star_precision);
    RUN_TEST(test_reject_positional_arg);

    // Incomplete / unknown
    RUN_TEST(test_reject_bare_percent_at_end);
    RUN_TEST(test_reject_percent_then_digit_no_conversion);
    RUN_TEST(test_reject_percent_dot_no_conversion);
    RUN_TEST(test_reject_flag_only);
    RUN_TEST(test_reject_unknown_letter);
    RUN_TEST(test_reject_length_modifier);

    // DoS prevention
    RUN_TEST(test_reject_three_digit_width);
    RUN_TEST(test_reject_three_digit_precision);

    // Defensive
    RUN_TEST(test_null_pointer_is_safe);

    return UNITY_END();
}
