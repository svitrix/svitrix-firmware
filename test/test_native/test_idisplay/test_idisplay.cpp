#include <unity.h>
#include "MockDisplay.h"

// ═══════════════════════════════════════════════════════════════════════════
// Group 1: IDisplayRenderer
// ═══════════════════════════════════════════════════════════════════════════

void test_renderer_clear_and_show(void)
{
    MockDisplayRenderer mock;
    IDisplayRenderer *iface = &mock;

    TEST_ASSERT_EQUAL(0, mock.clearCalls);
    TEST_ASSERT_EQUAL(0, mock.showCalls);

    iface->clear();
    iface->clear();
    iface->show();

    TEST_ASSERT_EQUAL(2, mock.clearCalls);
    TEST_ASSERT_EQUAL(1, mock.showCalls);
}

void test_renderer_print_text(void)
{
    MockDisplayRenderer mock;
    IDisplayRenderer *iface = &mock;

    iface->printText(10, 20, "Hello", true, 1);

    TEST_ASSERT_EQUAL(1, mock.printTextCalls);
    TEST_ASSERT_EQUAL(10, mock.lastPrintX);
    TEST_ASSERT_EQUAL(20, mock.lastPrintY);
    TEST_ASSERT_EQUAL_STRING("Hello", mock.lastPrintText.c_str());
    TEST_ASSERT_TRUE(mock.lastPrintCentered);
    TEST_ASSERT_EQUAL(1, mock.lastPrintCase);
}

void test_renderer_draw_progress_bar(void)
{
    MockDisplayRenderer mock;
    IDisplayRenderer *iface = &mock;

    iface->drawProgressBar(0, 7, 75, 0x00FF00, 0x333333);

    TEST_ASSERT_EQUAL(1, mock.drawProgressBarCalls);
    TEST_ASSERT_EQUAL(75, mock.lastProgress);
    TEST_ASSERT_EQUAL_HEX32(0x00FF00, mock.lastPColor);
    TEST_ASSERT_EQUAL_HEX32(0x333333, mock.lastPbColor);
}

void test_renderer_draw_filled_rect(void)
{
    MockDisplayRenderer mock;
    IDisplayRenderer *iface = &mock;

    iface->drawFilledRect(2, 3, 10, 5, 0xFF0000);

    TEST_ASSERT_EQUAL(1, mock.drawFilledRectCalls);
    TEST_ASSERT_EQUAL(2, mock.lastRectX);
    TEST_ASSERT_EQUAL(3, mock.lastRectY);
    TEST_ASSERT_EQUAL(10, mock.lastRectW);
    TEST_ASSERT_EQUAL(5, mock.lastRectH);
    TEST_ASSERT_EQUAL_HEX32(0xFF0000, mock.lastRectColor);
}

// ═══════════════════════════════════════════════════════════════════════════
// Group 2: IDisplayControl
// ═══════════════════════════════════════════════════════════════════════════

void test_control_set_brightness(void)
{
    MockDisplayControl mock;
    IDisplayControl *iface = &mock;

    iface->setBrightness(128);

    TEST_ASSERT_EQUAL(1, mock.setBrightnessCalls);
    TEST_ASSERT_EQUAL(128, mock.lastBrightness);
}

void test_control_set_power(void)
{
    MockDisplayControl mock;
    IDisplayControl *iface = &mock;

    iface->setPower(true);
    TEST_ASSERT_TRUE(mock.lastPowerState);

    iface->setPower(false);
    TEST_ASSERT_FALSE(mock.lastPowerState);
    TEST_ASSERT_EQUAL(2, mock.setPowerCalls);
}

void test_control_get_settings(void)
{
    MockDisplayControl mock;
    IDisplayControl *iface = &mock;

    mock.settingsReturn = "{\"brightness\":100}";
    String result = iface->getSettings();

    TEST_ASSERT_EQUAL(1, mock.getSettingsCalls);
    TEST_ASSERT_EQUAL_STRING("{\"brightness\":100}", result.c_str());
}

void test_control_auto_transition(void)
{
    MockDisplayControl mock;
    IDisplayControl *iface = &mock;

    mock.autoTransitionReturn = false;
    bool result = iface->setAutoTransition(true);

    TEST_ASSERT_EQUAL(1, mock.setAutoTransitionCalls);
    TEST_ASSERT_TRUE(mock.lastAutoTransition);
    TEST_ASSERT_FALSE(result);
}

// ═══════════════════════════════════════════════════════════════════════════
// Group 3: IDisplayNavigation
// ═══════════════════════════════════════════════════════════════════════════

void test_nav_next_previous(void)
{
    MockDisplayNavigation mock;
    IDisplayNavigation *iface = &mock;

    iface->nextApp();
    iface->nextApp();
    iface->previousApp();

    TEST_ASSERT_EQUAL(2, mock.nextAppCalls);
    TEST_ASSERT_EQUAL(1, mock.previousAppCalls);
}

void test_nav_get_effect_names(void)
{
    MockDisplayNavigation mock;
    IDisplayNavigation *iface = &mock;

    mock.effectNamesReturn = "Plasma,Pacifica,Matrix";
    String result = iface->getEffectNames();

    TEST_ASSERT_EQUAL(1, mock.getEffectNamesCalls);
    TEST_ASSERT_EQUAL_STRING("Plasma,Pacifica,Matrix", result.c_str());
}

void test_nav_switch_to_app(void)
{
    MockDisplayNavigation mock;
    IDisplayNavigation *iface = &mock;

    mock.switchReturn = true;
    bool result = iface->switchToApp("{\"name\":\"Time\"}");

    TEST_ASSERT_EQUAL(1, mock.switchToAppCalls);
    TEST_ASSERT_EQUAL_STRING("{\"name\":\"Time\"}", mock.lastSwitchJson.c_str());
    TEST_ASSERT_TRUE(result);
}

void test_nav_parse_custom_page(void)
{
    MockDisplayNavigation mock;
    IDisplayNavigation *iface = &mock;

    mock.parseReturn = true;
    bool result = iface->parseCustomPage("MyApp", "{\"text\":\"hi\"}", true);

    TEST_ASSERT_EQUAL(1, mock.parseCustomPageCalls);
    TEST_ASSERT_EQUAL_STRING("MyApp", mock.lastCustomPageName.c_str());
    TEST_ASSERT_EQUAL_STRING("{\"text\":\"hi\"}", mock.lastCustomPageJson.c_str());
    TEST_ASSERT_TRUE(mock.lastPreventSave);
    TEST_ASSERT_TRUE(result);
}

// ═══════════════════════════════════════════════════════════════════════════
// Group 4: IDisplayNotifier
// ═══════════════════════════════════════════════════════════════════════════

void test_notifier_dismiss(void)
{
    MockDisplayNotifier mock;
    IDisplayNotifier *iface = &mock;

    iface->dismissNotify();
    iface->dismissNotify();
    iface->dismissNotify();

    TEST_ASSERT_EQUAL(3, mock.dismissCalls);
}

void test_notifier_indicator_state(void)
{
    MockDisplayNotifier mock;
    IDisplayNotifier *iface = &mock;

    iface->setIndicator1State(true);
    iface->setIndicator2State(false);
    iface->setIndicator3State(true);

    TEST_ASSERT_TRUE(mock.lastIndicator1State);
    TEST_ASSERT_FALSE(mock.lastIndicator2State);
    TEST_ASSERT_TRUE(mock.lastIndicator3State);

    TEST_ASSERT_EQUAL(1, mock.setIndicator1StateCalls);
    TEST_ASSERT_EQUAL(1, mock.setIndicator2StateCalls);
    TEST_ASSERT_EQUAL(1, mock.setIndicator3StateCalls);
}

void test_notifier_indicator_color(void)
{
    MockDisplayNotifier mock;
    IDisplayNotifier *iface = &mock;

    iface->setIndicator1Color(0xFF0000);
    iface->setIndicator2Color(0x00FF00);
    iface->setIndicator3Color(0x0000FF);

    TEST_ASSERT_EQUAL_HEX32(0xFF0000, mock.lastIndicator1Color);
    TEST_ASSERT_EQUAL_HEX32(0x00FF00, mock.lastIndicator2Color);
    TEST_ASSERT_EQUAL_HEX32(0x0000FF, mock.lastIndicator3Color);

    TEST_ASSERT_EQUAL(1, mock.setIndicator1ColorCalls);
    TEST_ASSERT_EQUAL(1, mock.setIndicator2ColorCalls);
    TEST_ASSERT_EQUAL(1, mock.setIndicator3ColorCalls);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test runner
// ═══════════════════════════════════════════════════════════════════════════

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // IDisplayRenderer
    RUN_TEST(test_renderer_clear_and_show);
    RUN_TEST(test_renderer_print_text);
    RUN_TEST(test_renderer_draw_progress_bar);
    RUN_TEST(test_renderer_draw_filled_rect);

    // IDisplayControl
    RUN_TEST(test_control_set_brightness);
    RUN_TEST(test_control_set_power);
    RUN_TEST(test_control_get_settings);
    RUN_TEST(test_control_auto_transition);

    // IDisplayNavigation
    RUN_TEST(test_nav_next_previous);
    RUN_TEST(test_nav_get_effect_names);
    RUN_TEST(test_nav_switch_to_app);
    RUN_TEST(test_nav_parse_custom_page);

    // IDisplayNotifier
    RUN_TEST(test_notifier_dismiss);
    RUN_TEST(test_notifier_indicator_state);
    RUN_TEST(test_notifier_indicator_color);

    return UNITY_END();
}
