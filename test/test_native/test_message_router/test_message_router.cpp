#include <unity.h>
#include "MessageRouter.h"
#include <set>

static const char* PREFIX = "awtrix_abc";

// --- routeTopic: exact matches ---

void test_route_notify(void)
{
    TEST_ASSERT_EQUAL(CMD_NOTIFY, routeTopic("awtrix_abc/notify", PREFIX));
}

void test_route_notify_dismiss(void)
{
    TEST_ASSERT_EQUAL(CMD_NOTIFY_DISMISS, routeTopic("awtrix_abc/notify/dismiss", PREFIX));
}

void test_route_switch(void)
{
    TEST_ASSERT_EQUAL(CMD_SWITCH, routeTopic("awtrix_abc/switch", PREFIX));
}

void test_route_power(void)
{
    TEST_ASSERT_EQUAL(CMD_POWER, routeTopic("awtrix_abc/power", PREFIX));
}

void test_route_reboot(void)
{
    TEST_ASSERT_EQUAL(CMD_REBOOT, routeTopic("awtrix_abc/reboot", PREFIX));
}

void test_route_sound(void)
{
    TEST_ASSERT_EQUAL(CMD_SOUND, routeTopic("awtrix_abc/sound", PREFIX));
}

void test_route_custom(void)
{
    TEST_ASSERT_EQUAL(CMD_CUSTOM, routeTopic("awtrix_abc/custom/MyApp", PREFIX));
}

// --- routeTopic: edge cases ---

void test_route_unknown_topic(void)
{
    TEST_ASSERT_EQUAL(CMD_UNKNOWN, routeTopic("awtrix_abc/nonexistent", PREFIX));
}

void test_route_empty_topic(void)
{
    TEST_ASSERT_EQUAL(CMD_UNKNOWN, routeTopic("", PREFIX));
}

void test_route_no_prefix(void)
{
    TEST_ASSERT_EQUAL(CMD_UNKNOWN, routeTopic("/notify", PREFIX));
}

void test_route_wrong_prefix(void)
{
    TEST_ASSERT_EQUAL(CMD_UNKNOWN, routeTopic("other_device/notify", PREFIX));
}

// --- routeTopic: all command types ---

void test_route_all_commands(void)
{
    struct TopicCmd {
        const char* suffix;
        MqttCommandType cmd;
    };
    TopicCmd cases[] = {
        {"/notify",          CMD_NOTIFY},
        {"/notify/dismiss",  CMD_NOTIFY_DISMISS},
        {"/doupdate",        CMD_DO_UPDATE},
        {"/apps",            CMD_APPS},
        {"/switch",          CMD_SWITCH},
        {"/sendscreen",      CMD_SEND_SCREEN},
        {"/settings",        CMD_SETTINGS},
        {"/r2d2",            CMD_R2D2},
        {"/nextapp",         CMD_NEXT_APP},
        {"/previousapp",     CMD_PREVIOUS_APP},
        {"/rtttl",           CMD_RTTTL},
        {"/power",           CMD_POWER},
        {"/sleep",           CMD_SLEEP},
        {"/indicator1",      CMD_INDICATOR1},
        {"/indicator2",      CMD_INDICATOR2},
        {"/indicator3",      CMD_INDICATOR3},
        {"/moodlight",       CMD_MOODLIGHT},
        {"/reboot",          CMD_REBOOT},
        {"/sound",           CMD_SOUND},
        {"/custom/test",     CMD_CUSTOM},
    };
    String pfx = PREFIX;
    for (int i = 0; i < 20; i++)
    {
        String topic = pfx + cases[i].suffix;
        MqttCommandType result = routeTopic(topic, pfx);
        TEST_ASSERT_EQUAL_MESSAGE(cases[i].cmd, result, cases[i].suffix);
    }
}

// --- extractCustomTopicName ---

void test_extract_custom_simple(void)
{
    String name = extractCustomTopicName("awtrix_abc/custom/MyApp", PREFIX);
    TEST_ASSERT_EQUAL_STRING("MyApp", name.c_str());
}

void test_extract_custom_nested(void)
{
    String name = extractCustomTopicName("awtrix_abc/custom/folder/app", PREFIX);
    TEST_ASSERT_EQUAL_STRING("folder/app", name.c_str());
}

void test_extract_custom_not_custom_topic(void)
{
    String name = extractCustomTopicName("awtrix_abc/switch", PREFIX);
    TEST_ASSERT_EQUAL_STRING("", name.c_str());
}

void test_extract_custom_empty_name(void)
{
    String name = extractCustomTopicName("awtrix_abc/custom/", PREFIX);
    TEST_ASSERT_EQUAL_STRING("", name.c_str());
}

void test_extract_custom_different_prefix(void)
{
    String name = extractCustomTopicName("other/custom/App", "other");
    TEST_ASSERT_EQUAL_STRING("App", name.c_str());
}

// --- isJsonPayload ---

void test_json_valid(void)
{
    TEST_ASSERT_TRUE(isJsonPayload("{\"text\":\"hello\"}"));
}

void test_json_invalid_no_braces(void)
{
    TEST_ASSERT_FALSE(isJsonPayload("not json"));
}

void test_json_empty(void)
{
    TEST_ASSERT_FALSE(isJsonPayload(""));
}

void test_json_minimal(void)
{
    TEST_ASSERT_TRUE(isJsonPayload("{}"));
}

void test_json_only_opening(void)
{
    TEST_ASSERT_FALSE(isJsonPayload("{"));
}

// --- getSubscriptionTopics ---

void test_subscription_topics_not_empty(void)
{
    std::vector<String> topics = getSubscriptionTopics();
    TEST_ASSERT_TRUE(topics.size() >= 20);
}

void test_subscription_topics_all_start_with_slash(void)
{
    std::vector<String> topics = getSubscriptionTopics();
    for (size_t i = 0; i < topics.size(); i++)
    {
        TEST_ASSERT_TRUE_MESSAGE(
            topics[i].charAt(0) == '/',
            topics[i].c_str());
    }
}

void test_subscription_topics_no_duplicates(void)
{
    std::vector<String> topics = getSubscriptionTopics();
    std::set<std::string> seen;
    for (size_t i = 0; i < topics.size(); i++)
    {
        std::string s(topics[i].c_str());
        TEST_ASSERT_TRUE_MESSAGE(
            seen.find(s) == seen.end(),
            topics[i].c_str());
        seen.insert(s);
    }
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // routeTopic: exact matches
    RUN_TEST(test_route_notify);
    RUN_TEST(test_route_notify_dismiss);
    RUN_TEST(test_route_switch);
    RUN_TEST(test_route_power);
    RUN_TEST(test_route_reboot);
    RUN_TEST(test_route_sound);
    RUN_TEST(test_route_custom);

    // routeTopic: edge cases
    RUN_TEST(test_route_unknown_topic);
    RUN_TEST(test_route_empty_topic);
    RUN_TEST(test_route_no_prefix);
    RUN_TEST(test_route_wrong_prefix);

    // routeTopic: all commands
    RUN_TEST(test_route_all_commands);

    // extractCustomTopicName
    RUN_TEST(test_extract_custom_simple);
    RUN_TEST(test_extract_custom_nested);
    RUN_TEST(test_extract_custom_not_custom_topic);
    RUN_TEST(test_extract_custom_empty_name);
    RUN_TEST(test_extract_custom_different_prefix);

    // isJsonPayload
    RUN_TEST(test_json_valid);
    RUN_TEST(test_json_invalid_no_braces);
    RUN_TEST(test_json_empty);
    RUN_TEST(test_json_minimal);
    RUN_TEST(test_json_only_opening);

    // getSubscriptionTopics
    RUN_TEST(test_subscription_topics_not_empty);
    RUN_TEST(test_subscription_topics_all_start_with_slash);
    RUN_TEST(test_subscription_topics_no_duplicates);

    return UNITY_END();
}
