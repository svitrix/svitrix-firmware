/**
 * @file MQTTManager_Messages.cpp
 * @brief Incoming MQTT message reception and command dispatch.
 *
 * Contains the raw message adapter (onMqttMessage) and the main
 * command router (processMqttMessage) that dispatches to various
 * managers based on the MqttCommandType from MessageRouter.
 */
#include "MQTTManager_internal.h"

// ── Raw message adapter ─────────────────────────────────────────────

/// Convert raw MQTT payload bytes to a null-terminated String
/// and forward to processMqttMessage for command dispatch.
/// @param topic   Full MQTT topic the message was received on.
/// @param payload Raw byte payload from the broker (not null-terminated).
/// @param length  Number of bytes in payload.
void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length)
{
    if (systemConfig.debugMode)
        DEBUG_PRINTF("MQTT message received at topic %s", topic);

    char *payloadCopy = new char[length + 1];
    memcpy(payloadCopy, payload, length);
    payloadCopy[length] = '\0';

    processMqttMessage(String(topic), String(payloadCopy));

    delete[] payloadCopy;
}

// ── Command dispatch ────────────────────────────────────────────────

/// Route an incoming MQTT message to the appropriate manager based on topic.
/// Uses routeTopic() from MessageRouter to map the topic string to a
/// MqttCommandType, then dispatches to the matching manager (DisplayManager,
/// PeripheryManager, UpdateManager, PowerManager, etc.).
/// Unknown topics that exist in mqttValues are stored for external subscribers.
/// @param strTopic    Full MQTT topic string.
/// @param payloadCopy Message payload as a String.
void processMqttMessage(const String& strTopic, const String& payloadCopy)
{
    if (systemConfig.debugMode)
    {
        DEBUG_PRINTF("Processing MQTT message for topic %s", strTopic.c_str());
        DEBUG_PRINTF("Payload: %s", payloadCopy.c_str());
    }

    ++receivedMessages_;

    MqttCommandType cmd = routeTopic(strTopic, mqttConfig.prefix);

    switch (cmd)
    {
    case CMD_NOTIFY:
        if (!isJsonPayload(payloadCopy))
            return;
        dmNotify_->generateNotification(0, payloadCopy.c_str());
        return;

    case CMD_NOTIFY_DISMISS:
        dmNotify_->dismissNotify();
        return;

    case CMD_DO_UPDATE:
        if (mqttUpdater_->checkUpdate(true))
            mqttUpdater_->updateFirmware();
        return;

    case CMD_APPS:
        dmNav_->updateAppVector(payloadCopy.c_str());
        return;

    case CMD_SWITCH:
        dmNav_->switchToApp(payloadCopy.c_str());
        return;

    case CMD_SEND_SCREEN:
        MQTTManager.publish("screen", dmControl_->ledsAsJson().c_str());
        return;

    case CMD_SETTINGS:
        dmControl_->setNewSettings(payloadCopy.c_str());
        return;

    case CMD_R2D2:
        mqttSound_->r2d2(payloadCopy.c_str());
        return;

    case CMD_NEXT_APP:
        dmNav_->nextApp();
        return;

    case CMD_PREVIOUS_APP:
        dmNav_->previousApp();
        return;

    case CMD_RTTTL:
        mqttSound_->playRTTTLString(payloadCopy.c_str());
        return;

    case CMD_POWER:
    {
        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, payloadCopy.c_str());
        if (error)
        {
            if (systemConfig.debugMode)
                DEBUG_PRINTLN(F("Failed to parse json"));
            return;
        }
        if (doc.containsKey("power"))
            dmControl_->setPower(doc["power"].as<bool>());
        return;
    }

    case CMD_SLEEP:
    {
        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, payloadCopy.c_str());
        if (error)
        {
            if (systemConfig.debugMode)
                DEBUG_PRINTLN(F("Failed to parse json"));
            return;
        }
        if (doc.containsKey("sleep"))
        {
            dmControl_->setPower(false);
            mqttPower_->sleep(doc["sleep"].as<uint64_t>());
        }
        return;
    }

    case CMD_INDICATOR1:
        dmNotify_->indicatorParser(1, payloadCopy.c_str());
        return;

    case CMD_INDICATOR2:
        dmNotify_->indicatorParser(2, payloadCopy.c_str());
        return;

    case CMD_INDICATOR3:
        dmNotify_->indicatorParser(3, payloadCopy.c_str());
        return;

    case CMD_MOODLIGHT:
        dmControl_->moodlight(payloadCopy.c_str());
        return;

    case CMD_REBOOT:
        if (systemConfig.debugMode)
            DEBUG_PRINTLN("REBOOT COMMAND RECEIVED");
        delay(1000);
        ESP.restart();
        return;

    case CMD_SOUND:
        mqttSound_->parseSound(payloadCopy.c_str());
        return;

    case CMD_CUSTOM:
    {
        String topicName = extractCustomTopicName(strTopic, mqttConfig.prefix);
        if (topicName.length() > 0)
            dmNav_->parseCustomPage(topicName, payloadCopy.c_str(), false);
        return;
    }

    case CMD_UNKNOWN:
    default:
        break;
    }

    // Fall-through: update mqttValues map for stored topics
    if (mqttValues.find(strTopic) != mqttValues.end())
    {
        mqttValues[strTopic] = payloadCopy;
        if (systemConfig.debugMode)
        {
            Serial.print("Updated existing topic: ");
            Serial.println(strTopic);
            Serial.print("New value: ");
            Serial.println(mqttValues[strTopic]);
        }
    }
}
