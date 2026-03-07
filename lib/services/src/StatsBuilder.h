#pragma once

/**
 * @file StatsBuilder.h
 * @brief Device stats JSON serialization — no hardware dependencies.
 *
 * Extracted from: DisplayManager.cpp (getStats).
 * Used by: DisplayManager (stats API endpoint), MQTTManager (stats publish).
 * Tests: test/test_native/test_stats_builder/
 *
 * Separates data collection (hardware reads, stays in DisplayManager) from
 * JSON serialization (pure logic, extracted here). The caller populates a
 * StatsData struct from globals/hardware, then calls buildStatsJson().
 *
 * Uses manual String building instead of ArduinoJson so the function
 * is testable in native tests without the ArduinoJson library.
 */

#include <cstdint>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * All device stats needed for the JSON stats endpoint.
 *
 * Fields match the JSON keys from Dictionary.h:
 *   bat, bat_raw, type, lux, ldr_raw, ram, bri, temp, hum, uptime,
 *   wifi_signal, messages, version, indicator1-3, app, uid, matrix, ip_address.
 */
struct StatsData
{
    uint8_t batteryPercent;     // "bat" — battery percentage [0..100]
    uint16_t batteryRaw;        // "bat_raw" — raw ADC value
    int lux;                    // "lux" — ambient light level
    uint16_t ldrRaw;            // "ldr_raw" — raw LDR ADC value
    uint32_t freeRam;           // "ram" — free heap + PSRAM bytes
    uint8_t brightness;         // "bri" — current display brightness
    bool hasSensor;             // if true, include temp/hum fields
    float temperature;          // "temp" — already rounded by caller
    uint8_t tempDecimalPlaces;  // decimal places for temperature formatting
    uint8_t humidity;           // "hum" — relative humidity percentage
    String uptime;              // "uptime" — formatted uptime string
    int8_t wifiSignal;          // "wifi_signal" — RSSI in dBm
    uint32_t receivedMessages;  // "messages" — total MQTT messages received
    String version;             // "version" — firmware version string
    bool indicator1;            // "indicator1" — indicator LED state
    bool indicator2;            // "indicator2"
    bool indicator3;            // "indicator3"
    String currentApp;          // "app" — currently active app name
    String uid;                 // "uid" — unique device identifier
    bool matrixOn;              // "matrix" — true if display is on
    String ipAddress;           // "ip_address" — device IP address
};

/**
 * Build a JSON string from device stats.
 *
 * Output format matches the original ArduinoJson-based getStats():
 *   {"bat":100,"bat_raw":665,"type":0,...}
 *
 * When hasSensor is false, "temp" and "hum" fields are omitted.
 *
 * @param data Populated stats struct.
 * @return Compact JSON string.
 */
String buildStatsJson(const StatsData& data);

