#pragma once

#include "ITimeProvider.h"
#include "IRtcProvider.h"
#include "timer.h"
#include <ctime>
#include <cassert>

/**
 * @file RtcTimeProvider.h
 * @brief ITimeProvider implementation that prefers RTC over system time.
 *
 * Priority:
 * 1. If system time is synced (tm_year >= 120), use system time (NTP-backed)
 * 2. Else if RTC is available and running, use RTC time
 * 3. Else return false (no valid time source)
 *
 * This allows NTP to take precedence when available while falling back to
 * RTC during boot or when WiFi is disconnected.
 */
class RtcTimeProvider : public ITimeProvider
{
  public:
    void setRtc(IRtcProvider *rtc)
    {
        assert(rtc != nullptr);
        rtc_ = rtc;
    }

    bool hasRtc() const
    {
        return rtc_ != nullptr;
    }

    bool now(struct tm& out) const override
    {
        const struct tm *sysTime = timer_localtime();
        if (sysTime && sysTime->tm_year >= 120)
        {
            out = *sysTime;
            return true;
        }

        if (rtc_ && rtc_->isRunning())
        {
            time_t epoch = rtc_->getTime();
            if (epoch > 0)
            {
                struct tm *rtcTime = localtime(&epoch);
                if (rtcTime && rtcTime->tm_year >= 120)
                {
                    out = *rtcTime;
                    return true;
                }
            }
        }

        return false;
    }

  private:
    IRtcProvider *rtc_ = nullptr;
};
