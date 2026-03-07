#include "TimeEffects.h"
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// Sine-wave brightness modulation.
// Phase oscillates between 0.0 (black) and 1.0 (full color) over one `interval` period.
// Formula: phase = (sin(2π · t / interval) + 1) / 2
// Each RGB channel is multiplied by phase independently.
uint32_t fadeColorAt(uint32_t color, uint32_t interval, uint32_t currentMillis)
{
    // Guard: interval=0 causes division by zero → sin(Inf) → NaN → UB on uint8_t cast.
    if (interval == 0)
        return color;
    float phase = (sin(2 * PI * currentMillis / float(interval)) + 1) * 0.5;
    uint8_t r = ((color >> 16) & 0xFF) * phase;
    uint8_t g = ((color >> 8) & 0xFF) * phase;
    uint8_t b = (color & 0xFF) * phase;
    return (r << 16) | (g << 8) | b;
}

// Combined text animation effect dispatcher.
// Priority order: fade (sine-wave) > blink (square-wave) > static color.
// Only one effect is active at a time — fade takes precedence over blink.
uint32_t textEffectAt(uint32_t color, uint32_t fade, uint32_t blink, uint32_t currentMillis)
{
    // Fade effect: smooth sine-wave modulation with period = `fade` ms.
    if (fade > 0)
    {
        float phase = (sin(2 * PI * currentMillis / float(fade)) + 1) * 0.5;
        uint8_t r = ((color >> 16) & 0xFF) * phase;
        uint8_t g = ((color >> 8) & 0xFF) * phase;
        uint8_t b = (color & 0xFF) * phase;
        return (r << 16) | (g << 8) | b;
    }
    // Blink effect: square-wave toggle. First half of period = OFF, second half = ON.
    // Guard: blink < 2 causes blink/2=0 via integer division → always OFF.
    else if (blink >= 2)
    {
        if (currentMillis % blink > blink / 2)
        {
            return color; // ON phase
        }
        else
        {
            return 0; // OFF phase (black)
        }
    }
    // No effect: return color unchanged.
    else
    {
        return color;
    }
}
