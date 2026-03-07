#ifndef MOCK_FASTLED_H
#define MOCK_FASTLED_H

#include <cstdint>
#include <cmath>
#include <cstdlib>

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
struct CHSV;

// ---------------------------------------------------------------------------
// CRGB
// ---------------------------------------------------------------------------
struct CRGB {
    union {
        struct {
            uint8_t b;
            uint8_t g;
            uint8_t r;
        };
        uint8_t raw[3];
    };

    CRGB() : b(0), g(0), r(0) {}
    CRGB(uint8_t r, uint8_t g, uint8_t b) : b(b), g(g), r(r) {}
    CRGB(uint32_t colorcode)
        : b(colorcode & 0xFF)
        , g((colorcode >> 8) & 0xFF)
        , r((colorcode >> 16) & 0xFF) {}

    // Construct from CHSV (defined after CHSV)
    explicit CRGB(const CHSV &hsv);

    void setRGB(uint8_t nr, uint8_t ng, uint8_t nb) {
        r = nr; g = ng; b = nb;
    }

    bool operator==(const CRGB &rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }
    bool operator!=(const CRGB &rhs) const { return !(*this == rhs); }

    operator bool() const { return r || g || b; }

    CRGB &operator+=(const CRGB &rhs) {
        uint16_t tr = r + rhs.r; r = (tr > 255) ? 255 : tr;
        uint16_t tg = g + rhs.g; g = (tg > 255) ? 255 : tg;
        uint16_t tb = b + rhs.b; b = (tb > 255) ? 255 : tb;
        return *this;
    }

    void fadeToBlackBy(uint8_t fadeBy) {
        r = (uint8_t)((uint16_t)r * (256 - fadeBy) >> 8);
        g = (uint8_t)((uint16_t)g * (256 - fadeBy) >> 8);
        b = (uint8_t)((uint16_t)b * (256 - fadeBy) >> 8);
    }

    CRGB nscale8_video(uint8_t scaledown) const {
        CRGB out;
        out.r = (uint8_t)(((uint16_t)r * (uint16_t)scaledown) >> 8);
        out.g = (uint8_t)(((uint16_t)g * (uint16_t)scaledown) >> 8);
        out.b = (uint8_t)(((uint16_t)b * (uint16_t)scaledown) >> 8);
        return out;
    }

    // Named colors
    static const CRGB Black;
    static const CRGB White;
};

// Static member definitions — weak linkage for safe multi-TU inclusion
__attribute__((weak)) const CRGB CRGB::Black(0, 0, 0);
__attribute__((weak)) const CRGB CRGB::White(255, 255, 255);

// ---------------------------------------------------------------------------
// CHSV
// ---------------------------------------------------------------------------
struct CHSV {
    uint8_t h;
    uint8_t s;
    uint8_t v;

    CHSV() : h(0), s(0), v(0) {}
    CHSV(uint8_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {}
};

// ---------------------------------------------------------------------------
// HSV to RGB conversion
// ---------------------------------------------------------------------------
inline void hsv2rgb_spectrum(const CHSV &hsv, CRGB &rgb) {
    if (hsv.s == 0) {
        rgb.r = rgb.g = rgb.b = hsv.v;
        return;
    }

    uint8_t region = hsv.h / 43;
    uint8_t remainder = (hsv.h - (region * 43)) * 6;

    uint8_t p = (hsv.v * (255 - hsv.s)) >> 8;
    uint8_t q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    uint8_t t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0: rgb.r = hsv.v; rgb.g = t; rgb.b = p; break;
    case 1: rgb.r = q; rgb.g = hsv.v; rgb.b = p; break;
    case 2: rgb.r = p; rgb.g = hsv.v; rgb.b = t; break;
    case 3: rgb.r = p; rgb.g = q; rgb.b = hsv.v; break;
    case 4: rgb.r = t; rgb.g = p; rgb.b = hsv.v; break;
    default: rgb.r = hsv.v; rgb.g = p; rgb.b = q; break;
    }
}

// CRGB(CHSV) constructor — must be after CHSV definition
inline CRGB::CRGB(const CHSV &hsv) : b(0), g(0), r(0) {
    hsv2rgb_spectrum(hsv, *this);
}

// ---------------------------------------------------------------------------
// Palette types
// ---------------------------------------------------------------------------
enum TBlendType { NOBLEND = 0, LINEARBLEND = 1 };

struct CRGBPalette16 {
    CRGB entries[16];

    CRGBPalette16() {}

    CRGB &operator[](int i) { return entries[i & 15]; }
    const CRGB &operator[](int i) const { return entries[i & 15]; }
};

// Simplified ColorFromPalette — index-based lookup, no interpolation
inline CRGB ColorFromPalette(const CRGBPalette16 &pal, uint8_t index,
                              uint8_t brightness = 255, TBlendType = LINEARBLEND) {
    uint8_t palIndex = index >> 4; // 0-15
    CRGB c = pal[palIndex];
    if (brightness < 255) {
        c.r = (uint8_t)(((uint16_t)c.r * brightness) >> 8);
        c.g = (uint8_t)(((uint16_t)c.g * brightness) >> 8);
        c.b = (uint8_t)(((uint16_t)c.b * brightness) >> 8);
    }
    return c;
}

// Predefined palettes — simple stubs with distinct colors
inline CRGBPalette16 _makePalette(uint8_t baseHue) {
    CRGBPalette16 p;
    for (int i = 0; i < 16; i++) {
        CHSV hsv(baseHue + i * 16, 240, 255);
        hsv2rgb_spectrum(hsv, p.entries[i]);
    }
    return p;
}

static CRGBPalette16 RainbowColors_p = _makePalette(0);
static CRGBPalette16 OceanColors_p = _makePalette(128);
static CRGBPalette16 ForestColors_p = _makePalette(96);
static CRGBPalette16 LavaColors_p = _makePalette(0);
static CRGBPalette16 CloudColors_p = _makePalette(160);
static CRGBPalette16 PartyColors_p = _makePalette(64);
static CRGBPalette16 HeatColors_p = _makePalette(0);
static CRGBPalette16 RainbowStripeColors_p = _makePalette(0);

// ---------------------------------------------------------------------------
// FastLED math functions
// ---------------------------------------------------------------------------
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline uint8_t sin8(uint8_t theta) {
    return (uint8_t)(128.0 + 127.0 * sin(theta * 2.0 * M_PI / 256.0));
}

inline int16_t sin16(uint16_t theta) {
    return (int16_t)(32767.0 * sin(theta * 2.0 * M_PI / 65536.0));
}

inline uint8_t cos8(uint8_t theta) {
    return sin8(theta + 64);
}

inline uint8_t inoise8(uint8_t x, uint8_t y, uint8_t z) {
    // Simplified noise stub — returns deterministic pseudo-noise
    return (uint8_t)((x * 73 + y * 157 + z * 37) & 0xFF);
}

// ---------------------------------------------------------------------------
// Random number functions
// ---------------------------------------------------------------------------
inline uint8_t random8() { return (uint8_t)(rand() & 0xFF); }
inline uint8_t random8(uint8_t lim) { return lim ? (uint8_t)(rand() % lim) : 0; }
inline uint8_t random8(uint8_t mn, uint8_t mx) {
    return (mx > mn) ? (mn + (uint8_t)(rand() % (mx - mn))) : mn;
}
inline uint16_t random16() { return (uint16_t)(rand() & 0xFFFF); }

#endif // MOCK_FASTLED_H
