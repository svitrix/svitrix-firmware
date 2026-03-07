#include "AppRegistry.h"

// Built-in native app names. Order matches firmware registration in Apps.cpp.
std::vector<String> getNativeAppNames()
{
    return {"Time", "Date", "Temperature", "Humidity", "Battery"};
}

bool isNativeApp(const String& name)
{
    std::vector<String> natives = getNativeAppNames();
    for (size_t i = 0; i < natives.size(); i++)
    {
        if (natives[i] == name)
            return true;
    }
    return false;
}

// Build JSON object: {"name1":0,"name2":1,...}
// Manual string building to avoid ArduinoJson dependency in native tests.
String serializeAppList(const std::vector<String>& appNames)
{
    String json = "{";
    for (size_t i = 0; i < appNames.size(); i++)
    {
        if (i > 0)
            json += ",";
        json += "\"";
        json += appNames[i];
        json += "\":";
        json += String((int)i);
    }
    json += "}";
    return json;
}
