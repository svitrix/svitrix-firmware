#include <PowerManager.h>
#include <ArduinoJson.h>
#include "Globals.h"

static constexpr uint64_t kUsToSecFactor = 1000000ULL;
static constexpr gpio_num_t kWakeupPin = GPIO_NUM_27;

PowerManager_& PowerManager_::getInstance()
{
    static PowerManager_ instance;
    return instance;
}

PowerManager_& PowerManager = PowerManager_::getInstance();

void PowerManager_::sleepParser(const char *json)
{
    StaticJsonDocument<128> doc;
    if (deserializeJson(doc, json))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Failed to parse json"));
        return;
    }

    if (doc.containsKey("sleep"))
        sleep(doc["sleep"].as<uint64_t>());
}

void PowerManager_::sleep(uint64_t seconds)
{
    esp_sleep_enable_ext0_wakeup(kWakeupPin, 0);
    esp_sleep_enable_timer_wakeup(seconds * kUsToSecFactor);
    DEBUG_PRINTLN(F("Going to sleep..."));
    esp_deep_sleep_start();
}
