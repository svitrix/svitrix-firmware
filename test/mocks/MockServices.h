#ifndef MOCK_SERVICES_H
#define MOCK_SERVICES_H

#include "Arduino.h"
#include "ISound.h"
#include "IPower.h"
#include "IUpdater.h"
#include "IPeripheryProvider.h"

// ---------------------------------------------------------------------------
// MockSound — tracks calls and last arguments for ISound
// ---------------------------------------------------------------------------
struct MockSound : public ISound
{
    int playRTTTLStringCalls = 0;
    int parseSoundCalls = 0;
    int r2d2Calls = 0;

    String lastRtttl;
    String lastSoundJson;
    String lastR2d2Msg;
    const char *rtttlReturn = "OK";
    bool parseSoundReturn = true;

    const char *playRTTTLString(String rtttl) override
    {
        playRTTTLStringCalls++;
        lastRtttl = rtttl;
        return rtttlReturn;
    }

    bool parseSound(const char *json) override
    {
        parseSoundCalls++;
        lastSoundJson = json;
        return parseSoundReturn;
    }

    void r2d2(const char *msg) override
    {
        r2d2Calls++;
        lastR2d2Msg = msg;
    }
};

// ---------------------------------------------------------------------------
// MockPower — tracks calls and last arguments for IPower
// ---------------------------------------------------------------------------
struct MockPower : public IPower
{
    int sleepCalls = 0;
    int sleepParserCalls = 0;

    uint64_t lastSleepSeconds = 0;
    String lastSleepJson;

    void sleep(uint64_t seconds) override
    {
        sleepCalls++;
        lastSleepSeconds = seconds;
    }

    void sleepParser(const char *json) override
    {
        sleepParserCalls++;
        lastSleepJson = json;
    }
};

// ---------------------------------------------------------------------------
// MockUpdater — tracks calls and last arguments for IUpdater
// ---------------------------------------------------------------------------
struct MockUpdater : public IUpdater
{
    int checkUpdateCalls = 0;
    int updateFirmwareCalls = 0;

    bool lastWithScreen = false;
    bool checkUpdateReturn = false;

    bool checkUpdate(bool withScreen) override
    {
        checkUpdateCalls++;
        lastWithScreen = withScreen;
        return checkUpdateReturn;
    }

    void updateFirmware() override
    {
        updateFirmwareCalls++;
    }
};

// ---------------------------------------------------------------------------
// MockPeripheryProvider — tracks calls for IPeripheryProvider
// ---------------------------------------------------------------------------
struct MockPeripheryProvider : public IPeripheryProvider
{
    int stopSoundCalls = 0;
    int setVolumeCalls = 0;
    int readUptimeCalls = 0;

    uint8_t lastVolume = 0;
    unsigned long long uptimeReturn = 12345;

    void stopSound() override { stopSoundCalls++; }

    void setVolume(uint8_t vol) override
    {
        setVolumeCalls++;
        lastVolume = vol;
    }

    unsigned long long readUptime() override
    {
        readUptimeCalls++;
        return uptimeReturn;
    }
};

#endif // MOCK_SERVICES_H
