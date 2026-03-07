#include "MessageRouter.h"

MqttCommandType routeTopic(const String& topic, const String& prefix)
{
    // Topic must start with prefix
    if (!topic.startsWith(prefix.c_str()))
        return CMD_UNKNOWN;

    // Extract suffix after prefix (e.g. "/notify", "/custom/MyApp")
    String suffix = topic.substring(prefix.length());

    // /custom/# wildcard — must check before exact matches
    if (suffix.startsWith("/custom/") || suffix == "/custom")
        return CMD_CUSTOM;

    // Exact topic matches
    if (suffix == "/notify")          return CMD_NOTIFY;
    if (suffix == "/notify/dismiss")  return CMD_NOTIFY_DISMISS;
    if (suffix == "/doupdate")        return CMD_DO_UPDATE;
    if (suffix == "/apps")            return CMD_APPS;
    if (suffix == "/switch")          return CMD_SWITCH;
    if (suffix == "/sendscreen")      return CMD_SEND_SCREEN;
    if (suffix == "/settings")        return CMD_SETTINGS;
    if (suffix == "/r2d2")            return CMD_R2D2;
    if (suffix == "/nextapp")         return CMD_NEXT_APP;
    if (suffix == "/previousapp")     return CMD_PREVIOUS_APP;
    if (suffix == "/rtttl")           return CMD_RTTTL;
    if (suffix == "/power")           return CMD_POWER;
    if (suffix == "/sleep")           return CMD_SLEEP;
    if (suffix == "/indicator1")      return CMD_INDICATOR1;
    if (suffix == "/indicator2")      return CMD_INDICATOR2;
    if (suffix == "/indicator3")      return CMD_INDICATOR3;
    if (suffix == "/moodlight")       return CMD_MOODLIGHT;
    if (suffix == "/reboot")          return CMD_REBOOT;
    if (suffix == "/sound")           return CMD_SOUND;

    return CMD_UNKNOWN;
}

String extractCustomTopicName(const String& topic, const String& prefix)
{
    String customPrefix = prefix + "/custom/";
    if (!topic.startsWith(customPrefix.c_str()))
        return "";
    return topic.substring(customPrefix.length());
}

bool isJsonPayload(const String& payload)
{
    if (payload.length() < 2)
        return false;
    return payload.charAt(0) == '{' && payload.charAt(payload.length() - 1) == '}';
}

// Canonical subscription topic list. Matches the topics in onMqttConnected().
// Note: /nextapp was duplicated in the original code — listed once here.
std::vector<String> getSubscriptionTopics()
{
    return {
        "/brightness",
        "/notify/dismiss",
        "/notify",
        "/custom/#",
        "/switch",
        "/settings",
        "/previousapp",
        "/nextapp",
        "/doupdate",
        "/apps",
        "/power",
        "/sleep",
        "/indicator1",
        "/indicator2",
        "/indicator3",
        "/timeformat",
        "/dateformat",
        "/reboot",
        "/moodlight",
        "/sound",
        "/rtttl",
        "/sendscreen",
        "/r2d2",
    };
}
