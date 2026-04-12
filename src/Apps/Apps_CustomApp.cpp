/**
 * @file Apps_CustomApp.cpp
 * @brief Custom app renderer.
 *
 * Contains ShowCustomApp() which renders user-defined apps with support
 * for icons (JPEG/GIF), scrolling text, multi-fragment colored text,
 * progress bars, bar/line charts, draw instructions, and overlay effects.
 */
#include "Apps_internal.h"
#include "Functions.h"
#include "EffectRegistry.h"
#include "NeoMatrixCanvas.h"
#include "MQTTManager.h"
#include <LittleFS.h>

// ── Custom app renderer ────────────────────────────────────────────

/// Render a custom app by name. Handles the full rendering pipeline:
/// icon loading, text width calculation, scroll position management,
/// fragment/colored text rendering, charts, progress bars, and overlays.
/// @param name      Name of the custom app to render.
/// @param matrix    Pointer to the LED matrix driver.
/// @param state     Current UI state (app state, transition info).
/// @param x, y      Top-left rendering origin.
/// @param gifPlayer GIF decoder for animated icons.
void ShowCustomApp(const String& name, FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;

    CustomApp *ca = getCustomAppByName(name);
    if (ca == nullptr)
        return;

    if (!DisplayManager.appIsSwitching)
    {
        if (ca->duration > 0)
            DisplayManager.setAppTime(ca->duration);
    }

    // Background + effect
    DisplayManager.drawFilledRect(x, y, 32, 8, ca->background);
    if (ca->effect > -1)
    {
        NeoMatrixCanvas canvas(matrix);
        callEffect(canvas, x, y, ca->effect);
    }

    DisplayManager.setCurrentApp(ca->name);
    currentCustomApp = name;

    // Icon loading (lazy open from LittleFS, negative cache via iconNotFound flag)
    if ((ca->iconName.length() > 0) && !ca->icon && !ca->iconNotFound)
    {
        const char *extensions[] = {".jpg", ".gif"};
        bool isGifFlags[] = {false, true};
        bool found = false;

        for (int i = 0; i < 2; i++)
        {
            String filePath = "/ICONS/" + ca->iconName + extensions[i];
            if (LittleFS.exists(filePath))
            {
                ca->isGif = isGifFlags[i];
                ca->icon = LittleFS.open(filePath);
                ca->currentFrame = 0;
                found = true;
                break;
            }
        }
        if (!found)
            ca->iconNotFound = true;
    }

    bool hasIcon = ca->icon || !ca->jpegDataBuffer.empty();

    // Text width calculation — resolve placeholders once and cache results
    uint16_t textWidth = 0;
    String replacedText;
    std::vector<String> resolvedFragments;
    if (!ca->fragments.empty())
    {
        resolvedFragments.reserve(ca->fragments.size());
        for (const auto& fragment : ca->fragments)
        {
            resolvedFragments.push_back(replacePlaceholders(fragment));
            textWidth += getTextWidth(resolvedFragments.back().c_str(), ca->textCase);
        }
    }
    else
    {
        replacedText = replacePlaceholders(ca->text);
        textWidth = getTextWidth(replacedText.c_str(), ca->textCase);
    }

    uint16_t availableWidth = hasIcon ? 24 : 32;
    bool noScrolling = textWidth <= availableWidth;

    // Icon + charts + progress (top text layer)
    if (ca->topText)
        renderAppIcon(*ca, gifPlayer, x, y, noScrolling, hasIcon, &ca->currentFrame);

    // Auto-transition control
    if ((ca->repeat > 0) && (textWidth > availableWidth) && (state->appState == FIXED))
        DisplayManager.setAutoTransition(false);
    else
        DisplayManager.setAutoTransition(true);

    // Scroll reset (custom-app-specific repeat logic with currentRepeat counter)
    if (textWidth > availableWidth && !(state->appState == IN_TRANSITION))
    {
        if (ca->scrollposition + ca->textOffset <= (-textWidth))
        {
            if (ca->iconWasPushed && ca->pushIcon == 2)
                ca->iconWasPushed = false;

            if ((ca->currentRepeat + 1 >= ca->repeat) && (ca->repeat > 0))
            {
                DisplayManager.setAutoTransition(true);
                ca->currentRepeat = 0;
                DisplayManager.nextApp();
                ca->scrollDelay = 0;
                ca->scrollposition = 9 + ca->textOffset;
                return;
            }
            else if (ca->repeat > 0)
            {
                ++ca->currentRepeat;
            }
            ca->scrollDelay = 0;
            ca->scrollposition = 9 + ca->textOffset;
        }
    }

    // Scroll animation
    if (!noScrolling)
        updateScrollAnimation(*ca, hasIcon, displayConfig.matrixFps, state->appState == FIXED);

    // Text rendering (hsvOnZeroColor=false: color 0 fragments use TextEffect)
    renderAppText(*ca, x, y, textWidth, availableWidth,
                  replacedText, resolvedFragments, false);

    // Icon + charts + progress (bottom text layer)
    if (!ca->topText)
        renderAppIcon(*ca, gifPlayer, x, y, noScrolling, hasIcon, &ca->currentFrame);

    // Lifetime border (custom-app only)
    if (ca->lifeTimeEnd)
        DisplayManager.drawRect(x, y, 32, 8, 0x6e0700);

    renderAppOverlay(*ca, matrix, x, y);
    DisplayManager.resetTextColor();
}
