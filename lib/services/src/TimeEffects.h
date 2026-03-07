#pragma once

/**
 * @file TimeEffects.h
 * @brief Time-based color animation effects (fade, blink).
 *
 * All functions are pure — they accept currentMillis as a parameter
 * instead of calling millis(), making them deterministically testable.
 * Colors are packed uint32_t in 0xRRGGBB format.
 *
 * Extracted from: DisplayManager.cpp (fadeColor, textEffect).
 * Used by: DisplayManager (text rendering with fade/blink animations).
 * Tests: test/test_native/test_effects/
 */

#include <cstdint>

/**
 * Apply a sine-wave brightness fade to a color.
 *
 * Modulates all RGB channels by a sine phase: phase = (sin(2π·t/interval)+1)/2,
 * producing a smooth pulse from 0% to 100% brightness over one period.
 *   - At currentMillis=0: phase=0.5 (half brightness).
 *   - At interval/4: phase=1.0 (full brightness).
 *   - At interval/2: phase=0.5 (half brightness).
 *   - At 3·interval/4: phase=0.0 (black).
 *
 * @param color        Base color as packed 0xRRGGBB.
 * @param interval     Full cycle period in milliseconds.
 * @param currentMillis Current time in milliseconds.
 * @return Faded color as packed 0xRRGGBB.
 */
uint32_t fadeColorAt(uint32_t color, uint32_t interval, uint32_t currentMillis);

/**
 * Apply either a fade or blink effect to a text color.
 *
 * Priority: fade > blink > no effect.
 *   - If fade > 0: applies sine-wave fade (same as fadeColorAt) with period=fade.
 *   - Else if blink > 0: toggles between color and black. ON for the first half
 *     of each blink period, OFF for the second half.
 *   - Else: returns color unchanged.
 *
 * @param color        Base text color as packed 0xRRGGBB.
 * @param fade         Fade period in ms (0 = no fade).
 * @param blink        Blink period in ms (0 = no blink). Ignored if fade > 0.
 * @param currentMillis Current time in milliseconds.
 * @return Effected color as packed 0xRRGGBB, or 0x000000 (black) when off.
 */
uint32_t textEffectAt(uint32_t color, uint32_t fade, uint32_t blink, uint32_t currentMillis);

