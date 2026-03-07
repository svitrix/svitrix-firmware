/**
 * @file Apps_Registry.cpp
 * @brief App roster management: global data structures and lookup functions.
 *
 * Defines the live app vector, custom app map, and provides functions
 * to look up apps by name, index, or callback pointer.
 */
#include "Apps_internal.h"

// ── Global app roster ──────────────────────────────────────────────

std::vector<std::pair<String, AppCallback>> Apps; ///< Active app roster (name + callback)
String currentCustomApp;                          ///< Name of the currently active custom app
std::map<String, CustomApp> customApps;           ///< All registered custom apps by name

// ── Lookup functions ───────────────────────────────────────────────

/// Look up a custom app by name.
/// @return Pointer to the CustomApp, or nullptr if not found.
CustomApp *getCustomAppByName(const String& name)
{
    return customApps.count(name) ? &customApps[name] : nullptr;
}

/// Get the name of the app at a given index in the roster.
/// @return The app name, or "" if index is out of bounds.
String getAppNameAtIndex(int index)
{
    if (index >= 0 && index < (int)Apps.size())
    {
        return Apps[index].first;
    }
    return "";
}

/// Find the index of an app by name.
/// @return The zero-based index, or -1 if not found.
int findAppIndexByName(const String& name)
{
    auto it = std::find_if(Apps.begin(), Apps.end(), [&name](const std::pair<String, AppCallback>& appPair)
                           { return appPair.first == name; });
    if (it != Apps.end())
    {
        return std::distance(Apps.begin(), it);
    }
    return -1;
}
