
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
#include "DisplayManager.h"
#include "PeripheryManager.h"
#include "MQTTManager.h"
#include "ServerManager.h"
#include "MenuManager.h"
#include "Globals.h"
#include "UpdateManager.h"
#include "PowerManager.h"
#ifdef ENABLE_GAMES
#include "Games/GameManager.h"
#endif
#include "timer.h"
#include <cassert>

TaskHandle_t taskHandle = nullptr;
volatile bool StopTask = false;
bool stopBoot;

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

    // Wire up IDisplay interfaces (Phase 8, updated Phase 11: renderer + notifier split)
    UpdateManager.setDisplay(&DisplayManager.getRenderer());
#ifdef ENABLE_GAMES
    GameManager.setDisplay(&DisplayManager.getRenderer());
    SlotMachine.setDisplay(&DisplayManager.getRenderer());
    SvitrixSays.setDisplay(&DisplayManager.getRenderer());
#endif
    MenuManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager);
    ServerManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager, &DisplayManager.getNotifier());
    MQTTManager.setDisplay(&DisplayManager, &DisplayManager, &DisplayManager.getNotifier());

    // Wire up ISound, IPower, IUpdater interfaces (Phase 9.3)
    ServerManager.setServices(&PeripheryManager, &PowerManager, &UpdateManager);
    MQTTManager.setServices(&PeripheryManager, &PowerManager, &UpdateManager, &PeripheryManager);
    MenuManager.setServices(&PeripheryManager, &UpdateManager);

    // Verify all interface wiring is complete (Phase 10)
    assert(UpdateManager.hasDisplay());
#ifdef ENABLE_GAMES
    assert(GameManager.hasDisplay());
    assert(SlotMachine.hasDisplay());
    assert(SvitrixSays.hasDisplay());
#endif
    assert(MenuManager.hasDisplay());
    assert(ServerManager.hasDisplay());
    assert(MQTTManager.hasDisplay());
    assert(ServerManager.hasServices());
    assert(MQTTManager.hasServices());
    assert(MenuManager.hasServices());

    ServerManager.loadSettings();
    DisplayManager.setup();
    DisplayManager.HSVtext(9, 6, VERSION, true, 0);
    delay(500);
    xTaskCreatePinnedToCore(BootAnimation, "Task", 10000, NULL, 1, &taskHandle, 0);
    ServerManager.setup();
    if (ServerManager.isConnected)
    {
        // timer_init();
        DisplayManager.loadNativeApps();
        DisplayManager.loadCustomApps();
        UpdateManager.setup();
        DisplayManager.startArtnet();
        stopBootAnimation();
        float x = 4;
        String textForDisplay = "SVITRIX   " + ServerManager.myIP.toString();

        if (systemConfig.webPort != 80)
        {
            textForDisplay += ":" + String(systemConfig.webPort);
        }

        int textLength = textForDisplay.length() * 4;
        while (x >= -textLength)
        {
            DisplayManager.HSVtext(x, 6, textForDisplay.c_str(), true, 0);
            x -= 0.18;
        }


        if (mqttConfig.host != "")
        {
            DisplayManager.HSVtext(4, 6, "MQTT...", true, 0);
            MQTTManager.setup();
            MQTTManager.tick();
        }
    }
    else
    {
        systemConfig.apMode = true;
        stopBootAnimation();
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
    if (ServerManager.isConnected)
    {
        MQTTManager.tick();
    }
}
