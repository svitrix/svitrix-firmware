#include "StatsBuilder.h"
#include <cstdio>

// Helper: append "key":value for integer types
static void addInt(String& json, const char* key, long value)
{
    json += "\"";
    json += key;
    json += "\":";
    json += String((int)value);
}

// Helper: append "key":"value" for string types
static void addString(String& json, const char* key, const String& value)
{
    json += "\"";
    json += key;
    json += "\":\"";
    json += value;
    json += "\"";
}

// Helper: append "key":true/false for boolean types
static void addBool(String& json, const char* key, bool value)
{
    json += "\"";
    json += key;
    json += "\":";
    json += value ? "true" : "false";
}

// Build compact JSON matching the original ArduinoJson output format.
// Field order matches DisplayManager::getStats() for compatibility.
String buildStatsJson(const StatsData& data)
{
    String json = "{";

    addInt(json, "bat", data.batteryPercent);
    json += ",";

    addInt(json, "bat_raw", data.batteryRaw);
    json += ",";

    addInt(json, "type", 0);
    json += ",";

    addInt(json, "lux", data.lux);
    json += ",";

    addInt(json, "ldr_raw", data.ldrRaw);
    json += ",";

    // ram is uint32_t — use long cast to avoid overflow in String(int)
    json += "\"ram\":";
    json += String((long)data.freeRam);
    json += ",";

    addInt(json, "bri", data.brightness);

    if (data.hasSensor)
    {
        json += ",";
        json += "\"temp\":";
        char tempBuf[16];
        snprintf(tempBuf, sizeof(tempBuf), "%.*f", (int)data.tempDecimalPlaces, (double)data.temperature);
        json += tempBuf;
        json += ",";
        addInt(json, "hum", data.humidity);
    }

    json += ",";
    addString(json, "uptime", data.uptime);
    json += ",";

    addInt(json, "wifi_signal", data.wifiSignal);
    json += ",";

    // messages is uint32_t
    json += "\"messages\":";
    json += String((long)data.receivedMessages);
    json += ",";

    addString(json, "version", data.version);
    json += ",";

    addBool(json, "indicator1", data.indicator1);
    json += ",";
    addBool(json, "indicator2", data.indicator2);
    json += ",";
    addBool(json, "indicator3", data.indicator3);
    json += ",";

    addString(json, "app", data.currentApp);
    json += ",";
    addString(json, "uid", data.uid);
    json += ",";
    addBool(json, "matrix", data.matrixOn);
    json += ",";
    addString(json, "ip_address", data.ipAddress);

    json += "}";
    return json;
}
