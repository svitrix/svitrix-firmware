#pragma once

/**
 * @file AppRegistry.h
 * @brief Native app name registry and app list serialization.
 *
 * Extracted from: DisplayManager.cpp (getNativeAppByName, getAppsAsJson).
 * Used by: DisplayManager (app switching), MQTTManager (app list API).
 * Tests: test/test_native/test_app_registry/
 *
 * The firmware has 5 built-in "native" apps (Time, Date, Temperature,
 * Humidity, Battery) that are always available. Custom apps are loaded
 * via MQTT. This service provides:
 *   - the list of native app names
 *   - name validation (is this a native app?)
 *   - JSON serialization of the current app list with indices
 */

#include <vector>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * Get the list of built-in native app names.
 *
 * @return Vector of names: {"Time", "Date", "Temperature", "Humidity", "Battery"}.
 */
std::vector<String> getNativeAppNames();

/**
 * Check if the given name matches a built-in native app.
 *
 * Case-sensitive comparison (matches firmware behavior).
 *
 * @param name App name to check.
 * @return true if name is one of the 5 native apps.
 */
bool isNativeApp(const String& name);

/**
 * Serialize an app name list to a JSON object string.
 *
 * Maps each name to its index: ["Time","Date","MyApp"] → {"Time":0,"Date":1,"MyApp":2}
 * Uses manual String building (no ArduinoJson dependency).
 *
 * @param appNames Vector of app names in display order.
 * @return JSON object string. Empty list returns "{}".
 */
String serializeAppList(const std::vector<String>& appNames);

