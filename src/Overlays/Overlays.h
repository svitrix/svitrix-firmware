#pragma once

#include <deque>
#include <vector>
#include "MatrixDisplayUi.h"
#include "AppContent.h"

uint32_t TextEffect(uint32_t color, uint32_t fade, uint32_t blink);

struct Notification : AppContentBase
{
    // Override base default: notifications start scrolling from right edge
    Notification()
    {
        scrollposition = 34;
    }

    // Notification-only fields
    bool soundPlayed = false;
    unsigned long startime = 0;
    long duration = 0;
    int16_t repeat = -1;
    bool hold = false;
    bool wakeup = false;
    String sound;
    bool loopSound = false;
    String rtttl;
};


extern std::deque<Notification> notifications;
extern bool notifyFlag;

void StatusOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

void MenuOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

void NotifyOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);


extern OverlayCallback overlays[];
