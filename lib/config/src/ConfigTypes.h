#pragma once

#include <Arduino.h>
#include "LayoutEngine.h"

struct AuthConfig {
    String user;
    String pass;
};

struct NetworkConfig {
    bool isStatic;
    String ip;
    String gateway;
    String subnet;
    String primaryDns;
    String secondaryDns;
};

struct BatteryConfig {
    uint8_t percent;
    uint16_t raw;
    uint16_t minRaw;
    uint16_t maxRaw;
};

struct HaConfig {
    bool discovery;
    String prefix;
};

struct MqttConfig {
    String host;
    uint16_t port;
    String user;
    String pass;
    String prefix;
};

enum TempSensorType : uint8_t {
    TEMP_SENSOR_TYPE_NONE    = 0,
    TEMP_SENSOR_TYPE_BME280  = 1,
    TEMP_SENSOR_TYPE_HTU21DF = 2,
    TEMP_SENSOR_TYPE_BMP280  = 3,
    TEMP_SENSOR_TYPE_SHT31   = 4,
};

struct SensorConfig {
    float currentTemp;
    float currentHum;
    float currentLux;
    uint16_t ldrRaw;
    uint8_t tempSensorType;
    bool sensorReading;
    bool sensorsStable;
    float tempOffset;
    float humOffset;
};

struct DisplayConfig {
    int matrixLayout;
    uint8_t matrixFps;
    bool matrixOff;
    bool mirrorDisplay;
    bool rotateScreen;
    bool uppercaseLetters;
    int backgroundEffect;
};

struct BrightnessConfig {
    int brightness;
    int brightnessPercent;
    bool autoBrightness;
    uint8_t minBrightness;
    uint8_t maxBrightness;
    float ldrGamma;
    float ldrFactor;
    bool ldrOnGround;
};

struct ColorConfig {
    uint32_t textColor;
    uint32_t timeColor;
    uint32_t dateColor;
    uint32_t batColor;
    uint32_t tempColor;
    uint32_t humColor;
    uint32_t wdcActive;
    uint32_t wdcInactive;
    uint32_t calendarHeaderColor;
    uint32_t calendarTextColor;
    uint32_t calendarBodyColor;
};

struct TimeConfig {
    String timeFormat;
    String dateFormat;
    uint8_t timeMode;
    bool startOnMonday;
    String ntpServer;
    String ntpTz;
    bool isCelsius;
    int tempDecimalPlaces;
};

struct AppConfig {
    bool showTime;
    bool showDate;
    bool showBat;
    bool showTemp;
    bool showHum;
    bool showWeekday;
    bool autoTransition;
    int8_t transEffect;
    int timePerTransition;
    long timePerApp;
    uint8_t scrollSpeed;
    IconLayout nativeIconLayout;
    bool blockNavigation;
    bool nightMode;
    uint16_t nightStart;      // 21:00 in minutes from midnight
    uint16_t nightEnd;        // 06:00 in minutes from midnight
    uint8_t nightBrightness;
    uint32_t nightColor;      // default: red (0xFF0000)
    bool nightBlockTransition; // disable auto-transition in night mode
};

struct AudioConfig {
    bool soundActive;
    uint8_t soundVolume;
    String bootSound;
};

struct SystemConfig {
    bool debugMode;
    uint32_t apTimeout;
    int webPort;
    String hostname;
    bool updateCheck;
    long statsInterval;
    bool newyear;
    bool swapButtons;
    String buttonCallback;
    String deviceId;
    bool updateAvailable;
    bool apMode;
    String updateVersionUrl;
    String updateFirmwareUrl;
};
