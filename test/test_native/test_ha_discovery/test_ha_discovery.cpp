#include <unity.h>
#include "HADiscovery.h"
#include <cstring>
#include <cstdio>

// ── buildEntityId ───────────────────────────────────────────────────

void test_buildEntityId_basic(void)
{
    char buf[40];
    buildEntityId("%s_mat", "abc123", buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("abc123_mat", buf);
}

void test_buildEntityId_longer_suffix(void)
{
    char buf[40];
    buildEntityId("%s_ip_address", "ff00ee", buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("ff00ee_ip_address", buf);
}

void test_buildEntityId_empty_mac(void)
{
    char buf[40];
    buildEntityId("%s_bat", "", buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("_bat", buf);
}

void test_buildEntityId_truncation(void)
{
    char buf[8]; // too small for "abc123_mat" (10 chars)
    buildEntityId("%s_mat", "abc123", buf, sizeof(buf));
    // snprintf truncates to bufSize-1 and null-terminates
    TEST_ASSERT_EQUAL(7, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("abc123_", buf);
}

// ── buildConfigUrl ──────────────────────────────────────────────────

void test_buildConfigUrl_basic(void)
{
    char buf[32];
    uint8_t ip[] = {192, 168, 1, 42};
    buildConfigUrl(ip, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("http://192.168.1.42", buf);
}

void test_buildConfigUrl_zeros(void)
{
    char buf[32];
    uint8_t ip[] = {0, 0, 0, 0};
    buildConfigUrl(ip, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("http://0.0.0.0", buf);
}

void test_buildConfigUrl_max_octets(void)
{
    char buf[32];
    uint8_t ip[] = {255, 255, 255, 255};
    buildConfigUrl(ip, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("http://255.255.255.255", buf);
}

// ── Device info ─────────────────────────────────────────────────────

void test_manufacturer(void)
{
    TEST_ASSERT_EQUAL_STRING("Blueforcer", getDeviceManufacturer());
}

void test_model(void)
{
    TEST_ASSERT_EQUAL_STRING("SVITRIX", getDeviceModel());
}

// ── Matrix light ────────────────────────────────────────────────────

void test_matrix_light_id(void)
{
    auto desc = getMatrixLightDescriptor();
    TEST_ASSERT_EQUAL_STRING("%s_mat", desc.idTemplate);
}

void test_matrix_light_name(void)
{
    auto desc = getMatrixLightDescriptor();
    TEST_ASSERT_EQUAL_STRING("Matrix", desc.name);
}

void test_matrix_light_icon(void)
{
    auto desc = getMatrixLightDescriptor();
    TEST_ASSERT_EQUAL_STRING("mdi:clock-digital", desc.icon);
}

void test_matrix_light_no_class(void)
{
    auto desc = getMatrixLightDescriptor();
    TEST_ASSERT_NULL(desc.deviceClass);
    TEST_ASSERT_NULL(desc.unit);
}

// ── Indicator lights ────────────────────────────────────────────────

void test_indicator_count(void)
{
    size_t count;
    getIndicatorLightDescriptors(count);
    TEST_ASSERT_EQUAL(3, count);
}

void test_indicator1_icon(void)
{
    size_t count;
    auto *descs = getIndicatorLightDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("mdi:arrow-top-right-thick", descs[0].icon);
    TEST_ASSERT_EQUAL_STRING("Indicator 1", descs[0].name);
}

void test_indicator2_icon(void)
{
    size_t count;
    auto *descs = getIndicatorLightDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("mdi:arrow-right-thick", descs[1].icon);
    TEST_ASSERT_EQUAL_STRING("Indicator 2", descs[1].name);
}

void test_indicator3_icon(void)
{
    size_t count;
    auto *descs = getIndicatorLightDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("mdi:arrow-bottom-right-thick", descs[2].icon);
    TEST_ASSERT_EQUAL_STRING("Indicator 3", descs[2].name);
}

void test_indicator_ids_unique(void)
{
    size_t count;
    auto *descs = getIndicatorLightDescriptors(count);
    TEST_ASSERT_NOT_EQUAL(0, strcmp(descs[0].idTemplate, descs[1].idTemplate));
    TEST_ASSERT_NOT_EQUAL(0, strcmp(descs[1].idTemplate, descs[2].idTemplate));
}

// ── Selects ─────────────────────────────────────────────────────────

void test_select_count(void)
{
    size_t count;
    getSelectDescriptors(count);
    TEST_ASSERT_EQUAL(2, count);
}

void test_brightness_select(void)
{
    size_t count;
    auto *descs = getSelectDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("Brightness mode", descs[0].base.name);
    TEST_ASSERT_EQUAL_STRING("mdi:brightness-auto", descs[0].base.icon);
    TEST_ASSERT_EQUAL_STRING("Manual;Auto", descs[0].options);
}

void test_effect_select(void)
{
    size_t count;
    auto *descs = getSelectDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("Transition effect", descs[1].base.name);
    // Options should contain all 11 transition names
    TEST_ASSERT_NOT_NULL(strstr(descs[1].options, "Random"));
    TEST_ASSERT_NOT_NULL(strstr(descs[1].options, "Slide"));
    TEST_ASSERT_NOT_NULL(strstr(descs[1].options, "Fade"));
}

// ── Buttons ─────────────────────────────────────────────────────────

void test_button_count(void)
{
    size_t count;
    getButtonDescriptors(count);
    TEST_ASSERT_EQUAL(4, count);
}

void test_button_names(void)
{
    size_t count;
    auto *descs = getButtonDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("Dismiss notification", descs[0].name);
    TEST_ASSERT_EQUAL_STRING("Start Update", descs[1].name);
    TEST_ASSERT_EQUAL_STRING("Next app", descs[2].name);
    TEST_ASSERT_EQUAL_STRING("Previous app", descs[3].name);
}

void test_button_icons(void)
{
    size_t count;
    auto *descs = getButtonDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("mdi:bell-off", descs[0].icon);
    TEST_ASSERT_EQUAL_STRING("mdi:update", descs[1].icon);
    TEST_ASSERT_EQUAL_STRING("mdi:arrow-right-bold", descs[2].icon);
    TEST_ASSERT_EQUAL_STRING("mdi:arrow-left-bold", descs[3].icon);
}

// ── Switch ──────────────────────────────────────────────────────────

void test_switch_descriptor(void)
{
    auto desc = getTransitionSwitchDescriptor();
    TEST_ASSERT_EQUAL_STRING("Transition", desc.name);
    TEST_ASSERT_EQUAL_STRING("mdi:swap-horizontal", desc.icon);
    TEST_ASSERT_EQUAL_STRING("%s_tra", desc.idTemplate);
}

// ── Sensors ─────────────────────────────────────────────────────────

void test_sensor_count_with_battery(void)
{
    size_t count;
    getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL(11, count);
}

void test_sensor_count_without_battery(void)
{
    size_t count;
    getSensorDescriptors(count, false);
    TEST_ASSERT_EQUAL(10, count);
}

void test_sensor_current_app(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Current app", descs[0].name);
    TEST_ASSERT_EQUAL_STRING("mdi:apps", descs[0].icon);
    TEST_ASSERT_NULL(descs[0].deviceClass);
}

void test_sensor_device_topic(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Device topic", descs[1].name);
    TEST_ASSERT_EQUAL_STRING("mdi:id-card", descs[1].icon);
}

void test_sensor_temperature(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Temperature", descs[2].name);
    TEST_ASSERT_EQUAL_STRING("temperature", descs[2].deviceClass);
    TEST_ASSERT_NOT_NULL(descs[2].unit); // °C
    TEST_ASSERT_EQUAL_STRING("mdi:thermometer", descs[2].icon);
}

void test_sensor_humidity(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Humidity", descs[3].name);
    TEST_ASSERT_EQUAL_STRING("humidity", descs[3].deviceClass);
    TEST_ASSERT_EQUAL_STRING("%", descs[3].unit);
}

void test_sensor_illuminance(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Illuminance", descs[4].name);
    TEST_ASSERT_EQUAL_STRING("illuminance", descs[4].deviceClass);
    TEST_ASSERT_EQUAL_STRING("lx", descs[4].unit);
}

void test_sensor_version_no_icon(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Version", descs[5].name);
    TEST_ASSERT_NULL(descs[5].icon);
    TEST_ASSERT_NULL(descs[5].deviceClass);
}

void test_sensor_wifi_strength(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("WiFi strength", descs[6].name);
    TEST_ASSERT_EQUAL_STRING("signal_strength", descs[6].deviceClass);
    TEST_ASSERT_EQUAL_STRING("dB", descs[6].unit);
}

void test_sensor_uptime(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Uptime", descs[7].name);
    TEST_ASSERT_EQUAL_STRING("duration", descs[7].deviceClass);
    TEST_ASSERT_EQUAL_STRING("s", descs[7].unit);
}

void test_sensor_ram(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Free ram", descs[8].name);
    TEST_ASSERT_EQUAL_STRING("data_size", descs[8].deviceClass);
    TEST_ASSERT_EQUAL_STRING("B", descs[8].unit);
    TEST_ASSERT_EQUAL_STRING("mdi:memory", descs[8].icon);
}

void test_sensor_ip_address(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("IP Address", descs[9].name);
    TEST_ASSERT_EQUAL_STRING("mdi:wifi", descs[9].icon);
}

void test_sensor_battery_last(void)
{
    size_t count;
    auto *descs = getSensorDescriptors(count, true);
    TEST_ASSERT_EQUAL_STRING("Battery", descs[10].name);
    TEST_ASSERT_EQUAL_STRING("battery", descs[10].deviceClass);
    TEST_ASSERT_EQUAL_STRING("%", descs[10].unit);
    TEST_ASSERT_EQUAL_STRING("mdi:battery-90", descs[10].icon);
}

// ── Binary sensors ──────────────────────────────────────────────────

void test_binary_sensor_count(void)
{
    size_t count;
    getBinarySensorDescriptors(count);
    TEST_ASSERT_EQUAL(3, count);
}

void test_binary_sensor_names(void)
{
    size_t count;
    auto *descs = getBinarySensorDescriptors(count);
    TEST_ASSERT_EQUAL_STRING("Button left", descs[0].name);
    TEST_ASSERT_EQUAL_STRING("Button select", descs[1].name);
    TEST_ASSERT_EQUAL_STRING("Button right", descs[2].name);
}

void test_binary_sensors_no_icon(void)
{
    size_t count;
    auto *descs = getBinarySensorDescriptors(count);
    for (size_t i = 0; i < count; i++)
    {
        TEST_ASSERT_NULL(descs[i].icon);
    }
}

// ── Total entity count ──────────────────────────────────────────────

void test_total_count_with_battery(void)
{
    TEST_ASSERT_EQUAL(25, getTotalEntityCount(true));
}

void test_total_count_without_battery(void)
{
    TEST_ASSERT_EQUAL(24, getTotalEntityCount(false));
}

void test_total_count_matches_sum(void)
{
    size_t indicators, selects, buttons, sensors, binarySensors;
    getIndicatorLightDescriptors(indicators);
    getSelectDescriptors(selects);
    getButtonDescriptors(buttons);
    getSensorDescriptors(sensors, true);
    getBinarySensorDescriptors(binarySensors);

    size_t expected = 1 + indicators + selects + buttons + 1 + sensors + binarySensors;
    TEST_ASSERT_EQUAL(expected, getTotalEntityCount(true));
}

// ── All entity IDs unique ───────────────────────────────────────────

void test_all_ids_unique(void)
{
    // Collect all idTemplates into a flat array
    const char *ids[30];
    size_t idx = 0;

    ids[idx++] = getMatrixLightDescriptor().idTemplate;

    size_t count;
    auto *indDescs = getIndicatorLightDescriptors(count);
    for (size_t i = 0; i < count; i++)
        ids[idx++] = indDescs[i].idTemplate;

    auto *selDescs = getSelectDescriptors(count);
    for (size_t i = 0; i < count; i++)
        ids[idx++] = selDescs[i].base.idTemplate;

    auto *btnDescs = getButtonDescriptors(count);
    for (size_t i = 0; i < count; i++)
        ids[idx++] = btnDescs[i].idTemplate;

    ids[idx++] = getTransitionSwitchDescriptor().idTemplate;

    auto *senDescs = getSensorDescriptors(count, true);
    for (size_t i = 0; i < count; i++)
        ids[idx++] = senDescs[i].idTemplate;

    auto *binDescs = getBinarySensorDescriptors(count);
    for (size_t i = 0; i < count; i++)
        ids[idx++] = binDescs[i].idTemplate;

    // Verify all pairs are unique
    for (size_t i = 0; i < idx; i++)
    {
        for (size_t j = i + 1; j < idx; j++)
        {
            if (strcmp(ids[i], ids[j]) == 0)
            {
                char msg[80];
                snprintf(msg, sizeof(msg), "Duplicate ID template: %s (indices %zu and %zu)", ids[i], i, j);
                TEST_FAIL_MESSAGE(msg);
            }
        }
    }
}

// ── Runner ──────────────────────────────────────────────────────────

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    // buildEntityId
    RUN_TEST(test_buildEntityId_basic);
    RUN_TEST(test_buildEntityId_longer_suffix);
    RUN_TEST(test_buildEntityId_empty_mac);
    RUN_TEST(test_buildEntityId_truncation);

    // buildConfigUrl
    RUN_TEST(test_buildConfigUrl_basic);
    RUN_TEST(test_buildConfigUrl_zeros);
    RUN_TEST(test_buildConfigUrl_max_octets);

    // Device info
    RUN_TEST(test_manufacturer);
    RUN_TEST(test_model);

    // Matrix light
    RUN_TEST(test_matrix_light_id);
    RUN_TEST(test_matrix_light_name);
    RUN_TEST(test_matrix_light_icon);
    RUN_TEST(test_matrix_light_no_class);

    // Indicators
    RUN_TEST(test_indicator_count);
    RUN_TEST(test_indicator1_icon);
    RUN_TEST(test_indicator2_icon);
    RUN_TEST(test_indicator3_icon);
    RUN_TEST(test_indicator_ids_unique);

    // Selects
    RUN_TEST(test_select_count);
    RUN_TEST(test_brightness_select);
    RUN_TEST(test_effect_select);

    // Buttons
    RUN_TEST(test_button_count);
    RUN_TEST(test_button_names);
    RUN_TEST(test_button_icons);

    // Switch
    RUN_TEST(test_switch_descriptor);

    // Sensors
    RUN_TEST(test_sensor_count_with_battery);
    RUN_TEST(test_sensor_count_without_battery);
    RUN_TEST(test_sensor_current_app);
    RUN_TEST(test_sensor_device_topic);
    RUN_TEST(test_sensor_temperature);
    RUN_TEST(test_sensor_humidity);
    RUN_TEST(test_sensor_illuminance);
    RUN_TEST(test_sensor_version_no_icon);
    RUN_TEST(test_sensor_wifi_strength);
    RUN_TEST(test_sensor_uptime);
    RUN_TEST(test_sensor_ram);
    RUN_TEST(test_sensor_ip_address);
    RUN_TEST(test_sensor_battery_last);

    // Binary sensors
    RUN_TEST(test_binary_sensor_count);
    RUN_TEST(test_binary_sensor_names);
    RUN_TEST(test_binary_sensors_no_icon);

    // Total count
    RUN_TEST(test_total_count_with_battery);
    RUN_TEST(test_total_count_without_battery);
    RUN_TEST(test_total_count_matches_sum);

    // Uniqueness
    RUN_TEST(test_all_ids_unique);

    return UNITY_END();
}
