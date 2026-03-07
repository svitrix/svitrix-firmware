#include <unity.h>
#include "MockServices.h"

// ═══════════════════════════════════════════════════════════════════════════
// Group 1: ISound
// ═══════════════════════════════════════════════════════════════════════════

void test_sound_play_rtttl(void)
{
    MockSound mock;
    ISound *iface = &mock;

    const char *result = iface->playRTTTLString("Imperial:d=4,o=5,b=100:e,e,e");

    TEST_ASSERT_EQUAL(1, mock.playRTTTLStringCalls);
    TEST_ASSERT_EQUAL_STRING("Imperial:d=4,o=5,b=100:e,e,e", mock.lastRtttl.c_str());
    TEST_ASSERT_EQUAL_STRING("OK", result);
}

void test_sound_parse_sound(void)
{
    MockSound mock;
    ISound *iface = &mock;

    mock.parseSoundReturn = true;
    bool result = iface->parseSound("{\"sound\":\"alarm\"}");

    TEST_ASSERT_EQUAL(1, mock.parseSoundCalls);
    TEST_ASSERT_EQUAL_STRING("{\"sound\":\"alarm\"}", mock.lastSoundJson.c_str());
    TEST_ASSERT_TRUE(result);
}

void test_sound_r2d2(void)
{
    MockSound mock;
    ISound *iface = &mock;

    iface->r2d2("beep boop");
    iface->r2d2("whistle");

    TEST_ASSERT_EQUAL(2, mock.r2d2Calls);
    TEST_ASSERT_EQUAL_STRING("whistle", mock.lastR2d2Msg.c_str());
}

// ═══════════════════════════════════════════════════════════════════════════
// Group 2: IPower
// ═══════════════════════════════════════════════════════════════════════════

void test_power_sleep(void)
{
    MockPower mock;
    IPower *iface = &mock;

    iface->sleep(3600);

    TEST_ASSERT_EQUAL(1, mock.sleepCalls);
    TEST_ASSERT_EQUAL_UINT64(3600, mock.lastSleepSeconds);
}

void test_power_sleep_parser(void)
{
    MockPower mock;
    IPower *iface = &mock;

    iface->sleepParser("{\"sleep\":7200}");

    TEST_ASSERT_EQUAL(1, mock.sleepParserCalls);
    TEST_ASSERT_EQUAL_STRING("{\"sleep\":7200}", mock.lastSleepJson.c_str());
}

// ═══════════════════════════════════════════════════════════════════════════
// Group 3: IUpdater
// ═══════════════════════════════════════════════════════════════════════════

void test_updater_check_update(void)
{
    MockUpdater mock;
    IUpdater *iface = &mock;

    mock.checkUpdateReturn = true;
    bool result = iface->checkUpdate(true);

    TEST_ASSERT_EQUAL(1, mock.checkUpdateCalls);
    TEST_ASSERT_TRUE(mock.lastWithScreen);
    TEST_ASSERT_TRUE(result);
}

void test_updater_firmware(void)
{
    MockUpdater mock;
    IUpdater *iface = &mock;

    iface->updateFirmware();
    iface->updateFirmware();

    TEST_ASSERT_EQUAL(2, mock.updateFirmwareCalls);
}

void test_updater_check_returns_false(void)
{
    MockUpdater mock;
    IUpdater *iface = &mock;

    mock.checkUpdateReturn = false;
    bool result = iface->checkUpdate(false);

    TEST_ASSERT_FALSE(mock.lastWithScreen);
    TEST_ASSERT_FALSE(result);
}

// ═══════════════════════════════════════════════════════════════════════════
// Group 4: IPeripheryProvider
// ═══════════════════════════════════════════════════════════════════════════

void test_periphery_set_volume(void)
{
    MockPeripheryProvider mock;
    IPeripheryProvider *iface = &mock;

    iface->setVolume(15);

    TEST_ASSERT_EQUAL(1, mock.setVolumeCalls);
    TEST_ASSERT_EQUAL(15, mock.lastVolume);
}

void test_periphery_read_uptime(void)
{
    MockPeripheryProvider mock;
    IPeripheryProvider *iface = &mock;

    mock.uptimeReturn = 99999;
    unsigned long long result = iface->readUptime();

    TEST_ASSERT_EQUAL(1, mock.readUptimeCalls);
    TEST_ASSERT_EQUAL_UINT64(99999, result);
}

void test_periphery_stop_sound(void)
{
    MockPeripheryProvider mock;
    IPeripheryProvider *iface = &mock;

    iface->stopSound();
    iface->stopSound();
    iface->stopSound();

    TEST_ASSERT_EQUAL(3, mock.stopSoundCalls);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test runner
// ═══════════════════════════════════════════════════════════════════════════

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // ISound
    RUN_TEST(test_sound_play_rtttl);
    RUN_TEST(test_sound_parse_sound);
    RUN_TEST(test_sound_r2d2);

    // IPower
    RUN_TEST(test_power_sleep);
    RUN_TEST(test_power_sleep_parser);

    // IUpdater
    RUN_TEST(test_updater_check_update);
    RUN_TEST(test_updater_firmware);
    RUN_TEST(test_updater_check_returns_false);

    // IPeripheryProvider
    RUN_TEST(test_periphery_set_volume);
    RUN_TEST(test_periphery_read_uptime);
    RUN_TEST(test_periphery_stop_sound);

    return UNITY_END();
}
