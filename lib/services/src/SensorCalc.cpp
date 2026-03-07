#include "SensorCalc.h"

// Linear map from ADC range to [0..100], then clamp.
// Uses Arduino's map() macro: map(x, in_min, in_max, out_min, out_max).
uint8_t calculateBatteryPercent(uint16_t filteredAdc, uint16_t minBattery, uint16_t maxBattery)
{
    // Guard: equal range would cause division by zero in map() macro.
    if (minBattery >= maxBattery)
        return 0;
    int percent = map(filteredAdc, minBattery, maxBattery, 0, 100);
    return max(min(percent, 100), 0);
}

// Three-stage pipeline:
//   1. Normalize: ldrRaw × ldrFactor / 1023 × 100 → percentage [0..100]
//   2. Gamma correction: pow(percent, gamma) / pow(100, gamma-1)
//      gamma=1.0 → linear; gamma=3.0 → steep curve (more sensitivity in dark)
//   3. Map corrected percentage to [minBrightness..maxBrightness] output range.
uint8_t calculateBrightness(uint16_t ldrRaw, float ldrFactor, float ldrGamma,
                            uint8_t minBrightness, uint8_t maxBrightness)
{
    float brightnessPercent = (ldrRaw * ldrFactor) / 1023.0 * 100.0;
    // Clamp before gamma: ldrFactor > 1.0 can push percentage above 100,
    // which after pow() causes overflow in map() → garbage uint8_t result.
    if (brightnessPercent < 0.0f) brightnessPercent = 0.0f;
    if (brightnessPercent > 100.0f) brightnessPercent = 100.0f;
    brightnessPercent = pow(brightnessPercent, ldrGamma) / pow(100.0, ldrGamma - 1);
    return map((int)brightnessPercent, 0, 100, minBrightness, maxBrightness);
}

// Invert 10-bit ADC reading for ground-wired LDR circuits.
// On ground-wired LDRs, voltage drops as light increases → flip to get rising value.
uint16_t applyLdrInversion(uint16_t ldrRaw, bool ldrOnGround)
{
    if (ldrOnGround)
        return 1023 - ldrRaw;
    return ldrRaw;
}

// Simple additive calibration offset for temperature or humidity sensors.
float applySensorOffset(float rawValue, float offset)
{
    return rawValue + offset;
}

// Linear volume scaling with integer math to avoid floating-point.
// Uses uint16_t intermediate to prevent overflow: vol(0-30) × 255 fits in 16 bits.
uint8_t scaleVolume(uint8_t vol, uint8_t inputMax, uint8_t outputMax)
{
    if (inputMax == 0)
        return 0;
    return (uint8_t)((uint16_t)vol * outputMax / inputMax);
}
