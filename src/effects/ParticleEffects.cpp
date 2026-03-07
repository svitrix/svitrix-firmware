#include "ParticleEffects.h"

// ---- Twinkling Stars ----

namespace
{

struct Star
{
    CRGB color;
    float brightness;
};

Star stars[kMatrixWidth][kMatrixHeight];

} // namespace

void TwinklingStars(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    static uint32_t lastUpdate = 0;

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            stars[i][j].brightness -= 0.01f;
            if (stars[i][j].brightness < 0)
                stars[i][j].brightness = 0;
            if (stars[i][j].brightness > 0.8f)
                canvas.drawPixel(x + i, y + j,
                                 stars[i][j].color.nscale8_video(stars[i][j].brightness * 255));
        }
    }

    if (millis() - lastUpdate > static_cast<uint32_t>(1000 / settings->speed))
    {
        lastUpdate = millis();
        uint8_t numStars = random(1, 5);
        for (uint8_t n = 0; n < numStars; n++)
        {
            uint16_t sx = random(kMatrixWidth);
            uint16_t sy = random(kMatrixHeight);
            stars[sx][sy].color = ColorFromPalette(settings->palette, random8(), 255, blendMode(settings));
            stars[sx][sy].brightness = 1.0f;
            canvas.drawPixel(x + sx, y + sy, stars[sx][sy].color);
        }
    }
}

// ---- Fireworks ----

namespace
{

struct Firework
{
    float x, y;
    uint8_t life;
    bool exploded;
    CRGB color;
    uint16_t peak;
    float speed;
};

constexpr uint8_t kMaxFireworks = 5;
Firework fireworks[kMaxFireworks];
uint32_t lastFireworkTime = 0;
constexpr uint32_t kFireworkInterval = 350;

} // namespace

void Fireworks(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    if (millis() - lastFireworkTime >= 1000 / kFireworkInterval && random(100) < 50)
    {
        for (auto& fw : fireworks)
        {
            if (fw.life == 0)
            {
                fw.x = random(kMatrixWidth);
                fw.y = kMatrixHeight - 1;
                fw.life = 255;
                fw.exploded = false;
                fw.color = CRGB::White;
                fw.peak = random(1, 5);
                fw.speed = settings->speed * 0.5f;
                lastFireworkTime = millis();
                break;
            }
        }
    }

    for (auto& fw : fireworks)
    {
        if (fw.life == 0)
            continue;

        fw.life -= 5 * settings->speed;

        if (fw.y > fw.peak && !fw.exploded)
        {
            fw.y -= fw.speed;
        }
        else
        {
            fw.exploded = true;
            fw.color = ColorFromPalette(settings->palette, random8(), 255, blendMode(settings));
        }

        canvas.drawPixel(x + fw.x, y + fw.y, fw.color);

        if (fw.exploded)
        {
            if (fw.x > 0)
                canvas.drawPixel(x + fw.x - 1, y + fw.y, fw.color);
            if (fw.x < kMatrixWidth - 1)
                canvas.drawPixel(x + fw.x + 1, y + fw.y, fw.color);
            if (fw.y > 0)
                canvas.drawPixel(x + fw.x, y + fw.y - 1, fw.color);
            if (fw.y < kMatrixHeight - 1)
                canvas.drawPixel(x + fw.x, y + fw.y + 1, fw.color);
        }
    }
}

// ---- Ripple ----

namespace
{

constexpr uint8_t kRippleSize = 30;

struct RippleState
{
    uint16_t x, y;
    float life;
    CRGB color;
};

RippleState ripple;
CRGB tempLedsRipple[kMatrixWidth][kMatrixHeight];

} // namespace

void RippleEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    if (ripple.life > kRippleSize)
    {
        ripple.x = random(kMatrixWidth);
        ripple.y = random(kMatrixHeight);
        ripple.life = 0;
        ripple.color = ColorFromPalette(settings->palette, random(255), 255, blendMode(settings));
    }

    for (uint16_t i = 0; i < kMatrixWidth; i++)
        for (uint16_t j = 0; j < kMatrixHeight; j++)
            tempLedsRipple[i][j].fadeToBlackBy(45);

    for (uint16_t i = 0; i < kMatrixWidth; i++)
    {
        for (uint16_t j = 0; j < kMatrixHeight; j++)
        {
            uint16_t dx = abs(static_cast<int>(i) - static_cast<int>(ripple.x));
            uint16_t dy = abs(static_cast<int>(j) - static_cast<int>(ripple.y));
            auto dist = static_cast<uint16_t>(sqrt(dx * dx + dy * dy));

            if (dist >= ripple.life && dist < ripple.life + 2)
                tempLedsRipple[i][j] = ripple.color;

            canvas.drawPixel(x + i, y + j, tempLedsRipple[i][j]);
        }
    }

    ripple.life += settings->speed * 0.5f;
}

// ---- Matrix ----

namespace
{

uint32_t matrixLastMove = 0;
CRGB matrixLedState[kMatrixWidth][kMatrixHeight];

} // namespace

void Matrix(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    static const CRGB spawnColor(175, 255, 175);
    static const CRGB trailColor(27, 130, 39);
    constexpr uint8_t kSpawnIntensity = 8;

    uint8_t speed = 180 - static_cast<uint8_t>(settings->speed * 15);
    uint8_t fade = map(100, 0, 255, 50, 250);

    if (millis() - matrixLastMove >= speed)
    {
        matrixLastMove = millis();

        // Shift rows down
        for (uint16_t i = 0; i < kMatrixWidth; i++)
            for (uint16_t j = kMatrixHeight - 1; j > 0; j--)
                matrixLedState[i][j] = matrixLedState[i][j - 1];

        // Fade top row and spawn new drops
        for (uint16_t i = 0; i < kMatrixWidth; i++)
        {
            if (matrixLedState[i][0] == spawnColor)
                matrixLedState[i][0] = trailColor;
            else
                matrixLedState[i][0].fadeToBlackBy(fade);

            if (random8() < kSpawnIntensity)
                matrixLedState[i][0] = spawnColor;
        }
    }

    for (uint16_t i = 0; i < kMatrixWidth; i++)
        for (uint16_t j = 0; j < kMatrixHeight; j++)
            canvas.drawPixel(x + i, y + j, matrixLedState[i][j]);
}

// ---- Reset all particle effect state ----

void resetParticleEffectState()
{
    memset(stars, 0, sizeof(stars));
    memset(fireworks, 0, sizeof(fireworks));
    lastFireworkTime = 0;
    ripple = {};
    memset(tempLedsRipple, 0, sizeof(tempLedsRipple));
    matrixLastMove = 0;
    memset(matrixLedState, 0, sizeof(matrixLedState));
}
