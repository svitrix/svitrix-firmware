#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "ConfigTypes.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTLN(x)        \
    {                           \
        Serial.print("[");      \
        Serial.print(millis()); \
        Serial.print("] [");    \
        Serial.print(__func__); \
        Serial.print("]: ");    \
        Serial.println(x);      \
    }
#define DEBUG_PRINTF(format, ...)             \
    {                                         \
        Serial.print("[");                    \
        Serial.print(millis());               \
        Serial.print("] [");                  \
        Serial.print(__func__);               \
        Serial.print("]: ");                  \
        Serial.printf(format, ##__VA_ARGS__); \
        Serial.println();                     \
    }
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(format, ...)
#endif

void formatSettings();

#ifndef VERSION
#define VERSION "dev"
#endif

// Config structs
extern MqttConfig mqttConfig;
extern NetworkConfig networkConfig;
extern HaConfig haConfig;
extern SensorConfig sensorConfig;
extern BatteryConfig batteryConfig;
extern AuthConfig authConfig;

extern DisplayConfig displayConfig;
extern BrightnessConfig brightnessConfig;
extern ColorConfig colorConfig;
extern TimeConfig timeConfig;
extern AppConfig appConfig;
extern AudioConfig audioConfig;
extern SystemConfig systemConfig;

constexpr double movementFactor = 0.5;

void loadSettings();
void saveSettings();
bool isNightModeActive();
