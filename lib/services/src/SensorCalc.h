#pragma once

/**
 * @file SensorCalc.h
 * @brief Pure sensor calculation functions — no hardware reads.
 *
 * Each function takes raw sensor values and configuration as parameters,
 * returning computed results. The actual hardware reads (ADC, I2C) remain
 * in PeripheryManager — only the math is here.
 *
 * Extracted from: PeripheryManager.cpp (battery, LDR, temp/hum, volume).
 * Used by: PeripheryManager (sensor processing loop).
 * Tests: test/test_native/test_sensor_calc/
 */

#include <cstdint>
#include <cmath>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * Convert a filtered battery ADC reading to a percentage [0..100].
 *
 * Uses Arduino map() for linear interpolation, then clamps to [0..100].
 * Firmware defaults: minBattery=475 (empty), maxBattery=665 (full).
 *
 * @param filteredAdc  Smoothed ADC value from the battery voltage divider.
 * @param minBattery   ADC value corresponding to 0% (empty battery).
 * @param maxBattery   ADC value corresponding to 100% (full battery).
 * @return Battery percentage, clamped to [0..100].
 */
uint8_t calculateBatteryPercent(uint16_t filteredAdc, uint16_t minBattery, uint16_t maxBattery);

/**
 * Convert a raw LDR (light sensor) reading to a display brightness level.
 *
 * Pipeline: ldrRaw × ldrFactor → normalize to [0..100] → gamma correction
 * via pow(x, ldrGamma) → map to [minBrightness..maxBrightness].
 * Higher gamma values make the curve steeper (more sensitivity in dark).
 *
 * Firmware defaults: ldrFactor=1.0, ldrGamma=3.0, minBrightness=2, maxBrightness=160.
 *
 * @param ldrRaw        Raw 10-bit ADC reading [0..1023].
 * @param ldrFactor     Multiplier for sensitivity tuning (typically 1.0).
 * @param ldrGamma      Gamma exponent for non-linear response (1.0 = linear).
 * @param minBrightness Output floor (prevents display going fully dark).
 * @param maxBrightness Output ceiling (prevents overdriving LEDs).
 * @return Brightness value in [minBrightness..maxBrightness].
 */
uint8_t calculateBrightness(uint16_t ldrRaw, float ldrFactor, float ldrGamma,
                            uint8_t minBrightness, uint8_t maxBrightness);

/**
 * Conditionally invert the LDR reading for ground-wired sensors.
 *
 * Some LDR circuits wire the sensor to ground (voltage drops as light increases),
 * requiring inversion: result = 1023 - ldrRaw.
 *
 * @param ldrRaw     Raw 10-bit ADC reading [0..1023].
 * @param ldrOnGround If true, invert the reading. If false, pass through unchanged.
 * @return Corrected LDR value [0..1023].
 */
uint16_t applyLdrInversion(uint16_t ldrRaw, bool ldrOnGround);

/**
 * Apply a calibration offset to a sensor reading.
 *
 * Used for temperature and humidity sensor calibration:
 *   correctedTemp = applySensorOffset(rawTemp, TEMP_OFFSET);
 *   correctedHum  = applySensorOffset(rawHum, HUM_OFFSET);
 *
 * @param rawValue  Raw sensor reading (°C or %RH).
 * @param offset    Calibration offset (positive = add, negative = subtract).
 * @return Adjusted value: rawValue + offset.
 */
float applySensorOffset(float rawValue, float offset);

/**
 * Scale a volume level from one range to another.
 *
 * Linear scaling: result = vol × outputMax / inputMax.
 * Returns 0 if inputMax is 0 (division-by-zero guard).
 *
 * Firmware usage: scaleVolume(dfPlayerVol, 30, 255) — maps DFPlayer's
 * 0-30 range to the 0-255 range used by the volume display bar.
 *
 * @param vol       Input volume level.
 * @param inputMax  Maximum of the input range.
 * @param outputMax Maximum of the output range.
 * @return Scaled volume in [0..outputMax].
 */
uint8_t scaleVolume(uint8_t vol, uint8_t inputMax, uint8_t outputMax);

