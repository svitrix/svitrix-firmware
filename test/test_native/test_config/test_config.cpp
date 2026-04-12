#include <unity.h>
#include "ConfigTypes.h"

void setUp(void) {}
void tearDown(void) {}

// --- AuthConfig ---

void test_auth_config_defaults()
{
    AuthConfig cfg = {"", "svitrix"};
    TEST_ASSERT_EQUAL_STRING("", cfg.user.c_str());
    TEST_ASSERT_EQUAL_STRING("svitrix", cfg.pass.c_str());
}

void test_auth_config_assignment()
{
    AuthConfig cfg;
    cfg.user = "admin";
    cfg.pass = "secret";
    TEST_ASSERT_EQUAL_STRING("admin", cfg.user.c_str());
    TEST_ASSERT_EQUAL_STRING("secret", cfg.pass.c_str());
}

// --- NetworkConfig ---

void test_network_config_defaults()
{
    NetworkConfig cfg = {false, "192.168.178.10", "192.168.178.1", "255.255.255.0", "8.8.8.8", "1.1.1.1"};
    TEST_ASSERT_FALSE(cfg.isStatic);
    TEST_ASSERT_EQUAL_STRING("192.168.178.10", cfg.ip.c_str());
    TEST_ASSERT_EQUAL_STRING("192.168.178.1", cfg.gateway.c_str());
    TEST_ASSERT_EQUAL_STRING("255.255.255.0", cfg.subnet.c_str());
    TEST_ASSERT_EQUAL_STRING("8.8.8.8", cfg.primaryDns.c_str());
    TEST_ASSERT_EQUAL_STRING("1.1.1.1", cfg.secondaryDns.c_str());
}

// --- BatteryConfig ---

void test_battery_config_defaults()
{
    BatteryConfig cfg = {0, 0, 475, 665};
    TEST_ASSERT_EQUAL(0, cfg.percent);
    TEST_ASSERT_EQUAL(0, cfg.raw);
    TEST_ASSERT_EQUAL(475, cfg.minRaw);
    TEST_ASSERT_EQUAL(665, cfg.maxRaw);
}

void test_battery_config_percent_range()
{
    BatteryConfig cfg = {100, 665, 475, 665};
    TEST_ASSERT_EQUAL(100, cfg.percent);
    TEST_ASSERT_EQUAL(665, cfg.raw);
}

// --- HaConfig ---

void test_ha_config_defaults()
{
    HaConfig cfg = {false, "homeassistant"};
    TEST_ASSERT_FALSE(cfg.discovery);
    TEST_ASSERT_EQUAL_STRING("homeassistant", cfg.prefix.c_str());
}

// --- MqttConfig ---

void test_mqtt_config_defaults()
{
    MqttConfig cfg = {"", 1883, "", "", ""};
    TEST_ASSERT_EQUAL_STRING("", cfg.host.c_str());
    TEST_ASSERT_EQUAL(1883, cfg.port);
    TEST_ASSERT_EQUAL_STRING("", cfg.user.c_str());
    TEST_ASSERT_EQUAL_STRING("", cfg.pass.c_str());
    TEST_ASSERT_EQUAL_STRING("", cfg.prefix.c_str());
}

void test_mqtt_config_custom_port()
{
    MqttConfig cfg;
    cfg.host = "broker.local";
    cfg.port = 8883;
    cfg.user = "user";
    cfg.pass = "pass";
    cfg.prefix = "svitrix_abc123";
    TEST_ASSERT_EQUAL_STRING("broker.local", cfg.host.c_str());
    TEST_ASSERT_EQUAL(8883, cfg.port);
    TEST_ASSERT_EQUAL_STRING("svitrix_abc123", cfg.prefix.c_str());
}

// --- SensorConfig ---

void test_sensor_config_defaults()
{
    SensorConfig cfg = {0, 0, 0, 0, TEMP_SENSOR_TYPE_NONE, true, false, -9, 0};
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, cfg.currentTemp);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, cfg.currentHum);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, cfg.currentLux);
    TEST_ASSERT_EQUAL(0, cfg.ldrRaw);
    TEST_ASSERT_EQUAL(TEMP_SENSOR_TYPE_NONE, cfg.tempSensorType);
    TEST_ASSERT_TRUE(cfg.sensorReading);
    TEST_ASSERT_FALSE(cfg.sensorsStable);
    TEST_ASSERT_FLOAT_WITHIN(0.01, -9.0, cfg.tempOffset);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, cfg.humOffset);
}

void test_sensor_type_constants()
{
    TEST_ASSERT_EQUAL(0, TEMP_SENSOR_TYPE_NONE);
    TEST_ASSERT_EQUAL(1, TEMP_SENSOR_TYPE_BME280);
    TEST_ASSERT_EQUAL(2, TEMP_SENSOR_TYPE_HTU21DF);
    TEST_ASSERT_EQUAL(3, TEMP_SENSOR_TYPE_BMP280);
    TEST_ASSERT_EQUAL(4, TEMP_SENSOR_TYPE_SHT31);
}

// --- DisplayConfig ---

void test_display_config_defaults()
{
    DisplayConfig cfg = {0, 42, false, false, false, true, -1};
    TEST_ASSERT_EQUAL(0, cfg.matrixLayout);
    TEST_ASSERT_EQUAL(42, cfg.matrixFps);
    TEST_ASSERT_FALSE(cfg.matrixOff);
    TEST_ASSERT_FALSE(cfg.mirrorDisplay);
    TEST_ASSERT_FALSE(cfg.rotateScreen);
    TEST_ASSERT_TRUE(cfg.uppercaseLetters);
    TEST_ASSERT_EQUAL(-1, cfg.backgroundEffect);
}

// --- BrightnessConfig ---

void test_brightness_config_defaults()
{
    BrightnessConfig cfg = {120, 0, true, 2, 160, 3.0, 1.0, false};
    TEST_ASSERT_EQUAL(120, cfg.brightness);
    TEST_ASSERT_EQUAL(0, cfg.brightnessPercent);
    TEST_ASSERT_TRUE(cfg.autoBrightness);
    TEST_ASSERT_EQUAL(2, cfg.minBrightness);
    TEST_ASSERT_EQUAL(160, cfg.maxBrightness);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.0, cfg.ldrGamma);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1.0, cfg.ldrFactor);
    TEST_ASSERT_FALSE(cfg.ldrOnGround);
}

// --- ColorConfig ---

void test_color_config_defaults()
{
    ColorConfig cfg = {0xFFFFFF, 0, 0, 0, 0, 0, 0xFFFFFF, 0x666666, 0xFF0000, 0x000000, 0xFFFFFF};
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFF, cfg.textColor);
    TEST_ASSERT_EQUAL_HEX32(0, cfg.timeColor);
    TEST_ASSERT_EQUAL_HEX32(0, cfg.dateColor);
    TEST_ASSERT_EQUAL_HEX32(0, cfg.batColor);
    TEST_ASSERT_EQUAL_HEX32(0, cfg.tempColor);
    TEST_ASSERT_EQUAL_HEX32(0, cfg.humColor);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFF, cfg.wdcActive);
    TEST_ASSERT_EQUAL_HEX32(0x666666, cfg.wdcInactive);
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, cfg.calendarHeaderColor);
    TEST_ASSERT_EQUAL_HEX32(0x000000, cfg.calendarTextColor);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFF, cfg.calendarBodyColor);
}

// --- TimeConfig ---

void test_time_config_defaults()
{
    TimeConfig cfg = {"%H:%M:%S", "%d.%m.%y", 1, false, "de.pool.ntp.org", "CET-1CEST,M3.5.0,M10.5.0/3", false, 0};
    TEST_ASSERT_EQUAL_STRING("%H:%M:%S", cfg.timeFormat.c_str());
    TEST_ASSERT_EQUAL_STRING("%d.%m.%y", cfg.dateFormat.c_str());
    TEST_ASSERT_EQUAL(1, cfg.timeMode);
    TEST_ASSERT_FALSE(cfg.startOnMonday);
    TEST_ASSERT_EQUAL_STRING("de.pool.ntp.org", cfg.ntpServer.c_str());
    TEST_ASSERT_FALSE(cfg.isCelsius);
    TEST_ASSERT_EQUAL(0, cfg.tempDecimalPlaces);
}

// --- AppConfig ---

void test_app_config_defaults()
{
    AppConfig cfg = {true, true, true, true, true, true, false, 1, 400, 7000, 100, IconLayout::Left, false, false, 1260, 360, 5, 0xFF0000};
    TEST_ASSERT_TRUE(cfg.showTime);
    TEST_ASSERT_TRUE(cfg.showDate);
    TEST_ASSERT_TRUE(cfg.showBat);
    TEST_ASSERT_TRUE(cfg.showTemp);
    TEST_ASSERT_TRUE(cfg.showHum);
    TEST_ASSERT_TRUE(cfg.showWeekday);
    TEST_ASSERT_FALSE(cfg.autoTransition);
    TEST_ASSERT_EQUAL(1, cfg.transEffect);
    TEST_ASSERT_EQUAL(400, cfg.timePerTransition);
    TEST_ASSERT_EQUAL(7000, cfg.timePerApp);
    TEST_ASSERT_EQUAL(100, cfg.scrollSpeed);
    TEST_ASSERT_FALSE(cfg.blockNavigation);
}

// --- AudioConfig ---

void test_audio_config_defaults()
{
    AudioConfig cfg = {false, 30, ""};
    TEST_ASSERT_FALSE(cfg.soundActive);
    TEST_ASSERT_EQUAL(30, cfg.soundVolume);
    TEST_ASSERT_EQUAL_STRING("", cfg.bootSound.c_str());
}

// --- SystemConfig ---

void test_system_config_defaults()
{
    SystemConfig cfg = {true, 15, 80, "", false, 10000, false, false, "", "", false, false};
    TEST_ASSERT_TRUE(cfg.debugMode);
    TEST_ASSERT_EQUAL(15, cfg.apTimeout);
    TEST_ASSERT_EQUAL(80, cfg.webPort);
    TEST_ASSERT_EQUAL_STRING("", cfg.hostname.c_str());
    TEST_ASSERT_FALSE(cfg.updateCheck);
    TEST_ASSERT_EQUAL(10000, cfg.statsInterval);
    TEST_ASSERT_FALSE(cfg.newyear);
    TEST_ASSERT_FALSE(cfg.swapButtons);
    TEST_ASSERT_EQUAL_STRING("", cfg.buttonCallback.c_str());
    TEST_ASSERT_EQUAL_STRING("", cfg.deviceId.c_str());
    TEST_ASSERT_FALSE(cfg.updateAvailable);
    TEST_ASSERT_FALSE(cfg.apMode);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    // Phase 6 structs
    RUN_TEST(test_auth_config_defaults);
    RUN_TEST(test_auth_config_assignment);
    RUN_TEST(test_network_config_defaults);
    RUN_TEST(test_battery_config_defaults);
    RUN_TEST(test_battery_config_percent_range);
    RUN_TEST(test_ha_config_defaults);
    RUN_TEST(test_mqtt_config_defaults);
    RUN_TEST(test_mqtt_config_custom_port);
    RUN_TEST(test_sensor_config_defaults);
    RUN_TEST(test_sensor_type_constants);
    // Phase 6b structs
    RUN_TEST(test_display_config_defaults);
    RUN_TEST(test_brightness_config_defaults);
    RUN_TEST(test_color_config_defaults);
    RUN_TEST(test_time_config_defaults);
    RUN_TEST(test_app_config_defaults);
    RUN_TEST(test_audio_config_defaults);
    RUN_TEST(test_system_config_defaults);
    return UNITY_END();
}
