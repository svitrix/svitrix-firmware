/**
 * @file MatrixDisplayUi_Transitions.cpp
 * @brief Transition effects for switching between apps.
 *
 * Contains all 10 transition animations and their helper functions.
 * Each transition receives the current progress (0.0 – 1.0) via
 * state.ticksSinceLastStateSwitch / ticksPerTransition and blends
 * between the outgoing app (rendered with gif1) and the incoming
 * app (rendered with gif2).
 */
#include "MatrixDisplayUi.h"
#include "MatrixDisplayUi_internal.h"

// ── Helper functions ────────────────────────────────────────────────

/// Rotate point (x, y) around the matrix center (16, 4) by the given angle.
/// Modifies x and y in place.
void rotate(int& x, int& y, float angle)
{
    x -= 16;
    y -= 4;

    int newX = x * cos(angle) - y * sin(angle);
    int newY = x * sin(angle) + y * cos(angle);

    x = newX + 16;
    y = newY + 4;
}

/// Pick a random TransitionType, excluding RANDOM (index 0).
TransitionType getRandomTransition()
{
    return static_cast<TransitionType>((rand() % (CROSSFADE)) + 1);
}

// ── Transitions ─────────────────────────────────────────────────────

/// Fade to black, then fade in the new app. Uses quadratic easing.
void MatrixDisplayUi::fadeTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
    int fadeValue;
    if (progress < 0.5)
    {
        fadeValue = pow(progress * 2, 2) * 255;
    }
    else
    {
        fadeValue = pow((1.0 - progress) * 2, 2) * 255;
    }
    this->matrix->clear();
    if (progress < 0.5)
    {
        (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
    }
    else
    {
        (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
    }

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            CRGB color = host_->getLeds()[this->matrix->XY(i, j)];
            color.fadeToBlackBy(fadeValue);
            host_->getLeds()[this->matrix->XY(i, j)] = color;
        }
    }
}

/// Slide the old app out and the new app in vertically (up or down).
/// Direction is inverted when navigating backward.
void MatrixDisplayUi::slideTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
    int16_t x, y, x1, y1;
    switch (this->appAnimationDirection)
    {
    case SLIDE_UP:
        x = 0;
        y = -8 * progress;
        x1 = 0;
        y1 = y + 8;
        break;
    case SLIDE_DOWN:
        x = 0;
        y = 8 * progress;
        x1 = 0;
        y1 = y - 8;
        break;
    }
    int8_t dir = this->state.appTransitionDirection >= 0 ? 1 : -1;
    x *= dir;
    y *= dir;
    x1 *= dir;
    y1 *= dir;
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, x, y, &gif1);
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, x1, y1, &gif2);
}

/// Open a curtain from the center outward, revealing the new app underneath.
/// The old app frame is captured on the first tick and used as a static overlay.
void MatrixDisplayUi::curtainTransition()
{
    CRGB *leds = host_->getLeds();
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
    int curtainWidth = (int)(16 * progress);

    // Capture the old app frame on the first tick
    if (this->state.ticksSinceLastStateSwitch == 1 || this->state.ticksSinceLastStateSwitch == 0)
    {
        (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                ledsCopy[i + j * 32] = leds[this->matrix->XY(i, j)];
            }
        }
    }
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

    // Keep old-app pixels outside the revealed center strip
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if ((i < (16 - curtainWidth)) || (i >= (16 + curtainWidth)))
            {
                leds[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
            }
        }
    }
}

/// Zoom the old app down to nothing, then zoom the new app up from nothing.
/// Pixels are scaled toward/from the matrix center (16, 4).
void MatrixDisplayUi::zoomTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
    float scale = 1.0;
    if (progress < 0.5)
    {
        scale = 1 - progress * 2;
        (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
    }
    else
    {
        scale = (progress - 0.5) * 2;
        (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
    }

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ledsCopy[i + j * 32] = host_->getLeds()[this->matrix->XY(i, j)];
        }
    }

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int iScaled = 16 + (i - 16) * scale;
            int jScaled = 4 + (j - 4) * scale;

            if (iScaled < 0)
                iScaled = 0;
            if (iScaled >= 32)
                iScaled = 31;
            if (jScaled < 0)
                jScaled = 0;
            if (jScaled >= 8)
                jScaled = 7;
            host_->getLeds()[this->matrix->XY(i, j)] = ledsCopy[iScaled + jScaled * 32];
        }
    }
}

/// Spin the display 360 degrees, switching from old to new app at the halfway point.
void MatrixDisplayUi::rotateTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
    float angle = progress * 2 * PI;

    if (progress < 0.5)
    {
        (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
    }
    else
    {
        (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
    }

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ledsCopy[i + j * 32] = host_->getLeds()[this->matrix->XY(i, j)];
        }
    }

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int iRotated = i;
            int jRotated = j;
            rotate(iRotated, jRotated, angle);

            if (iRotated < 0)
                iRotated = 0;
            if (iRotated >= 32)
                iRotated = 31;
            if (jRotated < 0)
                jRotated = 0;
            if (jRotated >= 8)
                jRotated = 7;

            host_->getLeds()[this->matrix->XY(i, j)] = ledsCopy[iRotated + jRotated * 32];
        }
    }
}

/// Replace pixels randomly: each pixel independently flips from old to new
/// with probability proportional to progress.
void MatrixDisplayUi::pixelateTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ledsCopy[i + j * 32] = host_->getLeds()[this->matrix->XY(i, j)];
        }
    }

    this->matrix->clear();
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (random(255) > progress * 255)
            {
                host_->getLeds()[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
            }
        }
    }
}

/// Checkerboard dissolve: even-positioned pixels show the old app in the first half,
/// odd-positioned pixels show the old app in the second half.
void MatrixDisplayUi::rippleTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ledsCopy[i + j * 32] = host_->getLeds()[this->matrix->XY(i, j)];
        }
    }

    this->matrix->clear();
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if ((i + j) % 2 == 0 && progress < 0.5)
            {
                host_->getLeds()[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
            }
            else if ((i + j) % 2 != 0 && progress >= 0.5)
            {
                host_->getLeds()[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
            }
        }
    }
}

/// Flash between old and new app with 3 blinks. The screen goes black
/// between flashes, switching from old to new app at the halfway point.
void MatrixDisplayUi::blinkTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

    int blinks = 3;
    bool blinkState = (int)(progress * blinks) % 2 == 0;

    if (blinkState)
    {
        if (progress < 0.5)
        {
            (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
        }
        else
        {
            (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
        }
    }
    else
    {
        this->matrix->clear();
    }
}

/// Wipe the old app off-screen to the right, then wipe the new app in from the left.
/// Creates a "page reload" visual effect.
void MatrixDisplayUi::reloadTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
    int visiblePixel;

    if (progress < 0.5)
    {
        (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);

        visiblePixel = 32 * (1.0 - (progress * 2));
        if (visiblePixel < 0)
            visiblePixel = 0;

        for (int i = visiblePixel; i < 32; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                host_->getLeds()[this->matrix->XY(i, j)] = CRGB::Black;
            }
        }
    }
    else
    {
        (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

        visiblePixel = 32 * ((progress - 0.5) * 2);
        if (visiblePixel > 32)
            visiblePixel = 32;

        for (int i = visiblePixel; i < 32; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                host_->getLeds()[this->matrix->XY(i, j)] = CRGB::Black;
            }
        }
    }
}

/// Blend old and new app pixel-by-pixel using linear interpolation (lerp).
/// Both apps are rendered simultaneously and mixed based on progress.
void MatrixDisplayUi::crossfadeTransition()
{
    float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ledsCopy[i + j * 32] = host_->getLeds()[this->matrix->XY(i, j)];
        }
    }

    this->matrix->fillScreen(0);

    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            CRGB pixelOld = ledsCopy[i + j * 32];
            CRGB pixelNew = host_->getLeds()[this->matrix->XY(i, j)];
            host_->getLeds()[this->matrix->XY(i, j)] = pixelOld.lerp8(pixelNew, progress * 255);
        }
    }
}
