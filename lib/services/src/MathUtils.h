#pragma once

/**
 * @file MathUtils.h
 * @brief Pure math utilities with no hardware dependencies.
 *
 * Extracted from: Functions.cpp (roundToDecimalPlaces), DisplayManager.cpp (logMap).
 * Used by: DisplayManager (gamma correction), Functions.cpp (temperature formatting).
 * Tests: test/test_native/test_math/, test/test_native/test_logmap/
 */

#include <cmath>

/**
 * Round a floating-point value to a given number of decimal places.
 *
 * @param value  The number to round.
 * @param places Number of decimal digits to keep (0 = integer).
 * @return       Rounded value. Example: roundToDecimalPlaces(3.14159, 2) → 3.14
 */
double roundToDecimalPlaces(double value, int places);

/**
 * Logarithmic mapping with a two-segment curve.
 *
 * Maps x from [in_min..in_max] to [out_min..out_max] using a logarithmic
 * curve. The curve is split into two halves at the midpoint of the input
 * range: the lower half scales from out_min toward mid_point_out, and the
 * upper half scales from mid_point_out toward out_max.
 *
 * Values outside [in_min..in_max] are clamped to out_min / out_max.
 * The output is monotonically increasing.
 *
 * @param x             Input value.
 * @param in_min        Lower bound of input range.
 * @param in_max        Upper bound of input range.
 * @param out_min       Output when x == in_min.
 * @param out_max       Output when x == in_max.
 * @param mid_point_out Controls the curve shape — output value at the
 *                      transition point between the two log segments.
 *                      Higher values push the curve up (fast rise then plateau).
 * @return Mapped value in [out_min..out_max].
 *
 * Usage in firmware: gamma correction in DisplayManager::gammaCorrection():
 *   float gamma = logMap(actualBri, 2, 180, 0.535, 2.3, 1.9);
 */
float logMap(float x, float in_min, float in_max, float out_min, float out_max, float mid_point_out);

