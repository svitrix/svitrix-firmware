#pragma once

#include <Arduino.h>
#include "IPower.h"

class PowerManager_ : public IPower
{
  public:
    static PowerManager_& getInstance();

    PowerManager_(const PowerManager_&) = delete;
    PowerManager_& operator=(const PowerManager_&) = delete;
    PowerManager_(PowerManager_&&) = delete;
    PowerManager_& operator=(PowerManager_&&) = delete;

    void sleepParser(const char *json) override;
    void sleep(uint64_t seconds) override;

  private:
    PowerManager_() = default;
};

extern PowerManager_& PowerManager;
