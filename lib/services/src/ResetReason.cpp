#include "ResetReason.h"

const char* resetReasonToString(uint8_t code)
{
    switch (code)
    {
        case 0:  return kResetReasonUnknown;
        case 1:  return "poweron";
        case 2:  return "ext";
        case 3:  return "software";
        case 4:  return "panic";
        case 5:  return "int_wdt";
        case 6:  return "task_wdt";
        case 7:  return "wdt";
        case 8:  return "deepsleep";
        case 9:  return "brownout";
        case 10: return "sdio";
        default: return kResetReasonUnknown;
    }
}
