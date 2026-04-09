#pragma once
#include <Arduino.h>

/// Configuration for a single external data source.
struct DataSourceConfig
{
    String name;          // Unique ID, becomes the custom app name (e.g., "btc")
    String url;           // Full URL with query params
    String jsonPath;      // Dot-notation path to extract value (e.g., "bitcoin.usd")
    String displayFormat; // printf-style format (e.g., "$%.0f") or empty for raw value
    String icon;          // Icon name from LittleFS (empty = no icon)
    String textColor;     // Hex color "#RRGGBB" or empty for default
    uint32_t interval;    // Polling interval in seconds (minimum 60, default 900 = 15 min)

    static constexpr uint32_t MIN_INTERVAL = 60;
    static constexpr uint32_t DEFAULT_INTERVAL = 900;
    static constexpr size_t MAX_SOURCES = 8;
};
