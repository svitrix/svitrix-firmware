/**
 * @file MQTTManager_internal.h
 * @brief Shared internal state for MQTTManager modules.
 *
 * Declares extern references to globals defined in MQTTManager.cpp
 * and forward-declares free helper functions from the split modules.
 *
 * Only included by MQTTManager_*.cpp files — NOT part of the public API.
 */
#pragma once

#include "MQTTManager.h"
#include "Globals.h"
#include <ArduinoHA.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Dictionary.h"
#include "IDisplayControl.h"
#include "IDisplayNavigation.h"
#include "IDisplayNotifier.h"
#include "ISound.h"
#include "IPower.h"
#include "IUpdater.h"
#include "IPeripheryProvider.h"
#include "ServerManager.h"
#include "MessageRouter.h"
#include "HADiscovery.h"

// ── MQTT transport (defined in MQTTManager.cpp) ─────────────────────

extern WiFiClient espClient; ///< TCP socket for the MQTT connection
extern HADevice device;      ///< ArduinoHA device descriptor (name, MAC, version)
extern HAMqtt mqtt;          ///< ArduinoHA MQTT client (wraps PubSubClient)

// ── HA entity pointers (defined in MQTTManager.cpp) ─────────────────
// Created in setup() when HA discovery is enabled; nullptr otherwise.

extern HALight *Matrix;          ///< Main matrix light (brightness + RGB)
extern HALight *Indikator1;      ///< Top-right indicator light (RGB)
extern HALight *Indikator2;      ///< Middle-right indicator light (RGB)
extern HALight *Indikator3;      ///< Bottom-right indicator light (RGB)
extern HASelect *BriMode;        ///< Brightness mode select (Manual / Auto)
extern HASelect *transEffect;    ///< Transition effect select
extern HAButton *dismiss;        ///< Dismiss notification button
extern HAButton *nextApp;        ///< Next app button
extern HAButton *prevApp;        ///< Previous app button
extern HAButton *doUpdate;       ///< Start firmware update button
extern HASwitch *transition;     ///< Auto-transition on/off toggle
extern HASensor *battery;        ///< Battery percentage sensor (ULANZI only)
extern HASensor *temperature;    ///< Temperature sensor (°C)
extern HASensor *humidity;       ///< Humidity sensor (%)
extern HASensor *illuminance;    ///< Ambient light sensor (lx)
extern HASensor *uptime;         ///< Device uptime sensor (seconds)
extern HASensor *strength;       ///< WiFi signal strength sensor (dB)
extern HASensor *version;        ///< Firmware version sensor
extern HASensor *ram;            ///< Free RAM sensor (bytes)
extern HASensor *curApp;         ///< Current app name sensor
extern HASensor *myOwnID;        ///< Device MQTT prefix sensor
extern HASensor *ipAddr;         ///< Device IP address sensor
extern HABinarySensor *btnleft;  ///< Left hardware button state
extern HABinarySensor *btnmid;   ///< Middle hardware button state
extern HABinarySensor *btnright; ///< Right hardware button state

// ── HA entity ID buffers (defined in MQTTManager.cpp) ───────────────
// Each buffer holds a unique HA entity ID built from MAC + suffix.

extern char matID[40], ind1ID[40], ind2ID[40], ind3ID[40];
extern char briID[40], btnAID[40], btnBID[40], btnCID[40];
extern char appID[40], tempID[40], humID[40], luxID[40];
extern char verID[40], ramID[40], upID[40], sigID[40];
extern char btnLID[40], btnMID[40], btnRID[40];
extern char transID[40], doUpdateID[40], batID[40];
extern char myID[40], sSpeed[40], effectID[40], ipAddrID[40];

// ── Other shared state (defined in MQTTManager.cpp) ─────────────────

extern long receivedMessages_;                ///< Total MQTT messages received
extern bool connected;                        ///< true after first successful MQTT connection
extern unsigned long previousMillis_Stats;    ///< Timestamp of last stats publish
extern std::map<String, String> mqttValues;   ///< Cached values for subscribed external topics
extern std::vector<String> topicsToSubscribe; ///< Topics queued for subscription on next connect

// ── Free functions (defined in MQTTManager_Messages.cpp) ────────────

/// Route an incoming MQTT message to the appropriate manager.
void processMqttMessage(const String& strTopic, const String& payloadCopy);

/// Raw MQTT message callback: converts byte payload to String and dispatches.
void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length);

// ── Free functions (defined in MQTTManager_Callbacks.cpp) ───────────

/// HA button press dispatcher (dismiss, update, next, prev).
void onButtonCommand(HAButton *sender);
/// HA auto-transition switch toggle handler.
void onSwitchCommand(bool state, HASwitch *sender);
/// HA select change handler (brightness mode, transition effect).
void onSelectCommand(int8_t index, HASelect *sender);
/// HA RGB color change handler for matrix and indicators.
void onRGBColorCommand(HALight::RGBColor color, HALight *sender);
/// HA on/off state change handler for matrix and indicators.
void onStateCommand(bool state, HALight *sender);
/// HA brightness slider change handler.
void onBrightnessCommand(uint8_t brightness, HALight *sender);
/// HA scroll speed number input handler.
void onNumberCommand(HANumeric number, HANumber *sender);

// ── Display interfaces (defined in MQTTManager.cpp, set via setDisplay) ──

extern IDisplayControl *dmControl_;
extern IDisplayNavigation *dmNav_;
extern IDisplayNotifier *dmNotify_;

// ── Service interfaces (defined in MQTTManager.cpp, set via setServices) ──

extern ISound *mqttSound_;
extern IPower *mqttPower_;
extern IUpdater *mqttUpdater_;
extern IPeripheryProvider *mqttPeriphery_;

// ── Free functions (defined in MQTTManager.cpp) ─────────────────────

/// Fired when MQTT connection is established; subscribes to topics and publishes initial state.
void onMqttConnected();
/// Initialize the MQTT client with callbacks, credentials, and last-will.
void connect();
