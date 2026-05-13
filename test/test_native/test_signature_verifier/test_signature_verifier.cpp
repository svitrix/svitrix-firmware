#include <unity.h>
#include "SignatureVerifier.h"

void setUp() {}
void tearDown() {}

// Helper: build a base64 string of the requested decoded byte length.
// Output uses 'A' for every character + correct padding.
static String makeFakeBase64(size_t decodedBytes)
{
    // base64 encodes 3 bytes -> 4 chars; pad to align.
    size_t b64Chars = ((decodedBytes + 2) / 3) * 4;
    size_t padCount = (3 - (decodedBytes % 3)) % 3;
    String s;
    for (size_t i = 0; i < b64Chars - padCount; ++i)
        s += 'A';
    for (size_t i = 0; i < padCount; ++i)
        s += '=';
    return s;
}

// Helper: replace one character in a String (native mock lacks setCharAt).
static String replaceCharAt(const String& s, size_t index, char c)
{
    String out;
    for (size_t i = 0; i < s.length(); ++i)
        out += (i == index) ? c : s.charAt(i);
    return out;
}

// --- buildSignatureUrl ---

void test_build_sig_url_appends_dot_sig(void)
{
    TEST_ASSERT_EQUAL_STRING(
        "https://example.com/firmware.bin.sig",
        buildSignatureUrl("https://example.com/firmware.bin").c_str());
}

void test_build_sig_url_empty_returns_empty(void)
{
    TEST_ASSERT_EQUAL_STRING("", buildSignatureUrl("").c_str());
}

void test_build_sig_url_preserves_query_string(void)
{
    // We don't strip query strings — sig URL must match server's served path.
    TEST_ASSERT_EQUAL_STRING(
        "https://example.com/firmware.bin?v=1.sig",
        buildSignatureUrl("https://example.com/firmware.bin?v=1").c_str());
}

// --- isPlausibleRsa2048SignatureBase64: empty/whitespace ---

void test_plausibility_empty_string(void)
{
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64(""));
}

void test_plausibility_only_whitespace(void)
{
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64("   \n\r\t  "));
}

// --- isPlausibleRsa2048SignatureBase64: invalid chars ---

void test_plausibility_html_error_page(void)
{
    // 404 might come back as an HTML page — non-base64 chars must fail.
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64("<html>Not Found</html>"));
}

void test_plausibility_with_exclamation_mark(void)
{
    String s = replaceCharAt(makeFakeBase64(256), 10, '!');
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64(s));
}

// --- isPlausibleRsa2048SignatureBase64: wrong length ---

void test_plausibility_too_short(void)
{
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64(makeFakeBase64(16)));
}

void test_plausibility_too_long(void)
{
    // RSA-4096 signature would be 512 bytes — outside the ±2 slack.
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64(makeFakeBase64(512)));
}

void test_plausibility_truncated(void)
{
    // Common failure mode: download cut off mid-file.
    String full = makeFakeBase64(256);
    String truncated = full.substring(0, full.length() / 2);
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64(truncated));
}

// --- isPlausibleRsa2048SignatureBase64: bad padding ---

void test_plausibility_excess_padding(void)
{
    String s = makeFakeBase64(256);
    s += "==="; // 3 extra '=' would make total length non-multiple-of-4 too
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64(s));
}

void test_plausibility_not_multiple_of_four(void)
{
    String s = makeFakeBase64(256);
    s = s.substring(0, s.length() - 1);  // drop one char → length mod 4 == 3
    TEST_ASSERT_FALSE(isPlausibleRsa2048SignatureBase64(s));
}

// --- isPlausibleRsa2048SignatureBase64: valid happy path ---

void test_plausibility_exact_256_bytes(void)
{
    TEST_ASSERT_TRUE(isPlausibleRsa2048SignatureBase64(makeFakeBase64(256)));
}

void test_plausibility_with_trailing_newline(void)
{
    String s = makeFakeBase64(256);
    s += "\n";
    TEST_ASSERT_TRUE(isPlausibleRsa2048SignatureBase64(s));
}

void test_plausibility_with_internal_whitespace(void)
{
    // openssl base64 -A puts everything on one line, but openssl base64
    // (default) wraps at 64 chars with newlines. Must accept both.
    String s = makeFakeBase64(256);
    String wrapped;
    for (size_t i = 0; i < s.length(); ++i)
    {
        wrapped += s.charAt(i);
        if ((i + 1) % 64 == 0)
            wrapped += '\n';
    }
    TEST_ASSERT_TRUE(isPlausibleRsa2048SignatureBase64(wrapped));
}

void test_plausibility_within_slack_lower(void)
{
    // 254 bytes is within ±2 of 256 — still plausible.
    TEST_ASSERT_TRUE(isPlausibleRsa2048SignatureBase64(makeFakeBase64(254)));
}

void test_plausibility_within_slack_upper(void)
{
    TEST_ASSERT_TRUE(isPlausibleRsa2048SignatureBase64(makeFakeBase64(258)));
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_build_sig_url_appends_dot_sig);
    RUN_TEST(test_build_sig_url_empty_returns_empty);
    RUN_TEST(test_build_sig_url_preserves_query_string);
    RUN_TEST(test_plausibility_empty_string);
    RUN_TEST(test_plausibility_only_whitespace);
    RUN_TEST(test_plausibility_html_error_page);
    RUN_TEST(test_plausibility_with_exclamation_mark);
    RUN_TEST(test_plausibility_too_short);
    RUN_TEST(test_plausibility_too_long);
    RUN_TEST(test_plausibility_truncated);
    RUN_TEST(test_plausibility_excess_padding);
    RUN_TEST(test_plausibility_not_multiple_of_four);
    RUN_TEST(test_plausibility_exact_256_bytes);
    RUN_TEST(test_plausibility_with_trailing_newline);
    RUN_TEST(test_plausibility_with_internal_whitespace);
    RUN_TEST(test_plausibility_within_slack_lower);
    RUN_TEST(test_plausibility_within_slack_upper);
    return UNITY_END();
}
