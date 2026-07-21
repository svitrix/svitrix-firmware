#include <unity.h>
#include "ResetReason.h"
#include <cstring>

void setUp() {}
void tearDown() {}

// --- known codes ---

void test_unknown_code_returns_unknown(void)
{
    TEST_ASSERT_EQUAL_STRING("unknown", resetReasonToString(0));
}

void test_poweron(void)
{
    TEST_ASSERT_EQUAL_STRING("poweron", resetReasonToString(1));
}

void test_ext_reset(void)
{
    TEST_ASSERT_EQUAL_STRING("ext", resetReasonToString(2));
}

void test_software_reset(void)
{
    TEST_ASSERT_EQUAL_STRING("software", resetReasonToString(3));
}

void test_panic(void)
{
    TEST_ASSERT_EQUAL_STRING("panic", resetReasonToString(4));
}

void test_interrupt_wdt(void)
{
    TEST_ASSERT_EQUAL_STRING("int_wdt", resetReasonToString(5));
}

void test_task_wdt(void)
{
    TEST_ASSERT_EQUAL_STRING("task_wdt", resetReasonToString(6));
}

void test_wdt(void)
{
    TEST_ASSERT_EQUAL_STRING("wdt", resetReasonToString(7));
}

void test_deepsleep(void)
{
    TEST_ASSERT_EQUAL_STRING("deepsleep", resetReasonToString(8));
}

void test_brownout(void)
{
    TEST_ASSERT_EQUAL_STRING("brownout", resetReasonToString(9));
}

void test_sdio(void)
{
    TEST_ASSERT_EQUAL_STRING("sdio", resetReasonToString(10));
}

// --- out of range ---

void test_eleven_falls_back_to_unknown(void)
{
    TEST_ASSERT_EQUAL_STRING("unknown", resetReasonToString(11));
}

void test_max_byte_falls_back_to_unknown(void)
{
    TEST_ASSERT_EQUAL_STRING("unknown", resetReasonToString(255));
}

// --- properties ---

void test_never_returns_nullptr(void)
{
    for (uint16_t code = 0; code <= 255; ++code)
    {
        const char* s = resetReasonToString(static_cast<uint8_t>(code));
        TEST_ASSERT_NOT_NULL(s);
        // Tag is non-empty
        TEST_ASSERT_TRUE(strlen(s) > 0);
    }
}

void test_unknown_constant_matches_function_output(void)
{
    // The exported constant should equal what the function returns for code 0.
    TEST_ASSERT_EQUAL_STRING(kResetReasonUnknown, resetReasonToString(0));
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_unknown_code_returns_unknown);
    RUN_TEST(test_poweron);
    RUN_TEST(test_ext_reset);
    RUN_TEST(test_software_reset);
    RUN_TEST(test_panic);
    RUN_TEST(test_interrupt_wdt);
    RUN_TEST(test_task_wdt);
    RUN_TEST(test_wdt);
    RUN_TEST(test_deepsleep);
    RUN_TEST(test_brownout);
    RUN_TEST(test_sdio);
    RUN_TEST(test_eleven_falls_back_to_unknown);
    RUN_TEST(test_max_byte_falls_back_to_unknown);
    RUN_TEST(test_never_returns_nullptr);
    RUN_TEST(test_unknown_constant_matches_function_output);
    return UNITY_END();
}
