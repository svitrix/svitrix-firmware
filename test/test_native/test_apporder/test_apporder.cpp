#include <unity.h>
#include "AppOrderUtils.h"

// Helper: assert a result vector matches an expected list of names in order.
static void assertOrder(const std::vector<String>& result,
                        const std::vector<const char*>& expected)
{
    TEST_ASSERT_EQUAL_size_t(expected.size(), result.size());
    for (size_t i = 0; i < expected.size(); i++)
    {
        TEST_ASSERT_EQUAL_STRING(expected[i], result[i].c_str());
    }
}

// --- empty inputs ---

void test_empty_desired_returns_empty(void)
{
    std::vector<String> saved = {"Time", "Date"};
    std::vector<String> desired;
    assertOrder(orderApps(saved, desired), {});
}

void test_empty_saved_returns_desired_as_is(void)
{
    std::vector<String> saved;
    std::vector<String> desired = {"Time", "Date", "Battery"};
    assertOrder(orderApps(saved, desired), {"Time", "Date", "Battery"});
}

void test_both_empty(void)
{
    std::vector<String> saved;
    std::vector<String> desired;
    assertOrder(orderApps(saved, desired), {});
}

// --- exact ordering ---

void test_saved_order_applied_exactly(void)
{
    std::vector<String> saved = {"Date", "Time", "Battery"};
    std::vector<String> desired = {"Time", "Date", "Battery"};
    assertOrder(orderApps(saved, desired), {"Date", "Time", "Battery"});
}

void test_interleaved_native_weather_custom(void)
{
    std::vector<String> saved = {"Time", "OutdoorTemp", "Date", "myapp"};
    std::vector<String> desired = {"Time", "Date", "OutdoorTemp", "myapp"};
    assertOrder(orderApps(saved, desired), {"Time", "OutdoorTemp", "Date", "myapp"});
}

// --- new apps appended ---

void test_new_app_appended_at_end(void)
{
    std::vector<String> saved = {"Time", "Date"};
    std::vector<String> desired = {"Time", "Date", "OutdoorTemp"};
    assertOrder(orderApps(saved, desired), {"Time", "Date", "OutdoorTemp"});
}

void test_multiple_new_apps_keep_desired_order(void)
{
    std::vector<String> saved = {"Date"};
    std::vector<String> desired = {"Time", "Date", "Battery", "Humidity"};
    // Date first (from saved), then remaining in desired order.
    assertOrder(orderApps(saved, desired), {"Date", "Time", "Battery", "Humidity"});
}

// --- disabled / removed apps dropped ---

void test_saved_name_not_in_desired_is_dropped(void)
{
    // "Battery" was reordered earlier but is now disabled -> not in desired.
    std::vector<String> saved = {"Time", "Battery", "Date"};
    std::vector<String> desired = {"Time", "Date"};
    assertOrder(orderApps(saved, desired), {"Time", "Date"});
}

void test_all_saved_dropped_falls_back_to_desired(void)
{
    std::vector<String> saved = {"Foo", "Bar"};
    std::vector<String> desired = {"Time", "Date"};
    assertOrder(orderApps(saved, desired), {"Time", "Date"});
}

// --- duplicates ---

void test_duplicate_in_saved_emitted_once(void)
{
    std::vector<String> saved = {"Time", "Time", "Date"};
    std::vector<String> desired = {"Time", "Date"};
    assertOrder(orderApps(saved, desired), {"Time", "Date"});
}

void test_duplicate_in_desired_emitted_once(void)
{
    std::vector<String> saved = {"Date", "Time"};
    std::vector<String> desired = {"Time", "Date", "Time"};
    assertOrder(orderApps(saved, desired), {"Date", "Time"});
}

// --- multi-page custom apps (name0, name1) ---

void test_multipage_custom_apps_preserved(void)
{
    std::vector<String> saved = {"Time", "weather0", "weather1", "Date"};
    std::vector<String> desired = {"Time", "Date", "weather0", "weather1"};
    assertOrder(orderApps(saved, desired), {"Time", "weather0", "weather1", "Date"});
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_empty_desired_returns_empty);
    RUN_TEST(test_empty_saved_returns_desired_as_is);
    RUN_TEST(test_both_empty);

    RUN_TEST(test_saved_order_applied_exactly);
    RUN_TEST(test_interleaved_native_weather_custom);

    RUN_TEST(test_new_app_appended_at_end);
    RUN_TEST(test_multiple_new_apps_keep_desired_order);

    RUN_TEST(test_saved_name_not_in_desired_is_dropped);
    RUN_TEST(test_all_saved_dropped_falls_back_to_desired);

    RUN_TEST(test_duplicate_in_saved_emitted_once);
    RUN_TEST(test_duplicate_in_desired_emitted_once);

    RUN_TEST(test_multipage_custom_apps_preserved);

    return UNITY_END();
}
