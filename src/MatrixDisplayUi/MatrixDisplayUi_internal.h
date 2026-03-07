/**
 * @file MatrixDisplayUi_internal.h
 * @brief Shared internal state for MatrixDisplayUi modules.
 *
 * This header declares extern references to globals defined in
 * MatrixDisplayUi.cpp and forward-declares free helper functions
 * from MatrixDisplayUi_Transitions.cpp.
 *
 * Only included by MatrixDisplayUi_*.cpp files — NOT part of the public API.
 */
#pragma once

#include "MatrixDisplayUi.h"
#include "GifPlayer.h"

// ── Shared globals (defined in MatrixDisplayUi.cpp) ─────────────────
extern GifPlayer gif1;            ///< GIF player for the current (outgoing) app
extern GifPlayer gif2;            ///< GIF player for the next (incoming) app during transitions
extern uint8_t currentTransition; ///< Active TransitionType value for the current/next transition
extern bool gotNewTransition;     ///< True after a new random transition has been selected for the next cycle
extern bool swapped;              ///< True when the FIXED app frame has been rendered (transition complete)

// ── Free helper functions (defined in MatrixDisplayUi_Transitions.cpp) ──

/// Select a random TransitionType (excludes RANDOM itself).
TransitionType getRandomTransition();

/// Rotate point (x, y) around the matrix center (16, 4) by the given angle in radians.
void rotate(int& x, int& y, float angle);
