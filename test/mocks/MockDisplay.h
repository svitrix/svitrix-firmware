#ifndef MOCK_DISPLAY_H
#define MOCK_DISPLAY_H

#include "Arduino.h"
#include "IDisplayRenderer.h"
#include "IDisplayControl.h"
#include "IDisplayNavigation.h"
#include "IDisplayNotifier.h"

// ---------------------------------------------------------------------------
// MockDisplayRenderer — tracks calls and last arguments for IDisplayRenderer
// ---------------------------------------------------------------------------
struct MockDisplayRenderer : public IDisplayRenderer
{
    int clearCalls = 0;
    int showCalls = 0;
    int resetTextColorCalls = 0;
    int setTextColorCalls = 0;
    int printTextCalls = 0;
    int hsvTextCalls = 0;
    int drawFilledRectCalls = 0;
    int drawRGBBitmapCalls = 0;
    int drawBMPCalls = 0;
    int drawProgressBarCalls = 0;
    int drawMenuIndicatorCalls = 0;

    uint32_t lastTextColor = 0;
    int16_t lastPrintX = 0;
    int16_t lastPrintY = 0;
    String lastPrintText;
    bool lastPrintCentered = false;
    byte lastPrintCase = 0;
    int16_t lastRectX = 0;
    int16_t lastRectY = 0;
    int16_t lastRectW = 0;
    int16_t lastRectH = 0;
    uint32_t lastRectColor = 0;
    int lastProgress = 0;
    uint32_t lastPColor = 0;
    uint32_t lastPbColor = 0;
    int lastMenuCur = 0;
    int lastMenuTotal = 0;
    uint32_t lastMenuColor = 0;

    void clear() override { clearCalls++; }
    void show() override { showCalls++; }
    void resetTextColor() override { resetTextColorCalls++; }
    void setTextColor(uint32_t color) override { setTextColorCalls++; lastTextColor = color; }

    void printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase) override
    {
        printTextCalls++;
        lastPrintX = x;
        lastPrintY = y;
        lastPrintText = text;
        lastPrintCentered = centered;
        lastPrintCase = textCase;
    }

    void HSVtext(int16_t x, int16_t y, const char *text, bool clr, byte textCase) override
    {
        hsvTextCalls++;
        (void)x; (void)y; (void)text; (void)clr; (void)textCase;
    }

    void drawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) override
    {
        drawFilledRectCalls++;
        lastRectX = x; lastRectY = y; lastRectW = w; lastRectH = h; lastRectColor = color;
    }

    void drawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h) override
    {
        drawRGBBitmapCalls++;
        (void)x; (void)y; (void)bitmap; (void)w; (void)h;
    }

    void drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) override
    {
        drawBMPCalls++;
        (void)x; (void)y; (void)bitmap; (void)w; (void)h;
    }

    void drawProgressBar(int16_t x, int16_t y, int progress, uint32_t pColor, uint32_t pbColor) override
    {
        drawProgressBarCalls++;
        (void)x; (void)y;
        lastProgress = progress;
        lastPColor = pColor;
        lastPbColor = pbColor;
    }

    void drawMenuIndicator(int cur, int total, uint32_t color) override
    {
        drawMenuIndicatorCalls++;
        lastMenuCur = cur;
        lastMenuTotal = total;
        lastMenuColor = color;
    }
};

// ---------------------------------------------------------------------------
// MockDisplayControl — tracks calls and last arguments for IDisplayControl
// ---------------------------------------------------------------------------
struct MockDisplayControl : public IDisplayControl
{
    int setBrightnessCalls = 0;
    int setPowerCalls = 0;
    int powerStateParseCalls = 0;
    int setAutoTransitionCalls = 0;
    int applyAllSettingsCalls = 0;
    int setNewSettingsCalls = 0;
    int getSettingsCalls = 0;
    int getStatsCalls = 0;
    int moodlightCalls = 0;
    int ledsAsJsonCalls = 0;

    int lastBrightness = 0;
    bool lastPowerState = false;
    bool lastAutoTransition = false;
    bool autoTransitionReturn = true;
    bool moodlightReturn = true;
    String settingsReturn = "{}";
    String statsReturn = "{}";
    String ledsReturn = "[]";

    void setBrightness(int bri) override { setBrightnessCalls++; lastBrightness = bri; }
    void setPower(bool state) override { setPowerCalls++; lastPowerState = state; }
    void powerStateParse(const char *json) override { powerStateParseCalls++; (void)json; }

    bool setAutoTransition(bool active) override
    {
        setAutoTransitionCalls++;
        lastAutoTransition = active;
        return autoTransitionReturn;
    }

    void applyAllSettings() override { applyAllSettingsCalls++; }
    void setNewSettings(const char *json) override { setNewSettingsCalls++; (void)json; }
    String getSettings() override { getSettingsCalls++; return settingsReturn; }
    String getStats() override { getStatsCalls++; return statsReturn; }
    bool moodlight(const char *json) override { moodlightCalls++; (void)json; return moodlightReturn; }
    String ledsAsJson() override { ledsAsJsonCalls++; return ledsReturn; }
};

// ---------------------------------------------------------------------------
// MockDisplayNavigation — tracks calls for IDisplayNavigation
// ---------------------------------------------------------------------------
struct MockDisplayNavigation : public IDisplayNavigation
{
    int nextAppCalls = 0;
    int previousAppCalls = 0;
    int switchToAppCalls = 0;
    int updateAppVectorCalls = 0;
    int reorderAppsCalls = 0;
    int getAppsAsJsonCalls = 0;
    int getAppsWithIconCalls = 0;
    int parseCustomPageCalls = 0;
    int getEffectNamesCalls = 0;
    int getTransitionNamesCalls = 0;
    int loadNativeAppsCalls = 0;
    int setCustomAppColorsCalls = 0;

    String lastSwitchJson;
    String lastCustomPageName;
    String lastCustomPageJson;
    bool lastPreventSave = false;
    uint32_t lastCustomAppColor = 0;
    bool switchReturn = true;
    bool parseReturn = true;
    String effectNamesReturn = "effect1,effect2";
    String transitionNamesReturn = "trans1,trans2";
    String appsJsonReturn = "[]";
    String appsWithIconReturn = "[]";

    void nextApp() override { nextAppCalls++; }
    void previousApp() override { previousAppCalls++; }

    bool switchToApp(const char *json) override
    {
        switchToAppCalls++;
        lastSwitchJson = json;
        return switchReturn;
    }

    void updateAppVector(const char *json) override { updateAppVectorCalls++; (void)json; }
    void reorderApps(const String &jsonString) override { reorderAppsCalls++; (void)jsonString; }
    String getAppsAsJson() override { getAppsAsJsonCalls++; return appsJsonReturn; }
    String getAppsWithIcon() override { getAppsWithIconCalls++; return appsWithIconReturn; }

    bool parseCustomPage(const String &name, const char *json, bool preventSave) override
    {
        parseCustomPageCalls++;
        lastCustomPageName = name;
        lastCustomPageJson = json;
        lastPreventSave = preventSave;
        return parseReturn;
    }

    String getEffectNames() override { getEffectNamesCalls++; return effectNamesReturn; }
    String getTransitionNames() override { getTransitionNamesCalls++; return transitionNamesReturn; }
    void loadNativeApps() override { loadNativeAppsCalls++; }
    void setCustomAppColors(uint32_t color) override { setCustomAppColorsCalls++; lastCustomAppColor = color; }
};

// ---------------------------------------------------------------------------
// MockDisplayNotifier — tracks calls for IDisplayNotifier
// ---------------------------------------------------------------------------
struct MockDisplayNotifier : public IDisplayNotifier
{
    int generateNotificationCalls = 0;
    int dismissCalls = 0;
    int indicatorParserCalls = 0;
    int setIndicator1ColorCalls = 0;
    int setIndicator1StateCalls = 0;
    int setIndicator2ColorCalls = 0;
    int setIndicator2StateCalls = 0;
    int setIndicator3ColorCalls = 0;
    int setIndicator3StateCalls = 0;

    uint8_t lastNotifySource = 0;
    bool notifyReturn = true;
    bool indicatorReturn = true;
    uint32_t lastIndicator1Color = 0;
    uint32_t lastIndicator2Color = 0;
    uint32_t lastIndicator3Color = 0;
    bool lastIndicator1State = false;
    bool lastIndicator2State = false;
    bool lastIndicator3State = false;

    bool generateNotification(uint8_t source, const char *json) override
    {
        generateNotificationCalls++;
        lastNotifySource = source;
        (void)json;
        return notifyReturn;
    }

    void dismissNotify() override { dismissCalls++; }

    bool indicatorParser(uint8_t indicator, const char *json) override
    {
        indicatorParserCalls++;
        (void)indicator; (void)json;
        return indicatorReturn;
    }

    void setIndicator1Color(uint32_t color) override { setIndicator1ColorCalls++; lastIndicator1Color = color; }
    void setIndicator1State(bool state) override { setIndicator1StateCalls++; lastIndicator1State = state; }
    void setIndicator2Color(uint32_t color) override { setIndicator2ColorCalls++; lastIndicator2Color = color; }
    void setIndicator2State(bool state) override { setIndicator2StateCalls++; lastIndicator2State = state; }
    void setIndicator3Color(uint32_t color) override { setIndicator3ColorCalls++; lastIndicator3Color = color; }
    void setIndicator3State(bool state) override { setIndicator3StateCalls++; lastIndicator3State = state; }
};

#endif // MOCK_DISPLAY_H
