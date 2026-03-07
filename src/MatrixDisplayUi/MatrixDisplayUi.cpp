/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 by Daniel Eichhorn
 * Copyright (c) 2016 by Fabrice Weinberg
 * Copyright (c) 2023 by Stephan Muehl (Blueforcer)
 * Note: This old lib for SSD1306 displays has been extremely
 * modified for SVITRIX and has nothing to do with the original purposes.
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
 * @file MatrixDisplayUi.cpp
 * @brief Core implementation: constructor, state machine, update loop, app management.
 */
#include "MatrixDisplayUi.h"
#include "MatrixDisplayUi_internal.h"
#include "effects.h"
#include "effects/NeoMatrixCanvas.h"
#include "Globals.h"

// ── Global state (shared via MatrixDisplayUi_internal.h) ────────────
GifPlayer gif1;
GifPlayer gif2;
uint8_t currentTransition;
bool gotNewTransition = true;
bool swapped = false;

// ── Constructor / Destructor ────────────────────────────────────────

MatrixDisplayUi::MatrixDisplayUi(FastLED_NeoMatrix *matrix, IMatrixHost *host)
    : matrix(matrix)
    , host_(host)
{
}

MatrixDisplayUi::~MatrixDisplayUi()
{
}

// ── Initialization ──────────────────────────────────────────────────

void MatrixDisplayUi::init()
{
    this->matrix->begin();
    this->matrix->setTextWrap(false);
    this->matrix->setBrightness(70);
    gif1.setMatrix(this->matrix);
    gif2.setMatrix(this->matrix);
}

// ── Configuration ───────────────────────────────────────────────────

void MatrixDisplayUi::setTargetFPS(uint8_t fps)
{
    float oldInterval = this->updateInterval;
    this->updateInterval = ((float)1.0 / (float)fps) * 1000;

    // Rescale transition ticks to maintain the same real-time duration at the new FPS
    float changeRatio = oldInterval / (float)this->updateInterval;
    this->ticksPerTransition *= changeRatio;
}

void MatrixDisplayUi::setBackgroundEffect(int effect)
{
    this->BackgroundEffect = effect;
}

void MatrixDisplayUi::setGlobalOverlay(OverlayEffect e)
{
    globalOverlay_ = e;
}
OverlayEffect MatrixDisplayUi::getGlobalOverlay() const
{
    return globalOverlay_;
}

// ── Automatic control ───────────────────────────────────────────────

void MatrixDisplayUi::enablesetAutoTransition()
{
    this->setAutoTransition = true;
}

void MatrixDisplayUi::disablesetAutoTransition()
{
    this->setAutoTransition = false;
}

void MatrixDisplayUi::setsetAutoTransitionForwards()
{
    this->state.appTransitionDirection = 1;
    this->lastTransitionDirection = 1;
}

void MatrixDisplayUi::setsetAutoTransitionBackwards()
{
    this->state.appTransitionDirection = -1;
    this->lastTransitionDirection = -1;
}

void MatrixDisplayUi::setTimePerApp(long time)
{
    this->ticksPerApp = time / updateInterval;
}

void MatrixDisplayUi::setTimePerTransition(uint16_t time)
{
    this->ticksPerTransition = (int)((float)time / (float)updateInterval);
}

// ── App settings ────────────────────────────────────────────────────

void MatrixDisplayUi::setAppAnimation(AnimationDirection dir)
{
    this->appAnimationDirection = dir;
}

void MatrixDisplayUi::setApps(const std::vector<std::pair<String, AppCallback>>& appPairs)
{
    AppFunctions.clear();
    AppFunctions.reserve(appPairs.size());
    AppCount = appPairs.size();
    for (size_t i = 0; i < AppCount; ++i)
    {
        AppFunctions.push_back(appPairs[i].second);
    }
    this->resetState();
    host_->sendAppLoop();
    host_->setAutoTransition(true);
}

// ── Overlays & Background ───────────────────────────────────────────

void MatrixDisplayUi::setOverlays(OverlayCallback *overlayFunctions, uint8_t overlayCount)
{
    this->overlayFunctions = overlayFunctions;
    this->overlayCount = overlayCount;
}

void MatrixDisplayUi::setBackground(BackgroundCallback backgroundFunction)
{
    this->backgroundFunction = backgroundFunction;
}

// ── Manual control ──────────────────────────────────────────────────

void MatrixDisplayUi::nextApp()
{
    if (this->state.appState != IN_TRANSITION)
    {
        this->state.manualControl = true;
        this->state.appState = IN_TRANSITION;
        this->state.ticksSinceLastStateSwitch = 0;
        this->lastTransitionDirection = this->state.appTransitionDirection;
        this->state.appTransitionDirection = 1;
    }
}

void MatrixDisplayUi::previousApp()
{
    if (this->state.appState != IN_TRANSITION)
    {
        this->state.manualControl = true;
        this->state.appState = IN_TRANSITION;
        this->state.ticksSinceLastStateSwitch = 0;
        this->lastTransitionDirection = this->state.appTransitionDirection;
        this->state.appTransitionDirection = -1;
    }
}

bool MatrixDisplayUi::switchToApp(uint8_t app)
{
    if (app >= this->AppCount)
        return false;
    this->state.ticksSinceLastStateSwitch = 0;
    if (app == this->state.currentApp)
        return false;
    this->state.appState = FIXED;
    this->state.currentApp = app;
    return true;
}

void MatrixDisplayUi::transitionToApp(uint8_t app)
{
    if (app >= this->AppCount)
        return;
    this->state.ticksSinceLastStateSwitch = 0;
    if (app == this->state.currentApp)
        return;
    this->nextAppNumber = app;
    this->lastTransitionDirection = this->state.appTransitionDirection;
    this->state.manualControl = true;
    this->state.appState = IN_TRANSITION;
    this->state.appTransitionDirection = app < this->state.currentApp ? -1 : 1;
}

// ── State information ───────────────────────────────────────────────

MatrixDisplayUiState *MatrixDisplayUi::getUiState()
{
    return &this->state;
}

// ── Main update loop ────────────────────────────────────────────────

/// Called from the main loop. Checks if enough time has passed since the last
/// frame, applies frame-skipping if behind schedule, and triggers tick().
/// @return Remaining time budget in ms (negative means the frame took longer than allowed)
int16_t MatrixDisplayUi::update()
{
    unsigned long appStart = millis();
    int16_t timeBudget = this->updateInterval - (appStart - this->state.lastUpdate);
    if (timeBudget <= 0)
    {
        // Skip ticks proportional to overrun to keep transition timing consistent
        if (this->setAutoTransition && this->state.lastUpdate != 0)
            this->state.ticksSinceLastStateSwitch += ceil(-timeBudget / this->updateInterval);

        this->state.lastUpdate = appStart;
        this->tick();
    }

    return static_cast<int16_t>(this->updateInterval - (millis() - appStart));
}

/// Core state machine. Runs once per frame:
///   1. Advance tick counter
///   2. Handle state transitions (IN_TRANSITION -> FIXED, FIXED -> IN_TRANSITION)
///   3. Clear matrix and render layers: background -> app -> overlays -> indicators -> global overlay
///   4. Apply gamma correction and push to LEDs
void MatrixDisplayUi::tick()
{
    this->state.ticksSinceLastStateSwitch++;

    if (this->AppCount > 0)
    {
        switch (this->state.appState)
        {
        case IN_TRANSITION:
            if (this->state.ticksSinceLastStateSwitch >= this->ticksPerTransition)
            {
                this->state.appState = FIXED;
                this->state.currentApp = getnextAppNumber();
                this->state.ticksSinceLastStateSwitch = 0;
                this->nextAppNumber = -1;
            }
            break;
        case FIXED:
            if (this->state.manualControl)
            {
                this->state.appTransitionDirection = 1;
                this->state.manualControl = false;
            }
            if (this->state.ticksSinceLastStateSwitch >= this->ticksPerApp)
            {
                if (this->setAutoTransition)
                {
                    this->state.appState = IN_TRANSITION;
                }
                this->state.ticksSinceLastStateSwitch = 0;
            }
            break;
        }
    }

    this->matrix->clear();

    if (BackgroundEffect > -1)
    {
        NeoMatrixCanvas canvas(this->matrix);
        callEffect(canvas, 0, 0, BackgroundEffect);
    }

    if (this->AppCount > 0)
        this->drawApp();
    this->drawOverlays();
    this->drawIndicators();
    if (globalOverlay_ > 0)
    {
        NeoMatrixCanvas canvas(matrix);
        EffectOverlay(canvas, 0, 0, globalOverlay_);
    }
    host_->gammaCorrection();
    this->matrix->show();
}

// ── Drawing ─────────────────────────────────────────────────────────

/// Dispatches rendering to the appropriate transition effect (IN_TRANSITION)
/// or directly invokes the current app callback (FIXED).
/// In FIXED state, also selects the transition type for the next cycle.
void MatrixDisplayUi::drawApp()
{
    switch (this->state.appState)
    {
    case IN_TRANSITION:
    {
        swapped = false;
        gotNewTransition = false;
        if (currentTransition == SLIDE)
        {
            slideTransition();
        }
        else if (currentTransition == FADE)
        {
            fadeTransition();
        }
        else if (currentTransition == ZOOM)
        {
            zoomTransition();
        }
        else if (currentTransition == ROTATE)
        {
            rotateTransition();
        }
        else if (currentTransition == PIXELATE)
        {
            pixelateTransition();
        }
        else if (currentTransition == CURTAIN)
        {
            curtainTransition();
        }
        else if (currentTransition == RIPPLE)
        {
            rippleTransition();
        }
        else if (currentTransition == BLINK)
        {
            blinkTransition();
        }
        else if (currentTransition == RELOAD)
        {
            reloadTransition();
        }
        else if (currentTransition == CROSSFADE)
        {
            crossfadeTransition();
        }
        break;
    }
    case FIXED:
        if (appConfig.transEffect == RANDOM)
        {
            if (gotNewTransition == false)
            {
                currentTransition = getRandomTransition();
                gotNewTransition = true;
            }
        }
        else
        {
            currentTransition = appConfig.transEffect;
        }

        (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
        swapped = true;
        break;
    }
}

/// Invoke all registered overlay callbacks in order.
void MatrixDisplayUi::drawOverlays()
{
    for (uint8_t i = 0; i < this->overlayCount; i++)
    {
        (this->overlayFunctions[i])(this->matrix, &this->state, &gif2);
    }
}

// ── State management ────────────────────────────────────────────────

/// Return the next app index. Uses nextAppNumber if a directed transition
/// was requested, otherwise wraps around based on appTransitionDirection.
uint8_t MatrixDisplayUi::getnextAppNumber()
{
    if (this->nextAppNumber != -1)
        return this->nextAppNumber;
    return (this->state.currentApp + this->AppCount + this->state.appTransitionDirection) % this->AppCount;
}

/// Check whether the current app index is still valid after apps were replaced.
bool MatrixDisplayUi::isCurrentAppValid()
{
    return !AppFunctions.empty() && this->state.currentApp < AppCount;
}

/// Reset to app 0 only if the current app index became invalid
/// (e.g. after setApps() reduced the app count).
void MatrixDisplayUi::resetState()
{
    if (!isCurrentAppValid())
    {
        this->state.lastUpdate = 0;
        this->state.ticksSinceLastStateSwitch = 0;
        this->state.appState = FIXED;
        this->state.currentApp = 0;
    }
}

/// Unconditionally reset to app 0. Used after settings changes or external events.
void MatrixDisplayUi::forceResetState()
{
    this->state.lastUpdate = 0;
    this->state.ticksSinceLastStateSwitch = 0;
    this->state.appState = FIXED;
    this->state.currentApp = 0;
}
