#pragma once

/**
 * @file ResetReason.h
 * @brief Map ESP32 reset reason codes to short human-readable tags.
 *
 * Pure function — no hardware dependency. The caller passes the numeric
 * code from `esp_reset_reason()` (cast to uint8_t) and gets back a short
 * stable tag suitable for logs, MQTT payloads, and HA template sensors.
 *
 * Keeping the parameter as `uint8_t` (rather than `esp_reset_reason_t`)
 * avoids pulling `esp_system.h` into the native test environment, so the
 * mapping is testable on host without an ESP toolchain.
 *
 * Usage:
 *     #include <esp_system.h>
 *     lastResetReason = resetReasonToString(static_cast<uint8_t>(esp_reset_reason()));
 *
 * Tests: test/test_native/test_reset_reason/
 */

#include <cstdint>

/// Tag returned for codes outside the known range, or for ESP_RST_UNKNOWN.
constexpr const char* kResetReasonUnknown = "unknown";

/**
 * Convert an ESP32 reset reason code to a stable short tag.
 *
 * The returned pointer is a static string literal — safe to assign to
 * Arduino `String`, embed in JSON, or log directly.
 *
 * Code mapping (matches `esp_reset_reason_t` from ESP-IDF v4+):
 *   - 0 ESP_RST_UNKNOWN     -> "unknown"
 *   - 1 ESP_RST_POWERON     -> "poweron"
 *   - 2 ESP_RST_EXT         -> "ext"
 *   - 3 ESP_RST_SW          -> "software"
 *   - 4 ESP_RST_PANIC       -> "panic"
 *   - 5 ESP_RST_INT_WDT     -> "int_wdt"
 *   - 6 ESP_RST_TASK_WDT    -> "task_wdt"
 *   - 7 ESP_RST_WDT         -> "wdt"
 *   - 8 ESP_RST_DEEPSLEEP   -> "deepsleep"
 *   - 9 ESP_RST_BROWNOUT    -> "brownout"
 *   - 10 ESP_RST_SDIO       -> "sdio"
 *   - any other value       -> "unknown"
 *
 * @param code Reset reason numeric code from `esp_reset_reason()`.
 * @return Stable tag (never nullptr).
 */
const char* resetReasonToString(uint8_t code);
