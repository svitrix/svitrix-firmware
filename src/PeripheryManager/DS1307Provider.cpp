#include "DS1307Provider.h"
#include "Globals.h"

bool DS1307Provider::begin()
{
    Wire.beginTransmission(kI2cAddress);
    detected_ = (Wire.endTransmission() == 0);

    if (detected_ && systemConfig.debugMode)
    {
        DEBUG_PRINTLN(F("DS1307 RTC detected"));
    }

    return detected_;
}

bool DS1307Provider::isRunning()
{
    if (!detected_)
        return false;

    uint8_t seconds = 0;
    if (!readRegisters(kRegSeconds, &seconds, 1))
        return false;

    return (seconds & kClockHaltBit) == 0;
}

time_t DS1307Provider::getTime()
{
    if (!detected_)
        return 0;

    uint8_t buffer[7];
    if (!readRegisters(kRegSeconds, buffer, 7))
        return 0;

    if (buffer[0] & kClockHaltBit)
        return 0;

    struct tm t = {};
    t.tm_sec = bcdToDec(buffer[0] & 0x7F);
    t.tm_min = bcdToDec(buffer[1]);
    t.tm_hour = bcdToDec(buffer[2] & 0x3F);
    t.tm_mday = bcdToDec(buffer[4]);
    t.tm_mon = bcdToDec(buffer[5]) - 1;
    t.tm_year = bcdToDec(buffer[6]) + 100;

    return mktime(&t);
}

bool DS1307Provider::setTime(time_t epoch)
{
    if (!detected_)
        return false;

    struct tm *t = gmtime(&epoch);
    if (!t)
        return false;

    uint8_t buffer[7];
    buffer[0] = decToBcd(t->tm_sec) & 0x7F;
    buffer[1] = decToBcd(t->tm_min);
    buffer[2] = decToBcd(t->tm_hour);
    buffer[3] = decToBcd(t->tm_wday + 1);
    buffer[4] = decToBcd(t->tm_mday);
    buffer[5] = decToBcd(t->tm_mon + 1);
    buffer[6] = decToBcd(t->tm_year - 100);

    if (!writeRegisters(kRegSeconds, buffer, 7))
        return false;

    if (systemConfig.debugMode)
    {
        DEBUG_PRINTF("RTC time set to %04d-%02d-%02d %02d:%02d:%02d\n",
                     t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                     t->tm_hour, t->tm_min, t->tm_sec);
    }

    return true;
}

bool DS1307Provider::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t count)
{
    Wire.beginTransmission(kI2cAddress);
    Wire.write(reg);
    if (Wire.endTransmission() != 0)
        return false;

    Wire.requestFrom(kI2cAddress, count);
    if (Wire.available() < count)
        return false;

    for (uint8_t i = 0; i < count; i++)
    {
        buffer[i] = Wire.read();
    }

    return true;
}

bool DS1307Provider::writeRegisters(uint8_t reg, const uint8_t *buffer, uint8_t count)
{
    Wire.beginTransmission(kI2cAddress);
    Wire.write(reg);
    for (uint8_t i = 0; i < count; i++)
    {
        Wire.write(buffer[i]);
    }
    return Wire.endTransmission() == 0;
}
