/**
 * @file DisplayManager.h
 * @brief Coordinator for the LED matrix display.
 *
 * DisplayManager_ is a singleton that owns the 32x8 NeoPixel matrix and
 * coordinates all visual output: native apps, custom apps, notifications,
 * indicators, Artnet/DMX, and moodlight mode.
 *
 * Phase 11 split:
 *   DisplayRenderer_       — implements IDisplayRenderer (text + drawing)
 *   NotificationManager_   — implements IDisplayNotifier (notifications + indicators)
 *   DisplayManager_        — coordinator (IDisplayControl, IDisplayNavigation, IMatrixHost, IButtonHandler)
 *
 * Implementation files:
 *   DisplayManager.cpp           — Core: setup, tick, power, brightness, navigation
 *   DisplayRenderer.cpp          — Text rendering, drawing primitives, charts
 *   NotificationManager.cpp      — Notification parsing, indicators
 *   DisplayManager_CustomApps.cpp — Custom app lifecycle and app vector management
 *   DisplayManager_Settings.cpp  — Settings get/set, stats, serialization
 *   DisplayManager_Artnet.cpp    — Artnet/DMX input and moodlight mode
 *   DisplayManager_ParseHelpers.cpp — Shared JSON parsing utilities
 *
 * All modules share internal state via DisplayManager_internal.h.
 */
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <vector>
#include <FastLED_NeoMatrix.h>
#include "IButtonHandler.h"
#include "IMatrixHost.h"
#include "IDisplayControl.h"
#include "IDisplayNavigation.h"
#include "IDisplayPolicy.h"
#include "DisplayRenderer.h"
#include "NotificationManager.h"

class INotifier;
class IPeripheryProvider;

class DisplayManager_ : public IButtonHandler, public IMatrixHost, public IDisplayControl, public IDisplayNavigation
{
  private:
    DisplayManager_() = default;
    DisplayManager_(const DisplayManager_&) = delete;
    DisplayManager_& operator=(const DisplayManager_&) = delete;
    DisplayManager_(DisplayManager_&&) = delete;
    DisplayManager_& operator=(DisplayManager_&&) = delete;

    INotifier *notifier_ = nullptr;
    IPeripheryProvider *periphery_ = nullptr;
    bool (*isMenuActive_)() = nullptr;

    DisplayRenderer_ renderer_;
    NotificationManager_ notifMgr_;

    std::vector<IDisplayPolicy *> policies_;
    IDisplayPolicy *activePolicy_ = nullptr; ///< Cached first-active policy (nullptr == none)
    bool policyDirty_ = false;               ///< Force override re-application on next tick

  public:
    static DisplayManager_& getInstance();
    bool appIsSwitching; ///< True while an app transition animation is in progress
    bool showGif;        ///< Flag to enable GIF frame rendering in the current app

    /// Access the renderer sub-component (implements IDisplayRenderer).
    DisplayRenderer_& getRenderer()
    {
        return renderer_;
    }
    /// Access the notification sub-component (implements IDisplayNotifier).
    NotificationManager_& getNotifier()
    {
        return notifMgr_;
    }

    // ── Lifecycle (DisplayManager.cpp) ─────────────────────────────
    void setup();                                    ///< Initialize hardware, NeoMatrix, UI framework
    void tick();                                     ///< Main loop: dispatches to artnet/moodlight/UI
    void clearMatrix();                              ///< Clear + show
    void applyAllSettings() override;                ///< Apply all persisted settings to display/UI
    void setBrightness(int bri) override;            ///< Set brightness (respects matrixOff and wakeup)
    void setPower(bool state) override;              ///< Turn display on/off with sleep animation
    void powerStateParse(const char *json) override; ///< Parse power command from JSON or "true"/"1"
    void showSleepAnimation();                       ///< Play the "Z" sleep animation
    void gammaCorrection() override;                 ///< Apply gamma + optional mirror to LED buffer
    void checkNewYear();                             ///< New Year midnight easter egg

    // ── Navigation & buttons (DisplayManager.cpp) ──────────────────
    void leftButton() override;                   ///< Navigate to previous app
    void rightButton() override;                  ///< Navigate to next app
    void selectButton() override;                 ///< Dismiss current notification
    void selectButtonLong() override;             ///< Long press handler (currently unused)
    void nextApp() override;                      ///< Switch to next app (with debug log)
    void previousApp() override;                  ///< Switch to previous app (with debug log)
    void forceNextApp();                          ///< Force immediate app switch + MQTT publish
    bool setAutoTransition(bool active) override; ///< Enable/disable auto app cycling
    void setAppTime(long duration);               ///< Override time per app (ms)
    void setMatrixLayout(int layout);             ///< Recreate matrix with new pixel wiring layout

    // ── Custom apps (DisplayManager_CustomApps.cpp) ────────────────
    bool parseCustomPage(const String& name, const char *json, bool preventSave) override;
    bool generateCustomPage(const String& name, JsonObject doc, bool preventSave);
    void loadCustomApps();
    void loadNativeApps() override;
    bool switchToApp(const char *json) override;
    void updateAppVector(const char *json) override;
    void reorderApps(const String& jsonString) override;
    String getAppsAsJson() override;
    String getAppsWithIcon() override;
    void sendAppLoop() override;
    void setCustomAppColors(uint32_t color) override;

    // ── Settings & stats (DisplayManager_Settings.cpp) ─────────────
    String getSettings() override;
    void setNewSettings(const char *json) override;
    String getStats() override;
    String ledsAsJson() override;
    String getEffectNames() override;
    String getTransitionNames() override;
    CRGB *getLeds() override;

    // ── Artnet & Moodlight (DisplayManager_Artnet.cpp) ─────────────
    void startArtnet();
    bool moodlight(const char *json) override;

    // ── Dependency injection & calibration ──────────────────────────
    void setNotifier(INotifier *n);
    void setPeriphery(IPeripheryProvider *p);
    void setMenuActiveQuery(bool (*cb)());
    void subscribeViaMqtt(const char *topic);
    void setColorCorrection(CRGB c);
    void setColorTemperature(CRGB t);
    void setCurrentApp(const String& name);
    const String& getCurrentApp() const;

    /// Register a display policy. Order of registration defines priority —
    /// the first active policy wins on resolveTextColor / brightness clamp.
    /// Pointer must outlive DisplayManager.
    void registerPolicy(IDisplayPolicy *policy);

    /// @return The text color to draw with, respecting any active policy's
    ///         override. Falls back to `preferred` when no policy overrides.
    [[nodiscard]] uint32_t resolveTextColor(uint32_t preferred) const;

    /// Request that the next tick re-applies active-policy overrides even
    /// if the active-policy identity hasn't changed. Call after mutating
    /// a field a policy reads (e.g. appConfig.nightColor) so global state
    /// set on the original activation edge (matrix brightness, default
    /// text color) picks up the new value.
    void markPolicyConfigDirty();

    // ═══════════════════════════════════════════════════════════════
    // Rendering delegation — backward compat for apps/overlays that
    // call DisplayManager.method() directly. Consumers should use
    // IDisplayRenderer* via getRenderer() instead.
    // ═══════════════════════════════════════════════════════════════
    void clear()
    {
        renderer_.clear();
    }
    void show()
    {
        renderer_.show();
    }
    void resetTextColor()
    {
        renderer_.resetTextColor();
    }
    void setTextColor(uint32_t color)
    {
        renderer_.setTextColor(color);
    }
    void printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase)
    {
        renderer_.printText(x, y, text, centered, textCase);
    }
    void HSVtext(int16_t x, int16_t y, const char *text, bool clear, byte textCase)
    {
        renderer_.HSVtext(x, y, text, clear, textCase);
    }
    void drawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
    {
        renderer_.drawFilledRect(x, y, w, h, color);
    }
    void drawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h)
    {
        renderer_.drawRGBBitmap(x, y, bitmap, w, h);
    }
    void drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h)
    {
        renderer_.drawBMP(x, y, bitmap, w, h);
    }
    void drawProgressBar(int16_t x, int16_t y, int progress, uint32_t pColor, uint32_t pbColor)
    {
        renderer_.drawProgressBar(x, y, progress, pColor, pbColor);
    }
    void drawMenuIndicator(int cur, int total, uint32_t color)
    {
        renderer_.drawMenuIndicator(cur, total, color);
    }
    void GradientText(int16_t x, int16_t y, const char *text, int color1, int color2, bool clear, byte textCase)
    {
        renderer_.GradientText(x, y, text, color1, color2, clear, textCase);
    }
    void renderColoredText(int16_t x, int16_t y, const char *text, bool rainbow, int gradient0, int gradient1, uint32_t color, uint32_t fade, uint32_t blink, byte textCase)
    {
        renderer_.renderColoredText(x, y, text, rainbow, gradient0, gradient1, color, fade, blink, textCase);
    }
    void setCursor(int16_t x, int16_t y)
    {
        renderer_.setCursor(x, y);
    }
    void matrixPrint(const char *str)
    {
        renderer_.matrixPrint(str);
    }
    void matrixPrint(String str)
    {
        renderer_.matrixPrint(str);
    }
    void matrixPrint(char *str)
    {
        renderer_.matrixPrint(str);
    }
    void matrixPrint(char str[], size_t length)
    {
        renderer_.matrixPrint(str, length);
    }
    void matrixPrint(double number, uint8_t digits)
    {
        renderer_.matrixPrint(number, digits);
    }
    void drawPixel(int16_t x0, int16_t y0, uint32_t color)
    {
        renderer_.drawPixel(x0, y0, color);
    }
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
    {
        renderer_.drawLine(x0, y0, x1, y1, color);
    }
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
    {
        renderer_.drawRect(x, y, w, h, color);
    }
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color)
    {
        renderer_.drawCircle(x0, y0, r, color);
    }
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color)
    {
        renderer_.fillCircle(x0, y0, r, color);
    }
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint32_t color)
    {
        renderer_.drawFastVLine(x, y, h, color);
    }
    void drawJPG(uint16_t x, uint16_t y, fs::File jpgFile)
    {
        renderer_.drawJPG(x, y, jpgFile);
    }
    void drawJPG(int32_t x, int32_t y, const uint8_t jpeg_data[], uint32_t data_size)
    {
        renderer_.drawJPG(x, y, jpeg_data, data_size);
    }
    void drawBarChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color, uint32_t barBG)
    {
        renderer_.drawBarChart(x, y, data, dataSize, withIcon, color, barBG);
    }
    void drawLineChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color)
    {
        renderer_.drawLineChart(x, y, data, dataSize, withIcon, color);
    }
    void processDrawInstructions(int16_t x, int16_t y, String& drawInstructions)
    {
        renderer_.processDrawInstructions(x, y, drawInstructions);
    }
    CRGB getPixelColor(int16_t x, int16_t y)
    {
        return renderer_.getPixelColor(x, y);
    }

    // ═══════════════════════════════════════════════════════════════
    // Notification delegation — backward compat for internal DM calls.
    // Consumers should use IDisplayNotifier* via getNotifier() instead.
    // ═══════════════════════════════════════════════════════════════
    bool generateNotification(uint8_t source, const char *json)
    {
        return notifMgr_.generateNotification(source, json);
    }
    void dismissNotify()
    {
        notifMgr_.dismissNotify();
    }
    bool indicatorParser(uint8_t indicator, const char *json)
    {
        return notifMgr_.indicatorParser(indicator, json);
    }
    void setIndicator1Color(uint32_t color)
    {
        notifMgr_.setIndicator1Color(color);
    }
    void setIndicator1State(bool state)
    {
        notifMgr_.setIndicator1State(state);
    }
    void setIndicator2Color(uint32_t color)
    {
        notifMgr_.setIndicator2Color(color);
    }
    void setIndicator2State(bool state)
    {
        notifMgr_.setIndicator2State(state);
    }
    void setIndicator3Color(uint32_t color)
    {
        notifMgr_.setIndicator3Color(color);
    }
    void setIndicator3State(bool state)
    {
        notifMgr_.setIndicator3State(state);
    }
};

extern DisplayManager_& DisplayManager;
