#include "Overlays.h"
#include "MatrixDisplayUi.h"
#include "DisplayManager.h"
#include "Functions.h"
#include "MenuManager.h"
#include "PeripheryManager.h"
#include <WiFi.h>
#include "effects.h"
#include "effects/NeoMatrixCanvas.h"
#include "MQTTManager.h"

std::deque<Notification> notifications;
bool notifyFlag = false;

void StatusOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
{
    if (!WiFi.isConnected())
    {
        matrix->drawPixel(0, 0, fadeColor(0xFF0000, 2000));
    }
    if (!MQTTManager.isConnected())
    {
        matrix->drawPixel(0, 7, fadeColor(0xFFFF00, 2000));
    }
}

void MenuOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
{
    if (!MenuManager.inMenu)
        return;


    matrix->fillScreen(0);
    DisplayManager.setTextColor(0xFFFFFF);
    DisplayManager.printText(0, 6, MenuManager.menutext().c_str(), true, 2);
}

void NotifyOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
{
    // Check if notification flag is set
    if (notifications.empty())
    {
        notifyFlag = false;
        return;
    }
    else
    {
        notifyFlag = true;
    }

    if (notifications[0].wakeup && displayConfig.matrixOff)
    {
        DisplayManager.setBrightness(brightnessConfig.brightness);
    }

    // Check if notification duration has expired or if repeat count is 0 and hold is not enabled
    if ((((millis() - notifications[0].startime >= notifications[0].duration) && notifications[0].repeat == -1) || notifications[0].repeat == 0) && !notifications[0].hold)
    {
        DEBUG_PRINTLN(F("Notification deleted"));
        PeripheryManager.stopSound();
        if (notifications.size() >= 2)
        {
            notifications[1].startime = millis();
        }
        notifications[0].icon.close();
        notifications.pop_front();

        if (!notifications.empty() && notifications[0].wakeup && displayConfig.matrixOff)
        {
            DisplayManager.setBrightness(0);
        }

        if (notifications.empty())
        {
            notifyFlag = false;
            if (appConfig.autoTransition)
                DisplayManager.forceNextApp();
        }

        return;
    }

    DisplayManager.setCurrentApp(F("Notification"));

    bool hasIcon = notifications[0].icon || !notifications[0].jpegDataBuffer.empty();

    // Background + effect
    DisplayManager.drawFilledRect(0, 0, 32, 8, notifications[0].background);
    if (notifications[0].effect > -1)
    {
        NeoMatrixCanvas canvas(matrix);
        callEffect(canvas, 0, 0, notifications[0].effect);
    }

    // Calculate text width
    uint16_t textWidth = 0;
    if (!notifications[0].fragments.empty())
    {
        for (const auto& fragment : notifications[0].fragments)
        {
            textWidth += getTextWidth(fragment.c_str(), notifications[0].textCase);
        }
    }
    else
    {
        textWidth = getTextWidth(notifications[0].text.c_str(), notifications[0].textCase);
    }

    uint16_t availableWidth = hasIcon ? 24 : 32;
    bool noScrolling = (textWidth <= availableWidth);

    // Icon + charts + progress (top text layer)
    if (notifications[0].topText)
        renderAppIcon(notifications[0], gifPlayer, 0, 0, noScrolling, hasIcon, nullptr);

    // Scroll reset (notification-specific repeat countdown)
    if (textWidth > availableWidth && notifications[0].scrollposition + notifications[0].textOffset <= (-textWidth))
    {
        notifications[0].scrollDelay = 0;
        notifications[0].scrollposition = 9 + notifications[0].textOffset;

        if (notifications[0].pushIcon == 2)
            notifications[0].iconWasPushed = false;

        if (notifications[0].repeat > 0)
        {
            --notifications[0].repeat;
            if (notifications[0].repeat == 0)
                return;
        }
    }

    // Scroll animation
    if (!noScrolling)
        updateScrollAnimation(notifications[0], hasIcon, displayConfig.matrixFps * 1.2, true);

    // Disable repeat for non-scrolling text
    if (noScrolling)
        notifications[0].repeat = -1;

    // Text rendering (hsvOnZeroColor=true: color 0 fragments use HSV rainbow)
    renderAppText(notifications[0], 0, 0, textWidth, availableWidth,
                  notifications[0].text, notifications[0].fragments, true);

    // Icon + charts + progress (bottom text layer)
    if (!notifications[0].topText)
        renderAppIcon(notifications[0], gifPlayer, 0, 0, noScrolling, hasIcon, nullptr);

    // Sound playback (notification-only)
    if (!notifications[0].soundPlayed || notifications[0].loopSound)
    {
        if (!PeripheryManager.isPlaying())
        {
            if (notifications[0].sound != "" || (displayConfig.matrixOff && notifications[0].wakeup))
                PeripheryManager.playFromFile(notifications[0].sound);

            if (notifications[0].rtttl != "")
                PeripheryManager.playRTTTLString(notifications[0].rtttl);
        }
        notifications[0].soundPlayed = true;
    }

    renderAppOverlay(notifications[0], matrix, 0, 0);
    DisplayManager.resetTextColor();
}

OverlayCallback overlays[] = {MenuOverlay, NotifyOverlay, StatusOverlay};
