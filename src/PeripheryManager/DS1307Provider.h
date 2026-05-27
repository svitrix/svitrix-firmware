#pragma once

#include "IRtcProvider.h"
#include <Wire.h>

/**
 * @file DS1307Provider.h
 * @brief DS1307 RTC driver using direct I2C communication.
 *
 * The DS1307 is a low-power BCD real-time clock with 56 bytes of NV SRAM.
 * Address: 0x68. Registers 0x00-0x06 hold time/date in BCD format.
 * Bit 7 of register 0x00 is the Clock Halt (CH) bit: 1 = oscillator stopped.
 */
class DS1307Provider : public IRtcProvider
{
public:
    DS1307Provider() = default;
    ~DS1307Provider() override = default;

    DS1307Provider(const DS1307Provider&) = delete;
    DS1307Provider& operator=(const DS1307Provider&) = delete;

    bool begin() override;
    bool isRunning() override;
    time_t getTime() override;
    bool setTime(time_t epoch) override;

private:
    static constexpr uint8_t kI2cAddress = 0x68;
    static constexpr uint8_t kRegSeconds = 0x00;
    static constexpr uint8_t kRegMinutes = 0x01;
    static constexpr uint8_t kRegHours = 0x02;
    static constexpr uint8_t kRegDay = 0x03;
    static constexpr uint8_t kRegDate = 0x04;
    static constexpr uint8_t kRegMonth = 0x05;
    static constexpr uint8_t kRegYear = 0x06;
    static constexpr uint8_t kRegControl = 0x07;

    static constexpr uint8_t kClockHaltBit = 0x80;

    bool detected_ = false;

    uint8_t bcdToDec(uint8_t val) const { return (val >> 4) * 10 + (val & 0x0F); }
    uint8_t decToBcd(uint8_t val) const { return ((val / 10) << 4) | (val % 10); }

    bool readRegisters(uint8_t reg, uint8_t* buffer, uint8_t count);
    bool writeRegisters(uint8_t reg, const uint8_t* buffer, uint8_t count);
};
