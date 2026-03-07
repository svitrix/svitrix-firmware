#include <unity.h>
#include "AppRegistry.h"

// --- getNativeAppNames ---

void test_native_apps_count(void)
{
    std::vector<String> names = getNativeAppNames();
    TEST_ASSERT_EQUAL(5, names.size());
}

void test_native_apps_contains_time(void)
{
    std::vector<String> names = getNativeAppNames();
    bool found = false;
    for (size_t i = 0; i < names.size(); i++)
        if (names[i] == "Time") found = true;
    TEST_ASSERT_TRUE(found);
}

void test_native_apps_contains_all(void)
{
    std::vector<String> names = getNativeAppNames();
    const char* expected[] = {"Time", "Date", "Temperature", "Humidity", "Battery"};
    for (int i = 0; i < 5; i++)
    {
        bool found = false;
        for (size_t j = 0; j < names.size(); j++)
            if (names[j] == expected[i]) found = true;
        TEST_ASSERT_TRUE_MESSAGE(found, expected[i]);
    }
}

// --- isNativeApp ---

void test_is_native_app_time(void)
{
    TEST_ASSERT_TRUE(isNativeApp("Time"));
}

void test_is_native_app_battery(void)
{
    TEST_ASSERT_TRUE(isNativeApp("Battery"));
}

void test_is_native_app_unknown(void)
{
    TEST_ASSERT_FALSE(isNativeApp("Unknown"));
}

void test_is_native_app_empty(void)
{
    TEST_ASSERT_FALSE(isNativeApp(""));
}

void test_is_native_app_case_sensitive(void)
{
    // "time" (lowercase) should NOT match "Time"
    TEST_ASSERT_FALSE(isNativeApp("time"));
}

// --- serializeAppList ---

void test_serialize_empty(void)
{
    std::vector<String> empty;
    TEST_ASSERT_EQUAL_STRING("{}", serializeAppList(empty).c_str());
}

void test_serialize_single(void)
{
    std::vector<String> apps = {"Time"};
    TEST_ASSERT_EQUAL_STRING("{\"Time\":0}", serializeAppList(apps).c_str());
}

void test_serialize_multiple(void)
{
    std::vector<String> apps = {"Time", "Date", "MyApp"};
    String result = serializeAppList(apps);
    TEST_ASSERT_EQUAL_STRING("{\"Time\":0,\"Date\":1,\"MyApp\":2}", result.c_str());
}

void test_serialize_preserves_order(void)
{
    std::vector<String> apps = {"B", "A", "C"};
    String result = serializeAppList(apps);
    TEST_ASSERT_EQUAL_STRING("{\"B\":0,\"A\":1,\"C\":2}", result.c_str());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // getNativeAppNames
    RUN_TEST(test_native_apps_count);
    RUN_TEST(test_native_apps_contains_time);
    RUN_TEST(test_native_apps_contains_all);

    // isNativeApp
    RUN_TEST(test_is_native_app_time);
    RUN_TEST(test_is_native_app_battery);
    RUN_TEST(test_is_native_app_unknown);
    RUN_TEST(test_is_native_app_empty);
    RUN_TEST(test_is_native_app_case_sensitive);

    // serializeAppList
    RUN_TEST(test_serialize_empty);
    RUN_TEST(test_serialize_single);
    RUN_TEST(test_serialize_multiple);
    RUN_TEST(test_serialize_preserves_order);

    return UNITY_END();
}
