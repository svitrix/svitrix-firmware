#include <unity.h>
#include "StatsBuilder.h"
#include <cstring>

// Helper: create a StatsData with reasonable defaults
static StatsData makeDefaults()
{
    StatsData d;
    d.batteryPercent = 85;
    d.batteryRaw = 620;
    d.lux = 350;
    d.ldrRaw = 780;
    d.freeRam = 120000;
    d.brightness = 120;
    d.hasSensor = true;
    d.temperature = 23.5f;
    d.tempDecimalPlaces = 1;
    d.humidity = 45;
    d.uptime = "1d 3h";
    d.wifiSignal = -52;
    d.receivedMessages = 1024;
    d.version = "0.96";
    d.indicator1 = true;
    d.indicator2 = false;
    d.indicator3 = false;
    d.currentApp = "Time";
    d.uid = "awtrix_abc123";
    d.matrixOn = true;
    d.ipAddress = "192.168.1.100";
    return d;
}

// Helper: check if JSON string contains a substring
static bool jsonContains(const String& json, const char* fragment)
{
    return strstr(json.c_str(), fragment) != nullptr;
}

// --- structure ---

void test_json_starts_with_brace(void)
{
    String json = buildStatsJson(makeDefaults());
    TEST_ASSERT_EQUAL('{', json.charAt(0));
}

void test_json_ends_with_brace(void)
{
    String json = buildStatsJson(makeDefaults());
    TEST_ASSERT_EQUAL('}', json.charAt(json.length() - 1));
}

// --- all expected keys present ---

void test_json_contains_all_keys(void)
{
    String json = buildStatsJson(makeDefaults());
    TEST_ASSERT_TRUE(jsonContains(json, "\"bat\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"bat_raw\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"type\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"lux\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"ldr_raw\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"ram\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"bri\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"temp\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"hum\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"uptime\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"wifi_signal\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"messages\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"version\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"indicator1\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"indicator2\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"indicator3\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"app\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"uid\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"matrix\":"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"ip_address\":"));
}

// --- sensor conditional fields ---

void test_json_with_sensor_includes_temp_hum(void)
{
    StatsData d = makeDefaults();
    d.hasSensor = true;
    d.temperature = 22.3f;
    d.humidity = 60;
    String json = buildStatsJson(d);
    TEST_ASSERT_TRUE(jsonContains(json, "\"temp\":22.3"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"hum\":60"));
}

void test_json_without_sensor_excludes_temp_hum(void)
{
    StatsData d = makeDefaults();
    d.hasSensor = false;
    String json = buildStatsJson(d);
    TEST_ASSERT_FALSE(jsonContains(json, "\"temp\":"));
    TEST_ASSERT_FALSE(jsonContains(json, "\"hum\":"));
}

// --- value formatting ---

void test_json_boolean_values(void)
{
    StatsData d = makeDefaults();
    d.indicator1 = true;
    d.indicator2 = false;
    d.matrixOn = true;
    String json = buildStatsJson(d);
    TEST_ASSERT_TRUE(jsonContains(json, "\"indicator1\":true"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"indicator2\":false"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"matrix\":true"));
}

void test_json_string_values_quoted(void)
{
    StatsData d = makeDefaults();
    d.version = "0.96";
    d.uid = "test_uid";
    String json = buildStatsJson(d);
    TEST_ASSERT_TRUE(jsonContains(json, "\"version\":\"0.96\""));
    TEST_ASSERT_TRUE(jsonContains(json, "\"uid\":\"test_uid\""));
}

void test_json_numeric_values(void)
{
    StatsData d = makeDefaults();
    d.batteryPercent = 100;
    d.brightness = 160;
    d.lux = 0;
    String json = buildStatsJson(d);
    TEST_ASSERT_TRUE(jsonContains(json, "\"bat\":100"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"bri\":160"));
    TEST_ASSERT_TRUE(jsonContains(json, "\"lux\":0"));
}

void test_json_negative_wifi_signal(void)
{
    StatsData d = makeDefaults();
    d.wifiSignal = -85;
    String json = buildStatsJson(d);
    TEST_ASSERT_TRUE(jsonContains(json, "\"wifi_signal\":-85"));
}

void test_json_negative_temperature(void)
{
    StatsData d = makeDefaults();
    d.hasSensor = true;
    d.temperature = -5.2f;
    d.tempDecimalPlaces = 1;
    String json = buildStatsJson(d);
    TEST_ASSERT_TRUE(jsonContains(json, "\"temp\":-5.2"));
}

void test_json_type_always_zero(void)
{
    String json = buildStatsJson(makeDefaults());
    TEST_ASSERT_TRUE(jsonContains(json, "\"type\":0"));
}

void test_json_large_ram_value(void)
{
    StatsData d = makeDefaults();
    d.freeRam = 4000000;
    String json = buildStatsJson(d);
    TEST_ASSERT_TRUE(jsonContains(json, "\"ram\":4000000"));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // structure
    RUN_TEST(test_json_starts_with_brace);
    RUN_TEST(test_json_ends_with_brace);

    // all keys
    RUN_TEST(test_json_contains_all_keys);

    // sensor conditional
    RUN_TEST(test_json_with_sensor_includes_temp_hum);
    RUN_TEST(test_json_without_sensor_excludes_temp_hum);

    // value formatting
    RUN_TEST(test_json_boolean_values);
    RUN_TEST(test_json_string_values_quoted);
    RUN_TEST(test_json_numeric_values);
    RUN_TEST(test_json_negative_wifi_signal);
    RUN_TEST(test_json_negative_temperature);
    RUN_TEST(test_json_type_always_zero);
    RUN_TEST(test_json_large_ram_value);

    return UNITY_END();
}
