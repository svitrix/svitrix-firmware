#pragma once

/**
 * @file GammaUtils.h
 * @brief LED gamma correction calculation — no hardware dependencies.
 *
 * Extracted from: DisplayManager.cpp (gammaCorrection).
 * Used by: DisplayManager (LED gamma correction before FastLED show).
 * Tests: test/test_native/test_gamma/
 *
 * The gamma exponent controls how brightness maps to perceived luminance.
 * Low gamma (~0.5) compresses highlights; high gamma (~2.3) compresses shadows.
 * The firmware uses a two-segment logarithmic curve (via logMap) so that
 * the gamma rises quickly at low brightness and flattens at high brightness.
 */

#include "MathUtils.h"

/**
 * Calculate the gamma exponent for LED gamma correction.
 *
 * Maps brightness through logMap(brightness, 2, 180, 0.535, 2.3, 1.9):
 *   brightness  2   → gamma ~0.535 (low brightness, gentle correction)
 *   brightness 90   → gamma ~1.9   (midpoint, moderate correction)
 *   brightness 180  → gamma ~2.3   (high brightness, strong correction)
 *
 * Values below 2 return 0.535 (out_min). Values above 180 return 2.3 (out_max).
 *
 * @param brightness Actual LED brightness level (firmware range: 2–180).
 * @return Gamma exponent, always in [0.535..2.3].
 */
float calculateGamma(float brightness);

