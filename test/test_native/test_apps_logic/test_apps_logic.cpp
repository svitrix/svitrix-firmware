/**
 * @file test_apps_logic.cpp
 * @brief Tests for Apps_Registry lookup functions and getTimeFormat.
 *
 * Uses inline pattern (Pattern 1): copies pure logic into the test file
 * to avoid pulling in the full hardware dependency chain.
 * The tests exercise boundary conditions, not-found paths, and edge
 * cases that would cause crashes or silent misbehavior in production.
 */
#include <unity.h>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

// ── Minimal Arduino String mock (from test/mocks) ─────────────────
// We include the shared mock for String.
#include "Arduino.h"

// ── Type stubs ────────────────────────────────────────────────────
// Stand-in for the real AppCallback (function pointer to matrix draw).
typedef void (*AppCallback)(void *, void *, int, int, void *);

// Minimal CustomApp — only needs to exist so std::map<String, CustomApp> compiles.
struct CustomApp
{
    String name;
    String text;
};

// ── Global state (mirrors Apps_Registry.cpp) ──────────────────────

static std::vector<std::pair<String, AppCallback>> Apps;
static std::map<String, CustomApp> customApps;

// ── Inline copies of the functions under test ─────────────────────

CustomApp *getCustomAppByName(const String &name)
{
    return customApps.count(name) ? &customApps[name] : nullptr;
}

String getAppNameAtIndex(int index)
{
    if (index >= 0 && index < (int)Apps.size())
    {
        return Apps[index].first;
    }
    return "";
}

int findAppIndexByName(const String &name)
{
    auto it = std::find_if(Apps.begin(), Apps.end(), [&name](const std::pair<String, AppCallback> &appPair)
                           { return appPair.first == name; });
    if (it != Apps.end())
    {
        return std::distance(Apps.begin(), it);
    }
    return -1;
}

// ── Inline getTimeFormat + its config stub ────────────────────────

struct TimeConfig
{
    int timeMode;
    String timeFormat;
    bool startOnMonday;
};

static TimeConfig timeConfig;

const char *getTimeFormat()
{
    if (timeConfig.timeMode == 0)
    {
        return timeConfig.timeFormat.c_str();
    }
    else
    {
        if (timeConfig.timeFormat.length() > 5)
        {
            return timeConfig.timeFormat[2] == ' ' ? "%H %M" : "%H:%M";
        }
        else
        {
            return timeConfig.timeFormat.c_str();
        }
    }
}

// ── Dummy callbacks for testing ───────────────────────────────────

void dummyApp1(void *, void *, int, int, void *) {}
void dummyApp2(void *, void *, int, int, void *) {}
void dummyApp3(void *, void *, int, int, void *) {}

// ── setUp / tearDown ──────────────────────────────────────────────

void setUp(void)
{
    Apps.clear();
    customApps.clear();
    timeConfig.timeMode = 0;
    timeConfig.timeFormat = "%H:%M:%S";
    timeConfig.startOnMonday = true;
}

void tearDown(void) {}

// ══════════════════════════════════════════════════════════════════
//  getCustomAppByName tests
// ══════════════════════════════════════════════════════════════════

void test_getCustomApp_returns_nullptr_when_empty(void)
{
    TEST_ASSERT_NULL(getCustomAppByName("anything"));
}

void test_getCustomApp_returns_nullptr_for_missing_key(void)
{
    CustomApp ca;
    ca.name = "exists";
    customApps["exists"] = ca;
    TEST_ASSERT_NULL(getCustomAppByName("missing"));
}

void test_getCustomApp_returns_valid_pointer(void)
{
    CustomApp ca;
    ca.name = "myapp";
    ca.text = "hello";
    customApps["myapp"] = ca;
    CustomApp *result = getCustomAppByName("myapp");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("hello", result->text.c_str());
}

void test_getCustomApp_returns_mutable_pointer(void)
{
    CustomApp ca;
    ca.text = "original";
    customApps["app"] = ca;
    CustomApp *result = getCustomAppByName("app");
    result->text = "modified";
    // Verify the change persists in the map
    TEST_ASSERT_EQUAL_STRING("modified", customApps["app"].text.c_str());
}

void test_getCustomApp_case_sensitive(void)
{
    CustomApp ca;
    ca.name = "MyApp";
    customApps["MyApp"] = ca;
    TEST_ASSERT_NULL(getCustomAppByName("myapp"));
    TEST_ASSERT_NULL(getCustomAppByName("MYAPP"));
    TEST_ASSERT_NOT_NULL(getCustomAppByName("MyApp"));
}

void test_getCustomApp_empty_string_key(void)
{
    CustomApp ca;
    ca.name = "";
    customApps[""] = ca;
    TEST_ASSERT_NOT_NULL(getCustomAppByName(""));
}

// ══════════════════════════════════════════════════════════════════
//  getAppNameAtIndex tests
// ══════════════════════════════════════════════════════════════════

void test_getAppNameAtIndex_empty(void)
{
    TEST_ASSERT_EQUAL_STRING("", getAppNameAtIndex(0).c_str());
}

void test_getAppNameAtIndex_valid(void)
{
    Apps.push_back({"Time", dummyApp1});
    Apps.push_back({"Date", dummyApp2});
    TEST_ASSERT_EQUAL_STRING("Time", getAppNameAtIndex(0).c_str());
    TEST_ASSERT_EQUAL_STRING("Date", getAppNameAtIndex(1).c_str());
}

void test_getAppNameAtIndex_negative(void)
{
    Apps.push_back({"Time", dummyApp1});
    TEST_ASSERT_EQUAL_STRING("", getAppNameAtIndex(-1).c_str());
}

void test_getAppNameAtIndex_out_of_bounds(void)
{
    Apps.push_back({"Time", dummyApp1});
    TEST_ASSERT_EQUAL_STRING("", getAppNameAtIndex(1).c_str());
    TEST_ASSERT_EQUAL_STRING("", getAppNameAtIndex(100).c_str());
}

void test_getAppNameAtIndex_last_element(void)
{
    Apps.push_back({"A", dummyApp1});
    Apps.push_back({"B", dummyApp2});
    Apps.push_back({"C", dummyApp3});
    TEST_ASSERT_EQUAL_STRING("C", getAppNameAtIndex(2).c_str());
}

// ══════════════════════════════════════════════════════════════════
//  findAppIndexByName tests
// ══════════════════════════════════════════════════════════════════

void test_findAppIndex_empty_roster(void)
{
    TEST_ASSERT_EQUAL_INT(-1, findAppIndexByName("anything"));
}

void test_findAppIndex_found(void)
{
    Apps.push_back({"Time", dummyApp1});
    Apps.push_back({"Date", dummyApp2});
    Apps.push_back({"Temp", dummyApp3});
    TEST_ASSERT_EQUAL_INT(0, findAppIndexByName("Time"));
    TEST_ASSERT_EQUAL_INT(1, findAppIndexByName("Date"));
    TEST_ASSERT_EQUAL_INT(2, findAppIndexByName("Temp"));
}

void test_findAppIndex_not_found(void)
{
    Apps.push_back({"Time", dummyApp1});
    TEST_ASSERT_EQUAL_INT(-1, findAppIndexByName("Missing"));
}

void test_findAppIndex_case_sensitive(void)
{
    Apps.push_back({"Time", dummyApp1});
    TEST_ASSERT_EQUAL_INT(-1, findAppIndexByName("time"));
    TEST_ASSERT_EQUAL_INT(-1, findAppIndexByName("TIME"));
}

void test_findAppIndex_empty_name(void)
{
    Apps.push_back({"", dummyApp1});
    TEST_ASSERT_EQUAL_INT(0, findAppIndexByName(""));
}

void test_findAppIndex_duplicate_names_returns_first(void)
{
    Apps.push_back({"Dup", dummyApp1});
    Apps.push_back({"Dup", dummyApp2});
    TEST_ASSERT_EQUAL_INT(0, findAppIndexByName("Dup"));
}

// ══════════════════════════════════════════════════════════════════
//  getTimeFormat tests
// ══════════════════════════════════════════════════════════════════

void test_getTimeFormat_mode0_passthrough(void)
{
    timeConfig.timeMode = 0;
    timeConfig.timeFormat = "%H:%M:%S";
    TEST_ASSERT_EQUAL_STRING("%H:%M:%S", getTimeFormat());
}

void test_getTimeFormat_mode0_custom_format(void)
{
    timeConfig.timeMode = 0;
    timeConfig.timeFormat = "%I:%M %p";
    TEST_ASSERT_EQUAL_STRING("%I:%M %p", getTimeFormat());
}

void test_getTimeFormat_mode_nonzero_long_format_with_colon(void)
{
    // length > 5, char at [2] is ':', should return "%H:%M"
    timeConfig.timeMode = 1;
    timeConfig.timeFormat = "%H:%M:%S";
    TEST_ASSERT_EQUAL_STRING("%H:%M", getTimeFormat());
}

void test_getTimeFormat_mode_nonzero_long_format_with_space(void)
{
    // length > 5, char at [2] is ' ', should return "%H %M"
    timeConfig.timeMode = 2;
    timeConfig.timeFormat = "%H %M %S";
    TEST_ASSERT_EQUAL_STRING("%H %M", getTimeFormat());
}

void test_getTimeFormat_mode_nonzero_short_format_passthrough(void)
{
    // length <= 5, passes through regardless of content
    timeConfig.timeMode = 1;
    timeConfig.timeFormat = "%H:%M";
    TEST_ASSERT_EQUAL_STRING("%H:%M", getTimeFormat());
}

void test_getTimeFormat_mode_nonzero_exactly_5_chars(void)
{
    // length == 5, should passthrough (not > 5)
    timeConfig.timeMode = 3;
    timeConfig.timeFormat = "12:34";
    TEST_ASSERT_EQUAL_STRING("12:34", getTimeFormat());
}

void test_getTimeFormat_mode_nonzero_6_chars_colon_at_2(void)
{
    // length == 6 > 5, char at [2] is ':'
    timeConfig.timeMode = 1;
    timeConfig.timeFormat = "HH:MM:";
    TEST_ASSERT_EQUAL_STRING("%H:%M", getTimeFormat());
}

void test_getTimeFormat_mode_nonzero_6_chars_space_at_2(void)
{
    // length == 6 > 5, char at [2] is ' '
    timeConfig.timeMode = 1;
    timeConfig.timeFormat = "HH MM:";
    TEST_ASSERT_EQUAL_STRING("%H %M", getTimeFormat());
}

void test_getTimeFormat_mode_nonzero_long_neither_colon_nor_space(void)
{
    // length > 5, char at [2] is neither ':' nor ' ' → returns "%H:%M"
    timeConfig.timeMode = 1;
    timeConfig.timeFormat = "HHXMM:SS";
    TEST_ASSERT_EQUAL_STRING("%H:%M", getTimeFormat());
}

// ══════════════════════════════════════════════════════════════════
//  Runner
// ══════════════════════════════════════════════════════════════════

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // getCustomAppByName
    RUN_TEST(test_getCustomApp_returns_nullptr_when_empty);
    RUN_TEST(test_getCustomApp_returns_nullptr_for_missing_key);
    RUN_TEST(test_getCustomApp_returns_valid_pointer);
    RUN_TEST(test_getCustomApp_returns_mutable_pointer);
    RUN_TEST(test_getCustomApp_case_sensitive);
    RUN_TEST(test_getCustomApp_empty_string_key);

    // getAppNameAtIndex
    RUN_TEST(test_getAppNameAtIndex_empty);
    RUN_TEST(test_getAppNameAtIndex_valid);
    RUN_TEST(test_getAppNameAtIndex_negative);
    RUN_TEST(test_getAppNameAtIndex_out_of_bounds);
    RUN_TEST(test_getAppNameAtIndex_last_element);

    // findAppIndexByName
    RUN_TEST(test_findAppIndex_empty_roster);
    RUN_TEST(test_findAppIndex_found);
    RUN_TEST(test_findAppIndex_not_found);
    RUN_TEST(test_findAppIndex_case_sensitive);
    RUN_TEST(test_findAppIndex_empty_name);
    RUN_TEST(test_findAppIndex_duplicate_names_returns_first);

    // getTimeFormat
    RUN_TEST(test_getTimeFormat_mode0_passthrough);
    RUN_TEST(test_getTimeFormat_mode0_custom_format);
    RUN_TEST(test_getTimeFormat_mode_nonzero_long_format_with_colon);
    RUN_TEST(test_getTimeFormat_mode_nonzero_long_format_with_space);
    RUN_TEST(test_getTimeFormat_mode_nonzero_short_format_passthrough);
    RUN_TEST(test_getTimeFormat_mode_nonzero_exactly_5_chars);
    RUN_TEST(test_getTimeFormat_mode_nonzero_6_chars_colon_at_2);
    RUN_TEST(test_getTimeFormat_mode_nonzero_6_chars_space_at_2);
    RUN_TEST(test_getTimeFormat_mode_nonzero_long_neither_colon_nor_space);

    return UNITY_END();
}
