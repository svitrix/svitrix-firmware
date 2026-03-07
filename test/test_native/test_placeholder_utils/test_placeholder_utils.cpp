/**
 * @file test_placeholder_utils.cpp
 * @brief Tests for PlaceholderUtils — {{key}} substitution logic.
 *
 * These tests verify correct behavior AND catch the bugs that existed
 * before the fix: skipped placeholders when replacement is longer,
 * infinite loops when replacement contains {{, and edge cases.
 */
#include <unity.h>
#include <map>
#include <string>
#include "PlaceholderUtils.h"

// ── Mock value resolver ────────────────────────────────────────────

static std::map<std::string, std::string> mockValues;

static String mockGetValue(const String &key)
{
    auto it = mockValues.find(std::string(key.c_str()));
    if (it != mockValues.end())
        return String(it->second.c_str());
    return String("N/A");
}

void setUp(void)
{
    mockValues.clear();
}

void tearDown(void) {}

// ── Basic substitution ─────────────────────────────────────────────

void test_no_placeholders(void)
{
    String result = replacePlaceholdersWith("Hello World", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("Hello World", result.c_str());
}

void test_single_placeholder(void)
{
    mockValues["temp"] = "22.5";
    String result = replacePlaceholdersWith("Temp: {{temp}}°C", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("Temp: 22.5°C", result.c_str());
}

void test_unknown_key_returns_na(void)
{
    String result = replacePlaceholdersWith("Val: {{unknown}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("Val: N/A", result.c_str());
}

void test_empty_string(void)
{
    String result = replacePlaceholdersWith("", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("", result.c_str());
}

void test_empty_key(void)
{
    mockValues[""] = "empty_key";
    String result = replacePlaceholdersWith("{{}}!", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("empty_key!", result.c_str());
}

// ── Multiple placeholders ──────────────────────────────────────────

void test_two_adjacent_placeholders(void)
{
    mockValues["a"] = "X";
    mockValues["b"] = "Y";
    String result = replacePlaceholdersWith("{{a}}{{b}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("XY", result.c_str());
}

void test_three_placeholders_with_text(void)
{
    mockValues["city"] = "Moscow";
    mockValues["temp"] = "15";
    mockValues["unit"] = "C";
    String result = replacePlaceholdersWith("{{city}}: {{temp}}°{{unit}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("Moscow: 15°C", result.c_str());
}

// ── BUG REGRESSION: replacement longer than placeholder ────────────
// Before the fix, `start = end + 2` after replace() would point past
// the old `}}` position, causing subsequent placeholders to be skipped.

void test_longer_replacement_does_not_skip_next(void)
{
    mockValues["a"] = "LONGVALUE";
    mockValues["b"] = "OK";
    // "{{a}}{{b}}" — if {{a}} → LONGVALUE (9 chars vs 5 for {{a}}),
    // the old bug would skip {{b}} entirely
    String result = replacePlaceholdersWith("{{a}}{{b}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("LONGVALUEOK", result.c_str());
}

void test_shorter_replacement_finds_next(void)
{
    mockValues["long_key"] = "X";
    mockValues["b"] = "Y";
    String result = replacePlaceholdersWith("{{long_key}}{{b}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("XY", result.c_str());
}

void test_many_placeholders_all_resolved(void)
{
    for (int i = 0; i < 10; i++)
    {
        mockValues["k" + std::to_string(i)] = "v" + std::to_string(i);
    }
    String input = "{{k0}}{{k1}}{{k2}}{{k3}}{{k4}}{{k5}}{{k6}}{{k7}}{{k8}}{{k9}}";
    String result = replacePlaceholdersWith(input, mockGetValue);
    TEST_ASSERT_EQUAL_STRING("v0v1v2v3v4v5v6v7v8v9", result.c_str());
}

// ── BUG REGRESSION: replacement contains {{ — must not loop ────────
// If the resolved value itself contains "{{", the scanner must not
// try to re-parse it as a new placeholder (infinite loop risk).

void test_value_containing_double_braces_not_reparsed(void)
{
    mockValues["x"] = "{{not_a_key}}";
    String result = replacePlaceholdersWith("{{x}}!", mockGetValue);
    // The {{not_a_key}} in the value should appear literally, NOT be resolved
    TEST_ASSERT_EQUAL_STRING("{{not_a_key}}!", result.c_str());
}

void test_value_containing_partial_braces(void)
{
    mockValues["x"] = "a{{b";
    String result = replacePlaceholdersWith("{{x}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("a{{b", result.c_str());
}

// ── Edge cases ─────────────────────────────────────────────────────

void test_unclosed_placeholder_ignored(void)
{
    String result = replacePlaceholdersWith("{{unclosed", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("{{unclosed", result.c_str());
}

void test_unmatched_closing_braces_ignored(void)
{
    String result = replacePlaceholdersWith("text}} more", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("text}} more", result.c_str());
}

void test_nested_braces_treated_as_key(void)
{
    // {{{{inner}}}} — first {{ starts at 0, first }} at 8 → key = "{{inner"
    // This is degenerate input but should not crash
    mockValues["{{inner"] = "resolved";
    String result = replacePlaceholdersWith("{{{{inner}}}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("resolved}}", result.c_str());
}

void test_placeholder_at_end_of_string(void)
{
    mockValues["end"] = "!";
    String result = replacePlaceholdersWith("end={{end}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("end=!", result.c_str());
}

void test_placeholder_at_start_of_string(void)
{
    mockValues["start"] = "BEGIN";
    String result = replacePlaceholdersWith("{{start}} rest", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("BEGIN rest", result.c_str());
}

void test_only_braces_no_content(void)
{
    String result = replacePlaceholdersWith("{{}}", mockGetValue);
    // empty key → mockGetValue("") → "N/A" (unless mapped)
    TEST_ASSERT_EQUAL_STRING("N/A", result.c_str());
}

void test_replacement_to_empty_string(void)
{
    mockValues["gone"] = "";
    String result = replacePlaceholdersWith("A{{gone}}B", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("AB", result.c_str());
}

void test_same_placeholder_twice(void)
{
    mockValues["x"] = "42";
    String result = replacePlaceholdersWith("{{x}} and {{x}}", mockGetValue);
    TEST_ASSERT_EQUAL_STRING("42 and 42", result.c_str());
}

// ── Performance-relevant: input with no placeholders ───────────────

void test_long_string_no_placeholders(void)
{
    String longStr = "This is a very long string with no placeholders at all, "
                     "just regular text that should pass through unchanged.";
    String result = replacePlaceholdersWith(longStr, mockGetValue);
    TEST_ASSERT_EQUAL_STRING(longStr.c_str(), result.c_str());
}

// ── Runner ─────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Basic
    RUN_TEST(test_no_placeholders);
    RUN_TEST(test_single_placeholder);
    RUN_TEST(test_unknown_key_returns_na);
    RUN_TEST(test_empty_string);
    RUN_TEST(test_empty_key);

    // Multiple
    RUN_TEST(test_two_adjacent_placeholders);
    RUN_TEST(test_three_placeholders_with_text);

    // Bug regressions
    RUN_TEST(test_longer_replacement_does_not_skip_next);
    RUN_TEST(test_shorter_replacement_finds_next);
    RUN_TEST(test_many_placeholders_all_resolved);
    RUN_TEST(test_value_containing_double_braces_not_reparsed);
    RUN_TEST(test_value_containing_partial_braces);

    // Edge cases
    RUN_TEST(test_unclosed_placeholder_ignored);
    RUN_TEST(test_unmatched_closing_braces_ignored);
    RUN_TEST(test_nested_braces_treated_as_key);
    RUN_TEST(test_placeholder_at_end_of_string);
    RUN_TEST(test_placeholder_at_start_of_string);
    RUN_TEST(test_only_braces_no_content);
    RUN_TEST(test_replacement_to_empty_string);
    RUN_TEST(test_same_placeholder_twice);
    RUN_TEST(test_long_string_no_placeholders);

    return UNITY_END();
}
