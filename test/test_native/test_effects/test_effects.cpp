#include <unity.h>
#include "Arduino.h"
#include "FastLED.h"
#include "MockPixelCanvas.h"

// Include effect implementations directly (build_src_filter = -<*>)
#include "PatternEffects.cpp"
#include "WaveEffects.cpp"
#include "ParticleEffects.cpp"
#include "GameEffects.cpp"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static MockPixelCanvas mock;
static EffectSettings defaultSettings(2, RainbowColors_p, true);

void setUp(void) {
    mock.reset();
    setMockMillis(10000); // well past any initial time thresholds
}

void tearDown(void) {}

// ---------------------------------------------------------------------------
// Group 1: PatternEffects
// ---------------------------------------------------------------------------

void test_checkerboard_fills_all_pixels(void) {
    CheckerboardEffect(mock, 0, 0, &defaultSettings);
    TEST_ASSERT_EQUAL(kMatrixWidth * kMatrixHeight, mock.drawPixelCalls);
}

void test_checkerboard_alternates_colors(void) {
    // Call multiple times to let colorIndex1 and colorIndex2 diverge
    EffectSettings fastSettings(10, RainbowColors_p, true);
    for (int i = 0; i < 5; i++) {
        mock.reset();
        CheckerboardEffect(mock, 0, 0, &fastSettings);
    }
    // After several iterations the two palette indices differ,
    // so even/odd cells must have different colors
    CRGB c00 = mock.getPixel(0, 0);
    CRGB c10 = mock.getPixel(1, 0);
    TEST_ASSERT(c00 != c10);
}

void test_fade_fills_all_pixels(void) {
    Fade(mock, 0, 0, &defaultSettings);
    TEST_ASSERT_EQUAL(kMatrixWidth * kMatrixHeight, mock.drawPixelCalls);
}

void test_fade_horizontal_bands(void) {
    Fade(mock, 0, 0, &defaultSettings);
    // Each row should have the same color (horizontal bands)
    for (uint16_t j = 0; j < kMatrixHeight; j++) {
        CRGB rowColor = mock.getPixel(0, j);
        for (uint16_t i = 1; i < kMatrixWidth; i++) {
            CRGB c = mock.getPixel(i, j);
            TEST_ASSERT_EQUAL(rowColor.r, c.r);
            TEST_ASSERT_EQUAL(rowColor.g, c.g);
            TEST_ASSERT_EQUAL(rowColor.b, c.b);
        }
    }
}

void test_moving_line_draws_row(void) {
    // MovingLine draws one row of kMatrixWidth pixels
    setMockMillis(20000); // ensure time threshold is met
    MovingLine(mock, 0, 0, &defaultSettings);
    TEST_ASSERT_EQUAL(kMatrixWidth, mock.drawPixelCalls);
}

void test_theater_chase_fills_matrix(void) {
    setMockMillis(20000);
    TheaterChase(mock, 0, 0, &defaultSettings);
    // TheaterChase draws every pixel (some lit, some black)
    TEST_ASSERT_EQUAL(kMatrixWidth * kMatrixHeight, mock.drawPixelCalls);
}

// ---------------------------------------------------------------------------
// Group 2: WaveEffects
// ---------------------------------------------------------------------------

void test_plasma_fills_matrix(void) {
    Plasma(mock, 0, 0, &defaultSettings);
    TEST_ASSERT_EQUAL(kMatrixWidth * kMatrixHeight, mock.drawPixelCalls);
}

void test_pacifica_fills_matrix(void) {
    Pacifica(mock, 0, 0, &defaultSettings);
    TEST_ASSERT_EQUAL(kMatrixWidth * kMatrixHeight, mock.drawPixelCalls);
}

// ---------------------------------------------------------------------------
// Group 3: GameEffects
// ---------------------------------------------------------------------------

void test_ping_pong_draws_elements(void) {
    // PingPong draws paddles (3 pixels each) + ball (1 pixel) = 7
    setMockMillis(20000);
    EffectSettings pongSettings(8, RainbowColors_p, true);
    PingPongEffect(mock, 0, 0, &pongSettings);
    // 2 paddles * kPaddleHeight(3) + 1 ball = 7
    TEST_ASSERT_EQUAL(7, mock.drawPixelCalls);
}

void test_brick_breaker_draws_elements(void) {
    setMockMillis(20000);
    EffectSettings brickSettings;
    BrickBreakerEffect(mock, 0, 0, &brickSettings);
    // paddle(3) + ball(1) + bricks(3 rows * 16 cols = 48) = 52
    TEST_ASSERT(mock.drawPixelCalls >= 4); // at least paddle + ball
}

void test_looking_eyes_draws_bitmaps(void) {
    EffectSettings eyeSettings;
    LookingEyes(mock, 0, 0, &eyeSettings);
    TEST_ASSERT(mock.drawBitmapCalls >= 2);
}

void test_looking_eyes_draws_pupils(void) {
    EffectSettings eyeSettings;
    LookingEyes(mock, 0, 0, &eyeSettings);
    TEST_ASSERT(mock.fillRectCalls >= 2);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int, char **) {
    UNITY_BEGIN();

    // PatternEffects
    RUN_TEST(test_checkerboard_fills_all_pixels);
    RUN_TEST(test_checkerboard_alternates_colors);
    RUN_TEST(test_fade_fills_all_pixels);
    RUN_TEST(test_fade_horizontal_bands);
    RUN_TEST(test_moving_line_draws_row);
    RUN_TEST(test_theater_chase_fills_matrix);

    // WaveEffects
    RUN_TEST(test_plasma_fills_matrix);
    RUN_TEST(test_pacifica_fills_matrix);

    // GameEffects
    RUN_TEST(test_ping_pong_draws_elements);
    RUN_TEST(test_brick_breaker_draws_elements);
    RUN_TEST(test_looking_eyes_draws_bitmaps);
    RUN_TEST(test_looking_eyes_draws_pupils);

    return UNITY_END();
}
