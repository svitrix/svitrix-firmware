#pragma once

#include <vector>
#include <FS.h>
#include "OverlayMapping.h"
#include "LayoutEngine.h"

class FastLED_NeoMatrix;
class GifPlayer;

/// Base struct with rendering fields shared by Notification and CustomApp.
/// Both structs inherit from this to enable shared rendering functions.
struct AppContentBase
{
    bool center = true;
    String drawInstructions;
    float scrollposition = 0;
    int16_t scrollDelay = 0;
    String text;
    uint32_t color = 0;
    File icon;
    bool isGif = false;
    bool rainbow = false;
    int fade = 0;
    int blink = 0;
    int iconOffset = 0;
    byte textCase = 0;
    byte pushIcon = 0;
    float iconPosition = 0;
    bool iconWasPushed = false;
    int barData[16] = {0};
    int lineData[16] = {0};
    int barSize = 0;
    uint32_t barBG = 0;
    int lineSize = 0;
    std::vector<uint32_t> colors;
    std::vector<String> fragments;
    int textOffset = 0;
    int progress = -1;
    uint32_t pColor = 0x00FF00;
    int effect = -1;
    uint32_t background = 0;
    uint32_t pbColor = 0xFFFFFF;
    float scrollSpeed = 100;
    bool topText = true;
    bool noScrolling = true;
    OverlayEffect overlay = NONE;
    IconLayout layout = IconLayout::Left;
    int gradient[2] = {0};
    std::vector<uint8_t> jpegDataBuffer;
};

// ── Shared rendering functions (implemented in AppContentRenderer.cpp) ──

/// Render icon (GIF/JPEG) with push animation, separator line,
/// progress bar, draw instructions, bar/line charts.
/// @param currentFrame  If non-null, passes frame to playGif and updates it.
///                      Pass nullptr for notifications (no frame tracking).
/// @return icon width (8 if icon present, undefined if no icon).
int renderAppIcon(AppContentBase& app, GifPlayer *gifPlayer,
                  int16_t x, int16_t y, bool noScrolling, bool hasIcon,
                  uint8_t *currentFrame);

/// Apply scroll speed to scrollposition. Handles scroll delay
/// and initial position reset.
/// @param delayThreshold  Scroll starts after this many ticks
///                        (matrixFps*1.2 for notifications, matrixFps for apps).
/// @param canScroll       Whether scrolling is allowed this frame
///                        (always true for notifications, appState==FIXED for apps).
void updateScrollAnimation(AppContentBase& app, bool hasIcon,
                           float delayThreshold, bool canScroll);

/// Calculate text X position (centered or left-aligned).
int16_t calculateTextX(const AppContentBase& app,
                       uint16_t textWidth, bool hasIcon);

/// Render text (fragments or solid) in both scrolling and non-scrolling modes.
/// @param resolvedText       Pre-resolved text (with placeholders replaced).
/// @param resolvedFragments  Pre-resolved fragments (empty if no fragments).
/// @param hsvOnZeroColor     If true, fragment color==0 uses HSVtext (notifications).
///                           If false, color==0 falls back to app.color (custom apps).
void renderAppText(AppContentBase& app, int16_t x, int16_t y,
                   uint16_t textWidth, uint16_t availableWidth,
                   const String& resolvedText,
                   const std::vector<String>& resolvedFragments,
                   bool hsvOnZeroColor);

/// Render weather overlay effect if active.
void renderAppOverlay(const AppContentBase& app,
                      FastLED_NeoMatrix *matrix, int16_t x, int16_t y);
