#include "OverlayMapping.h"

// Enum → lowercase string. Used for MQTT status responses and debug logging.
// NONE maps to "clear" (not "none") to match the weather API convention.
String overlayToString(OverlayEffect effect)
{
    switch (effect)
    {
    case DRIZZLE:
        return "drizzle";
    case RAIN:
        return "rain";
    case SNOW:
        return "snow";
    case STORM:
        return "storm";
    case THUNDER:
        return "thunder";
    case FROST:
        return "frost";
    case NONE:
        return "clear";
    default:
        return "invalid";
    }
}

// String → enum. Case-insensitive via equalsIgnoreCase().
// Unrecognized strings default to NONE (clear weather).
// Used by MQTTManager to parse overlay commands from the API.
OverlayEffect overlayFromString(const String &overlay)
{
    if (overlay.equalsIgnoreCase("drizzle"))
        return DRIZZLE;
    else if (overlay.equalsIgnoreCase("rain"))
        return RAIN;
    else if (overlay.equalsIgnoreCase("snow"))
        return SNOW;
    else if (overlay.equalsIgnoreCase("storm"))
        return STORM;
    else if (overlay.equalsIgnoreCase("thunder"))
        return THUNDER;
    else if (overlay.equalsIgnoreCase("frost"))
        return FROST;
    else if (overlay.equalsIgnoreCase("clear"))
        return NONE;
    else
        return NONE; // Unknown → default to clear
}
