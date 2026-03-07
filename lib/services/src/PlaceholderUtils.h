/**
 * @file PlaceholderUtils.h
 * @brief Template placeholder substitution: {{key}} → value.
 *
 * Hardware-free utility for replacing {{key}} patterns in strings.
 * The value resolution is injected via a callback to keep this
 * module testable without MQTT or other hardware dependencies.
 */
#pragma once

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

#include <functional>

/// Replace all {{key}} placeholders in text using the provided resolver.
/// After each replacement, scanning continues from the end of the
/// inserted value (not the end of the original placeholder position),
/// ensuring subsequent placeholders are always found.
///
/// @param input    Input string with zero or more {{key}} patterns.
/// @param getValue Callback: given a key string, returns the replacement value.
/// @return New string with all placeholders resolved.
String replacePlaceholdersWith(const String &input,
                               const std::function<String(const String &)> &getValue);

