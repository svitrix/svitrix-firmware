
/*
 ***************************************************************************
 *                                                                         *
 *   SVITRIX, a custom firmware for the Ulanzi clock                  *
 *                                                                         *
 *   Copyright (C) 2024-2026  Svitrix                                      *
 *                                                                         *
 *   This work is licensed under a                                         *
 *   Creative Commons Attribution-NonCommercial-ShareAlike                 *
 *   4.0 International License.                                            *
 *                                                                         *
 *   More information:                                                     *
 *   https://github.com/svitrix/svitrix-firmware/blob/main/LICENSE.md *
 *                                                                         *
 *   This firmware is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 *                                                                         *
 ***************************************************************************/

#include <Arduino.h>
#include <esp_system.h>
#include "DisplayManager.h"
#include "PeripheryManager.h"
#include "MQTTManager.h"
#include "ServerManager.h"
#include "MenuManager.h"
#include "Globals.h"
#include "TextUtils.h"
#include "SvitrixFont.h"
#include "UpdateManager.h"
#include "PowerManager.h"
#include "DataFetcher/DataFetcher.h"
#include "timer.h"
#include "RtcTimeProvider.h"
#include "PeripheryManager/DS1307Provider.h"
#include "AlarmManager/AlarmManager.h"
#include "policies/NightModePolicy.h"
#include "Apps/Apps.h"
#include "ResetReason.h"
#include <cassert>

TaskHandle_t taskHandle = nullptr;
volatile bool StopTask = false;
bool stopBoot;
static DS1307Provider *rtcInstance = nullptr;
static bool rtcSynced = false;

void BootAnimation(void *parameter)
{
    const TickType_t xDelay = 1 / portTICK_PERIOD_MS;
    while (true)
    {
        if (StopTask)
        {
            break;
        }
        DisplayManager.HSVtext(4, 6, "SVITRIX", true, 0);
        vTaskDelay(xDelay);
    }
    taskHandle = nullptr;
    vTaskDelete(NULL);
}

/// Wait for boot animation task to fully stop before writing to leds[].
static void stopBootAnimation()
{
    StopTask = true;
    // Give the FreeRTOS task time to exit and release leds[]
    while (taskHandle != nullptr)
    {
        vTaskDelay(1);
    }
    // Extra safety: one tick for task cleanup
    vTaskDelay(2);
}

void setup()
{
    pinMode(15, OUTPUT);
    digitalWrite(15, LOW);
    delay(2000);
    Serial.begin(115200);
    // Capture last reset reason before anything else can mutate state.
    // The cause is preserved by hardware across reboots; cheap to query.
    lastResetReason = resetReasonToString(static_cast<uint8_t>(esp_reset_reason()));
    DEBUG_PRINTF("Boot. Last reset: %s", lastResetReason.c_str());
    setTextFont(&SvitrixFont);
    loadSettings();
    PeripheryManager.setup();

    // Wire up interfaces (Phase 5: break circular dependencies)
    PeripheryManager.addButtonHandler(&DisplayManager);
    PeripheryManager.addButtonHandler(&MenuManager);
    PeripheryManager.addButtonReporter(&MQTTManager);
    PeripheryManager.addButtonReporter(&ServerManager);
    PeripheryManager.setOnPowerToggle([](bool s)
                                      { DisplayManager.setPower(s); });
    PeripheryManager.setOnBrightnessChange([](int b)
                                           { DisplayManager.setBrightness(b); });
    PeripheryManager.setOnFactoryReset([]()
                                       { ServerManager.erase(); });
    PeripheryManager.setIsMenuActive([]()
                                     { return MenuManager.inMenu; });
    DisplayManager.setNotifier(&MQTTManager);
    DisplayManager.setPeriphery(&PeripheryManager);
    DisplayManager.setMenuActiveQuery([]()
                                      { return MenuManager.inMenu; });

    // RTC initialization (DS1307 at 0x68, detected after I2C init in PeripheryManager.setup())
    static DS1307Provider ds1307;
    static RtcTimeProvider rtcTimeProvider;
    if (ds1307.begin())
    {
        rtcTimeProvider.setRtc(&ds1307);
        rtcInstance = &ds1307;
        if (!ds1307.isRunning())
        {
            DEBUG_PRINTLN(F("RTC battery may be dead - clock halted"));
        }
    }

    // Display policies (scheduled/event-driven overrides for brightness,
    // text color, auto-transition). Order of registration = priority.
    // Statics so the objects outlive main(); DisplayManager only stores pointers.
    static NightModePolicy nightModePolicy(appConfig, rtcTimeProvider);
    DisplayManager.registerPolicy(&nightModePolicy);

    // Wire up IDisplay interfaces (Phase 8, updated Phase 11: renderer + notifier split)
    UpdateManager.setDisplay(&DisplayManager.getRenderer());
    MenuManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager);
    ServerManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
    MQTTManager.setDisplay(&DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
    setAppsNotifier(&MQTTManager);
    DataFetcher.setNavigation(&DisplayManager);

    // Wire up ISound, IPower, IUpdater interfaces (Phase 9.3)
    ServerManager.setServices(&PeripheryManager, &PowerManager, &UpdateManager);
    ServerManager.setMqttReconnectCallback([]() { MQTTManager.reconnect(); });
    MQTTManager.setServices(&PeripheryManager, &PowerManager, &UpdateManager, &PeripheryManager);
    MenuManager.setServices(&PeripheryManager, &UpdateManager);

    // Wire up AlarmManager (autonomous mode)
    AlarmManager.setSound(&PeripheryManager, &PeripheryManager);
    AlarmManager.setNotifier(&DisplayManager.getNotifier());
    AlarmManager.setTimeProvider(&rtcTimeProvider);
    MenuManager.setAlarmProvider(&AlarmManager); // on-device alarm menu

    // Verify all interface wiring is complete (Phase 10)
    assert(UpdateManager.hasDisplay());
    assert(MenuManager.hasDisplay());
    assert(ServerManager.hasDisplay());
    assert(MQTTManager.hasDisplay());
    assert(DataFetcher.hasNavigation());
    assert(ServerManager.hasServices());
    assert(MQTTManager.hasServices());
    assert(MenuManager.hasServices());
    assert(AlarmManager.hasServices());
    assert(MenuManager.hasAlarm());

    ServerManager.loadSettings();
    DisplayManager.setup();
    DisplayManager.HSVtext(9, 6, VERSION, true, 0);
    delay(2500);
    xTaskCreatePinnedToCore(BootAnimation, "Task", 10000, NULL, 1, &taskHandle, 0);
    ServerManager.setup();
    if (ServerManager.isConnected)
    {
        // timer_init();
        // Custom apps first so loadNativeApps() can merge them into the
        // saved appOrder (single source of truth for the unified app loop).
        DisplayManager.loadCustomApps();
        DisplayManager.loadNativeApps();
        // Re-apply settings now that apps are loaded (playlist needs app list)
        DisplayManager.applyAllSettings();
        UpdateManager.setup();
        DisplayManager.startArtnet();
        stopBootAnimation();
        float x = 4;
        String textForDisplay = "SVITRIX XE1E   " + ServerManager.myIP.toString();

        if (systemConfig.webPort != 80)
        {
            textForDisplay += ":" + String(systemConfig.webPort);
        }

        textForDisplay += "   " + systemConfig.hostname + ".local";

        int textLength = textForDisplay.length() * 4;
        while (x >= -textLength)
        {
            DisplayManager.HSVtext(x, 6, textForDisplay.c_str(), true, 0);
            x -= 0.4;  // Balanced scroll speed (~4 seconds)
        }
        delay(300);

        DEBUG_PRINTLN("Starting MQTT check...");
        if (mqttConfig.host != "")
        {
            DEBUG_PRINTF("MQTT host: %s\n", mqttConfig.host.c_str());
            DisplayManager.HSVtext(4, 6, "MQTT...", true, 0);
            MQTTManager.setup();
            MQTTManager.tick();
        }
        else
        {
            DEBUG_PRINTLN("MQTT not configured, skipping");
        }
        DEBUG_PRINTLN("Starting DataFetcher...");
        DataFetcher.setup();
        DEBUG_PRINTLN("Starting AlarmManager...");
        AlarmManager.setup();
        DEBUG_PRINTLN("Setup complete, entering loop");
    }
    else
    {
        systemConfig.apMode = true;
        stopBootAnimation();
        AlarmManager.setup(); // Alarms work in AP mode too
    }
    delay(200);
    DisplayManager.setBrightness(brightnessConfig.brightness);
}

void loop()
{
    timer_tick();
    ServerManager.tick();
    DisplayManager.tick();
    PeripheryManager.tick();
    AlarmManager.tick(time(nullptr)); // Alarms work even without WiFi
    if (ServerManager.isConnected)
    {
        MQTTManager.tick();
        DataFetcher.tick();

        // Sync NTP → RTC once after time becomes valid
        if (!rtcSynced && rtcInstance)
        {
            const struct tm *t = timer_localtime();
            if (t && t->tm_year >= 120)
            {
                time_t now = mktime(const_cast<struct tm *>(t));
                if (rtcInstance->setTime(now))
                {
                    rtcSynced = true;
                    DEBUG_PRINTLN(F("NTP time synced to RTC"));
                }
            }
        }
    }
}
