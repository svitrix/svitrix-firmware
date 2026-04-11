/**
 * @file DisplayManager_internal.h
 * @brief Shared internal state for DisplayManager modules.
 *
 * This header declares extern references to globals defined in
 * DisplayManager.cpp (LED buffers, matrix, UI) and DisplayManager_Artnet.cpp
 * (artnet timing). It also forward-declares free helper functions from
 * other modules that the core needs.
 *
 * Only included by DisplayManager_*.cpp files — NOT part of the public API.
 */
#pragma once

#include <FastLED_NeoMatrix.h>
#include <ArduinoJson.h>
#include "MatrixDisplayUi.h"
#include "GifPlayer.h"
#include "OverlayMapping.h"
#include <vector>

// ── Matrix dimensions ──────────────────────────────────────────────
// Canonical definitions live in EffectTypes.h (kMatrixWidth/kMatrixHeight).
// These macros exist for backward compat with code using MATRIX_WIDTH/HEIGHT.
#include "EffectTypes.h"
#define MATRIX_WIDTH kMatrixWidth
#define MATRIX_HEIGHT kMatrixHeight

// ── Shared globals (defined in DisplayManager.cpp) ─────────────────
extern CRGB leds[];                ///< Primary LED buffer (MATRIX_WIDTH * MATRIX_HEIGHT)
extern CRGB ledsCopy[];            ///< Pre-gamma copy for color readback
extern FastLED_NeoMatrix *matrix;  ///< NeoMatrix driver (layout set by setMatrixLayout)
extern MatrixDisplayUi *ui;        ///< App framework: transitions, overlays, indicators
extern float actualBri;            ///< Current brightness (0-255), updated by setBrightness
extern int16_t cursor_x, cursor_y; ///< Text cursor position for matrixPrint()
extern uint32_t textColor;         ///< Current text drawing color
// ── Artnet/moodlight state (defined in DisplayManager.cpp) ──────────
extern unsigned long lastArtnetStatusTime; ///< Timestamp of last Artnet packet (for timeout)
extern bool artnetMode;                    ///< True while receiving Artnet frames
extern bool moodlightMode;                 ///< True while moodlight fill is active

// ── Display calibration (defined in DisplayManager.cpp) ─────────────
extern float displayGamma;    ///< Gamma correction exponent (0 = disabled)
extern CRGB colorCorrection;  ///< LED color correction (0 = disabled)
extern CRGB colorTemperature; ///< LED color temperature (0 = disabled)

// ── App tracking (defined in DisplayManager.cpp) ────────────────────
extern String currentApp; ///< Name of the currently displayed app

// ── Free functions from other modules ──────────────────────────────
void ResetCustomApps();                                                          ///< Resets scroll state on non-active custom apps (DisplayManager_CustomApps.cpp)
void checkLifetime(uint8_t pos);                                                 ///< Checks and removes expired custom apps (DisplayManager_CustomApps.cpp)
bool jpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap); ///< TJpg_Decoder callback (DisplayManager_Drawing.cpp)

// --- Generic JSON field helpers (DisplayManager_ParseHelpers.cpp) ---
bool parseCRGBFromJson(JsonObject doc, const char *key, CRGB& target);
void readColorField(JsonObject doc, const char *key, uint32_t& target, uint32_t defaultColor);
void decodeBase64Icon(const String& data, std::vector<uint8_t>& buffer);

// --- Shared JSON parsing helpers (DisplayManager_ParseHelpers.cpp) ---
// Used by both generateCustomPage() and generateNotification()
void parseProgressBar(JsonObject doc, int& progress, uint32_t& pColor, uint32_t& pbColor);
void parseChartData(JsonObject doc, int *barData, int& barSize, int *lineData, int& lineSize, uint32_t& barBG);
void parseGradient(JsonObject doc, int gradient[2]);
void parseTextOrFragments(JsonObject doc, String& text, std::vector<uint32_t>& colors, std::vector<String>& fragments, uint32_t defaultColor);
void parseCommonAppFields(JsonObject doc, int& effect, OverlayEffect& overlay,
                          bool& rainbow, byte& pushIcon, byte& textCase,
                          int& iconOffset, int& textOffset, float& scrollSpeed,
                          bool& topText, int& fade, int& blink, bool& center,
                          bool& noScrolling, int16_t& repeat, String& drawInstructions);
uint32_t parseAppColor(JsonObject doc);
uint32_t parseBackground(JsonObject doc);

// --- Indicator dispatch helpers (DisplayManager_ParseHelpers.cpp) ---
void setIndicatorState(uint8_t indicator, bool state);
void setIndicatorColor(uint8_t indicator, uint32_t color);
void setIndicatorBlink(uint8_t indicator, int blink);
void setIndicatorFade(uint8_t indicator, int fade);
bool getIndicatorState(uint8_t indicator);
uint32_t getIndicatorColor(uint8_t indicator);
