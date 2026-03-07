#pragma once

/**
 * @file HADiscovery.h
 * @brief Home Assistant entity descriptors and ID generation.
 *
 * Extracted from: MQTTManager.cpp (setup).
 * Used by: MQTTManager (HA auto-discovery entity creation).
 * Tests: test/test_native/test_ha_discovery/
 *
 * Provides a registry of all HA entities the device exposes,
 * separated from ArduinoHA object creation and MQTT transport.
 * Each descriptor holds the entity's ID template, display name,
 * icon, device class, and unit — enough to configure any
 * ArduinoHA object without referencing Dictionary.h directly.
 *
 * Self-contained: no Arduino.h or Dictionary.h dependency.
 */

#include <cstdint>
#include <cstddef>

/// Describes a single HA entity (light, sensor, button, switch, etc.).
struct HAEntityDescriptor
{
    const char *idTemplate;  ///< sprintf format with one %s for MAC: "%s_mat"
    const char *name;        ///< Display name: "Matrix"
    const char *icon;        ///< MDI icon: "mdi:clock-digital" (nullptr = not set)
    const char *deviceClass; ///< HA device class: "temperature" (nullptr = not set)
    const char *unit;        ///< Unit of measurement: "°C" (nullptr = not set)
};

/// Describes an HA select entity with additional options field.
struct HASelectDescriptor
{
    HAEntityDescriptor base;
    const char *options; ///< Semicolon-separated options: "Manual;Auto"
};

// --- Lights ---

/// Matrix light descriptor (supports brightness + RGB).
HAEntityDescriptor getMatrixLightDescriptor();

/// Three indicator light descriptors (RGB only).
/// @param[out] count Set to number of descriptors (always 3).
/// @return Pointer to static array of indicator descriptors.
const HAEntityDescriptor *getIndicatorLightDescriptors(size_t &count);

// --- Selects ---

/// Brightness mode and transition effect selects.
/// @param[out] count Set to number of descriptors (always 2).
/// @return Pointer to static array: [0] = brightness mode, [1] = transition effect.
const HASelectDescriptor *getSelectDescriptors(size_t &count);

// --- Buttons ---

/// Action buttons: dismiss, start update, next app, previous app.
/// @param[out] count Set to number of descriptors (always 4).
/// @return Pointer to static array of button descriptors.
const HAEntityDescriptor *getButtonDescriptors(size_t &count);

// --- Switch ---

/// Auto-transition toggle switch descriptor.
HAEntityDescriptor getTransitionSwitchDescriptor();

// --- Sensors ---

/// All HASensor entities. Battery is always last in the array.
/// @param[out] count Set to 11 (with battery) or 10 (without).
/// @param includeBattery true to include the battery sensor.
/// @return Pointer to static array of sensor descriptors.
const HAEntityDescriptor *getSensorDescriptors(size_t &count, bool includeBattery);

// --- Binary sensors ---

/// Three hardware button binary sensors (left, select, right).
/// @param[out] count Set to number of descriptors (always 3).
/// @return Pointer to static array of binary sensor descriptors.
const HAEntityDescriptor *getBinarySensorDescriptors(size_t &count);

// --- Utilities ---

/// Build an entity ID by formatting idTemplate with macStr.
/// Example: buildEntityId("%s_mat", "abc123", buf, 40) -> "abc123_mat"
void buildEntityId(const char *idTemplate, const char *macStr,
                   char *outBuf, size_t bufSize);

/// Build a configuration URL from an IP address.
/// Example: buildConfigUrl({192,168,1,42}, buf, 32) -> "http://192.168.1.42"
void buildConfigUrl(const uint8_t ip[4], char *outBuf, size_t bufSize);

/// @return "Blueforcer"
const char *getDeviceManufacturer();

/// @return "SVITRIX"
const char *getDeviceModel();

/// Total number of HA entities the device registers.
/// @param includeBattery true to count the battery sensor.
/// @return 24 (without battery) or 25 (with battery).
size_t getTotalEntityCount(bool includeBattery);

