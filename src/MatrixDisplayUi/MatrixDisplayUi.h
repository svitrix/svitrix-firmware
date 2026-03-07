/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 by Daniel Eichhorn
 * Copyright (c) 2016 by Fabrice Weinberg
 * Highly modified 2023 for SVITRIX by Stephan Muehl (Blueforcer)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

/**
 * @file MatrixDisplayUi.h
 * @brief App framework for the 32x8 LED matrix.
 *
 * MatrixDisplayUi manages the lifecycle of display apps: registration,
 * cycling with configurable timing, animated transitions between apps,
 * overlay rendering, and status indicator display.
 *
 * Implementation is split across multiple .cpp files in this directory:
 *   MatrixDisplayUi.cpp              — Core: constructor, state machine, update loop, app management
 *   MatrixDisplayUi_Transitions.cpp  — 10 animated transition effects between apps
 *   MatrixDisplayUi_Indicators.cpp   — Status indicator rendering and configuration
 *
 * All modules share internal state via MatrixDisplayUi_internal.h.
 */

#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>
#include "FastLED_NeoMatrix.h"
#include "GifPlayer.h"
#include "IMatrixHost.h"
#include "OverlayMapping.h"

#ifndef DEBUG_MatrixDisplayUi
#define DEBUG_MatrixDisplayUi(...)
#endif

/// Vertical direction for slide transition animation.
enum AnimationDirection
{
    SLIDE_UP,
    SLIDE_DOWN
};

/// Visual effect used when transitioning between apps.
/// RANDOM selects a different effect each time.
enum TransitionType
{
    RANDOM,
    SLIDE,
    FADE,
    ZOOM,
    ROTATE,
    PIXELATE,
    CURTAIN,
    RIPPLE,
    BLINK,
    RELOAD,
    CROSSFADE
};

/// Current phase of the app display cycle.
enum AppState
{
    IN_TRANSITION, ///< Animated transition between two apps is playing
    FIXED          ///< Current app is displayed statically
};

/// Shared state exposed to app and overlay callbacks.
struct MatrixDisplayUiState
{
    u_int64_t lastUpdate = 0;                    ///< Timestamp (millis) of the last frame update
    unsigned long ticksSinceLastStateSwitch = 0; ///< Ticks elapsed since entering current AppState

    AppState appState = FIXED; ///< Current display phase (FIXED or IN_TRANSITION)
    uint8_t currentApp = 0;    ///< Index of the currently displayed app

    int8_t appTransitionDirection = 1; ///< +1 = forward (next), -1 = backward (previous)
    bool lastFrameShown = false;       ///< True after the final frame of a transition
    bool manualControl = false;        ///< True when transition was triggered by user input

    void *userData = NULL; ///< Custom data pointer available to all callbacks
};

/**
 * App drawing callback.
 * Uses std::function to support both raw function pointers (native apps)
 * and lambdas with captures (custom apps that capture their name).
 * @param matrix  NeoMatrix driver for pixel operations
 * @param state   Current UI state (app index, transition progress, etc.)
 * @param x       Horizontal offset (non-zero during slide transitions)
 * @param y       Vertical offset (non-zero during slide transitions)
 * @param gifPlayer  GIF decoder instance for animated icons
 */
using AppCallback = std::function<void(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)>;

/**
 * Overlay drawing callback, rendered on top of the current app each frame.
 * @param matrix     NeoMatrix driver
 * @param state      Current UI state
 * @param gifPlayer  GIF decoder instance
 */
typedef void (*OverlayCallback)(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

/**
 * Background drawing callback, rendered behind all apps.
 * @param matrix  NeoMatrix driver
 */
typedef void (*BackgroundCallback)(FastLED_NeoMatrix *matrix);

/**
 * @brief App framework for the 32x8 LED matrix.
 *
 * Manages a list of display apps with automatic or manual cycling,
 * animated transitions, overlay layers, background effects, and
 * three configurable RGB status indicators.
 *
 * Lifecycle: construct -> init() -> setApps() -> call update() each loop iteration.
 */
class MatrixDisplayUi
{
  private:
    FastLED_NeoMatrix *matrix; ///< NeoMatrix driver (not owned, passed via constructor)
    IMatrixHost *host_;        ///< Display host for LED access, gamma, app lifecycle
    CRGB ledsCopy[256];        ///< Temporary buffer for transition effects (32x8 pixels)

    AnimationDirection appAnimationDirection = SLIDE_DOWN; ///< Slide direction for SLIDE transition
    int8_t lastTransitionDirection = 1;                    ///< Saved direction before manual override

    unsigned long ticksPerApp = 151;  ///< Ticks to display each app (~5000ms at 30 FPS)
    uint16_t ticksPerTransition = 15; ///< Ticks for transition animation (~500ms at 30 FPS)

    bool setAutoTransition = true;         ///< Whether apps cycle automatically
    std::vector<AppCallback> AppFunctions; ///< Registered app callbacks

    int8_t nextAppNumber = -1; ///< Target app index for directed transition, -1 = sequential

    OverlayCallback *overlayFunctions = nullptr;     ///< Array of overlay callbacks (not owned)
    BackgroundCallback backgroundFunction = nullptr; ///< Background effect callback (not owned)
    uint8_t overlayCount = 0;                        ///< Number of registered overlays
    int BackgroundEffect = -1;                       ///< Active background effect index, -1 = none
    OverlayEffect globalOverlay_ = NONE;             ///< Weather overlay effect for all apps
    MatrixDisplayUiState state;                      ///< Current UI state

    unsigned long updateInterval = 33; ///< Milliseconds between frames (1000/FPS)

    // ── Private drawing & state methods ───────────────────────────────
    void drawApp();           ///< Dispatch to current app or transition effect
    void drawOverlays();      ///< Render all registered overlay callbacks
    void tick();              ///< State machine: advance ticks, handle state switches, render frame
    void resetState();        ///< Reset to app 0 if current app is no longer valid
    bool isCurrentAppValid(); ///< Check if currentApp index is within bounds

    // ── Transition effects (MatrixDisplayUi_Transitions.cpp) ──────────
    void slideTransition();
    void fadeTransition();
    void zoomTransition();
    void rotateTransition();
    void pixelateTransition();
    void curtainTransition();
    void rippleTransition();
    void blinkTransition();
    void reloadTransition();
    void crossfadeTransition();

  public:
    MatrixDisplayUi(FastLED_NeoMatrix *matrix, IMatrixHost *host);
    ~MatrixDisplayUi();

    uint8_t AppCount = 0; ///< Number of currently registered apps

    /// Initialize the matrix hardware and GIF players. Call once after construction.
    void init();

    /// Calculate the next app index based on direction or explicit nextAppNumber.
    uint8_t getnextAppNumber();

    /// Set target frames per second. Adjusts update interval and transition tick counts.
    void setTargetFPS(uint8_t fps);

    /// Set background visual effect index. Pass -1 to disable.
    void setBackgroundEffect(int effect);

    /// Set the global weather overlay effect applied to all apps.
    void setGlobalOverlay(OverlayEffect e);

    /// Get the current global weather overlay effect.
    OverlayEffect getGlobalOverlay() const;

    /// Enable automatic cycling to the next app after the configured display time.
    void enablesetAutoTransition();

    /// Disable automatic app cycling. Apps only change via manual control.
    void disablesetAutoTransition();

    /// Set automatic transition direction to forward (next app).
    void setsetAutoTransitionForwards();

    /// Set automatic transition direction to backward (previous app).
    void setsetAutoTransitionBackwards();

    /// Set how long each app is displayed before transitioning (in milliseconds).
    void setTimePerApp(long time);

    /// Set the duration of transition animations (in milliseconds).
    void setTimePerTransition(uint16_t time);

    // ── Indicator configuration (MatrixDisplayUi_Indicators.cpp) ──────

    void setIndicator1Color(uint32_t color); ///< Set indicator 1 RGB color (top-right corner)
    void setIndicator1State(bool state);     ///< Enable/disable indicator 1
    void setIndicator1Blink(int Blink);      ///< Set indicator 1 blink interval in ms, 0 = off
    void setIndicator1Fade(int fade);        ///< Set indicator 1 fade period in ms, 0 = off

    void setIndicator2Color(uint32_t color); ///< Set indicator 2 RGB color (middle-right)
    void setIndicator2State(bool state);     ///< Enable/disable indicator 2
    void setIndicator2Blink(int Blink);      ///< Set indicator 2 blink interval in ms, 0 = off
    void setIndicator2Fade(int fade);        ///< Set indicator 2 fade period in ms, 0 = off

    void setIndicator3Color(uint32_t color); ///< Set indicator 3 RGB color (bottom-right corner)
    void setIndicator3State(bool state);     ///< Enable/disable indicator 3
    void setIndicator3Blink(int Blink);      ///< Set indicator 3 blink interval in ms, 0 = off
    void setIndicator3Fade(int fade);        ///< Set indicator 3 fade period in ms, 0 = off

    /// Render all active indicators onto the matrix buffer.
    void drawIndicators();

    /// Compute a faded color using a sine wave cycle.
    /// @param color     Base RGB color (packed uint32)
    /// @param interval  Full fade cycle period in milliseconds
    /// @return Faded color at the current point in the cycle
    uint32_t fadeColor(uint32_t color, uint32_t interval);

    // ── App settings ──────────────────────────────────────────────────

    /// Set the vertical direction for slide transition animations.
    void setAppAnimation(AnimationDirection dir);

    /// Register the list of apps. Replaces any previously registered apps.
    /// Each pair contains the app name and its drawing callback.
    void setApps(const std::vector<std::pair<String, AppCallback>>& appPairs);

    // ── Overlay & background ──────────────────────────────────────────

    /// Register overlay callbacks drawn on top of apps each frame.
    void setOverlays(OverlayCallback *overlayFunctions, uint8_t overlayCount);

    /// Register a background callback rendered behind all apps.
    void setBackground(BackgroundCallback backgroundfunction);

    /// Unconditionally reset state to app 0 (e.g. after settings change).
    void forceResetState();

    // ── Manual control ────────────────────────────────────────────────

    /// Trigger transition to the next app (forward direction).
    void nextApp();

    /// Trigger transition to the previous app (backward direction).
    void previousApp();

    /// Switch to a specific app immediately without transition animation.
    /// @return false if app index is out of range or already current
    bool switchToApp(uint8_t app);

    /// Transition to a specific app with animation.
    /// Direction is chosen automatically based on relative position.
    void transitionToApp(uint8_t app);

    // ── State & update ────────────────────────────────────────────────

    /// Get a pointer to the current UI state (for external inspection).
    MatrixDisplayUiState *getUiState();

    /// Main loop method. Call each iteration. Renders a frame if enough time has elapsed.
    /// @return Remaining time budget in ms (negative = frame overrun)
    int16_t update();

    // ── Indicator state (public for direct access by DisplayManager) ──
    uint32_t indicator1Color = 0xFF0000; ///< Indicator 1 color (default: red)
    uint32_t indicator2Color = 0x00FF00; ///< Indicator 2 color (default: green)
    uint32_t indicator3Color = 0x0000FF; ///< Indicator 3 color (default: blue)

    bool indicator1State = false; ///< Indicator 1 enabled
    bool indicator2State = false; ///< Indicator 2 enabled
    bool indicator3State = false; ///< Indicator 3 enabled

    int indicator1Blink = 0; ///< Indicator 1 blink interval (ms), 0 = solid
    int indicator2Blink = 0; ///< Indicator 2 blink interval (ms), 0 = solid
    int indicator3Blink = 0; ///< Indicator 3 blink interval (ms), 0 = solid

    int indicator1Fade = 0; ///< Indicator 1 fade period (ms), 0 = solid
    int indicator2Fade = 0; ///< Indicator 2 fade period (ms), 0 = solid
    int indicator3Fade = 0; ///< Indicator 3 fade period (ms), 0 = solid
};
