#pragma once

/**
 * @file MessageRouter.h
 * @brief MQTT topic routing and command parsing — no WiFi/network dependencies.
 *
 * Extracted from: MQTTManager.cpp (processMqttMessage, onMqttConnected).
 * Used by: MQTTManager (message dispatch), ServerManager (HTTP API reuse).
 * Tests: test/test_native/test_message_router/
 *
 * The firmware handles ~20 MQTT topic types. This service provides:
 *   - MqttCommandType enum for all known commands
 *   - routeTopic(): topic string → command type (pure string matching)
 *   - extractCustomTopicName(): extract app name from /custom/... wildcard
 *   - isJsonPayload(): validate JSON object braces
 *   - getSubscriptionTopics(): single source of truth for topic subscriptions
 */

#include <vector>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * All MQTT command types handled by the firmware.
 *
 * Each value corresponds to a topic suffix under the MQTT prefix:
 *   CMD_NOTIFY        → /notify
 *   CMD_NOTIFY_DISMISS → /notify/dismiss
 *   CMD_CUSTOM        → /custom/# (wildcard)
 *   CMD_UNKNOWN       → unrecognized topic
 */
enum MqttCommandType
{
    CMD_UNKNOWN,
    CMD_NOTIFY,
    CMD_NOTIFY_DISMISS,
    CMD_DO_UPDATE,
    CMD_APPS,
    CMD_SWITCH,
    CMD_SEND_SCREEN,
    CMD_SETTINGS,
    CMD_R2D2,
    CMD_NEXT_APP,
    CMD_PREVIOUS_APP,
    CMD_RTTTL,
    CMD_POWER,
    CMD_SLEEP,
    CMD_INDICATOR1,
    CMD_INDICATOR2,
    CMD_INDICATOR3,
    CMD_MOODLIGHT,
    CMD_REBOOT,
    CMD_SOUND,
    CMD_CUSTOM,
};

/**
 * Route an MQTT topic string to a command type.
 *
 * Strips the prefix before matching against known suffixes.
 * The /custom/... wildcard matches any topic starting with prefix + "/custom/".
 *
 * @param topic  Full MQTT topic string (e.g. "awtrix_abc123/notify").
 * @param prefix Device MQTT prefix (e.g. "awtrix_abc123").
 * @return Matching MqttCommandType, or CMD_UNKNOWN if no match.
 */
MqttCommandType routeTopic(const String& topic, const String& prefix);

/**
 * Extract the app name from a /custom/... topic.
 *
 * Input:  topic="awtrix_abc/custom/MyApp", prefix="awtrix_abc"
 * Output: "MyApp"
 *
 * Handles nested paths: "prefix/custom/folder/app" → "folder/app".
 *
 * @param topic  Full MQTT topic string.
 * @param prefix Device MQTT prefix.
 * @return App name after "/custom/", or "" if topic doesn't match.
 */
String extractCustomTopicName(const String& topic, const String& prefix);

/**
 * Check if a payload string looks like a JSON object.
 *
 * Simple brace check: starts with '{' and ends with '}'.
 * Used by /notify handler to reject malformed payloads before parsing.
 *
 * @param payload MQTT message payload.
 * @return true if payload is wrapped in curly braces.
 */
bool isJsonPayload(const String& payload);

/**
 * Get all MQTT subscription topic suffixes.
 *
 * Returns the canonical list of topic suffixes (without prefix) that the
 * firmware subscribes to. Single source of truth — replaces the hardcoded
 * array in onMqttConnected().
 *
 * @return Vector of suffix strings, each starting with "/".
 */
std::vector<String> getSubscriptionTopics();

