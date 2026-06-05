#pragma once

#include <Arduino.h>
#include "LayoutEngine.h"

struct AuthConfig {
    String user;
    String pass;
};

struct WifiNetwork {
    String ssid;
    String password;
};

struct WifiConfig {
    WifiNetwork networks[3];
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
    bool showAlarms;
    bool autoTransition;
    int8_t transEffect;
    int timePerTransition;
    long timePerApp;
    uint8_t scrollSpeed;
    uint16_t timeDuration;    // Clock display duration (1-300s)
    uint16_t dateDuration;    // Date display duration (1-60s)
    uint16_t tempDuration;    // Temperature display duration (1-60s)
    uint16_t humDuration;     // Humidity display duration (1-60s)
    uint16_t batDuration;     // Battery display duration (1-60s)
    IconLayout nativeIconLayout;
    bool blockNavigation;
    bool nightMode;
    uint16_t nightStart;      // 21:00 in minutes from midnight
    uint16_t nightEnd;        // 06:00 in minutes from midnight
    uint8_t nightBrightness;
    uint32_t nightColor;      // default: red (0xFF0000)
    bool nightBlockTransition; // disable auto-transition in night mode
    String appOrder;          // persisted JSON array of app names (unified app loop order)
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

enum WeatherLocationType : uint8_t {
    WEATHER_LOC_CITY = 0,
    WEATHER_LOC_COORDS = 1,
    WEATHER_LOC_AUTO_IP = 2,
    WEATHER_LOC_STATION = 3
};

struct WeatherConfig {
    String apiKey;
    WeatherLocationType locationType;
    String city;
    float latitude;
    float longitude;
    String stationId;         // PWS station ID (e.g. "pws:KMAHANOV10")
    uint16_t updateInterval;  // minutes (10, 15, 30, 60)
    bool showOutdoorTemp;
    bool showOutdoorHumidity;
    bool showPressure;
    bool showAirQuality;
    bool showIndoorTemp;
    bool showIndoorHumidity;
    bool showUV;
    // Display settings for weather apps
    uint32_t outdoorTempColor;
    uint32_t outdoorHumColor;
    uint32_t pressureColor;
    uint32_t aqiColor;
    uint32_t uvColor;
    uint8_t outdoorTempDuration;  // seconds
    uint8_t outdoorHumDuration;
    uint8_t pressureDuration;
    uint8_t aqiDuration;
    uint8_t uvDuration;
};

struct WeatherData {
    float outdoorTemp;
    float outdoorHumidity;
    float pressure;           // mb/hPa
    int aqi;                  // US EPA index (1-6)
    float uv;                 // UV index (0-11+)
    String condition;         // "sunny", "cloudy", etc.
    int conditionCode;        // WeatherAPI condition code
    unsigned long lastUpdate; // millis() of last update
    bool valid;               // data available
};

enum PlaylistItemType : uint8_t {
    PLAYLIST_ITEM_APP = 0,
    PLAYLIST_ITEM_EFFECT = 1,
};

struct PlaylistItem {
    PlaylistItemType type;    // app or effect
    String name;              // app name or effect name
    uint16_t duration;        // seconds (0 = use app's default duration)
};

struct PlaylistConfig {
    bool enabled;             // false = use simple appOrder mode
    String items;             // JSON array of playlist items
};

// Unified rotation config - replaces both appOrder and playlist
// Each item: {id, type, name, enabled, duration, color, icon}
enum RotationItemType : uint8_t {
    ROTATION_ITEM_APP = 0,
    ROTATION_ITEM_EFFECT = 1,
};

struct RotationConfig {
    String items;             // JSON array: [{id, type, name, enabled, duration, color, icon}, ...]
};
