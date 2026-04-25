#ifndef RENDER_EFFECT_FASTLED_H
#define RENDER_EFFECT_FASTLED_H
// Prevent test/mocks/FastLED.h from being re-processed when MockPixelCanvas.h
// later does `#include "FastLED.h"` and resolves it via its own directory.
#define MOCK_FASTLED_H

// Fork of test/mocks/FastLED.h tuned for the render_effect tool.
// Differences from the test mock:
//   * Predefined palettes use real FastLED color values (so HeatColors_p
//     looks like fire instead of a generic rainbow stub).
//   * ColorFromPalette interpolates between adjacent entries on
//     LINEARBLEND, matching firmware behaviour on the device.
// All other surface area is identical to the test mock.

#include <cstdint>
#include <cmath>
#include <cstdlib>

struct CHSV;

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
    CRGB(uint8_t r_, uint8_t g_, uint8_t b_) : b(b_), g(g_), r(r_) {}
    CRGB(uint32_t colorcode)
        : b(colorcode & 0xFF)
        , g((colorcode >> 8) & 0xFF)
        , r((colorcode >> 16) & 0xFF) {}

    explicit CRGB(const CHSV &hsv);

    void setRGB(uint8_t nr, uint8_t ng, uint8_t nb) { r = nr; g = ng; b = nb; }
    bool operator==(const CRGB &rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b; }
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

    static const CRGB Black;
    static const CRGB White;
};

__attribute__((weak)) const CRGB CRGB::Black(0, 0, 0);
__attribute__((weak)) const CRGB CRGB::White(255, 255, 255);

struct CHSV {
    uint8_t h, s, v;
    CHSV() : h(0), s(0), v(0) {}
    CHSV(uint8_t h_, uint8_t s_, uint8_t v_) : h(h_), s(s_), v(v_) {}
};

inline void hsv2rgb_spectrum(const CHSV &hsv, CRGB &rgb) {
    if (hsv.s == 0) { rgb.r = rgb.g = rgb.b = hsv.v; return; }
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

inline CRGB::CRGB(const CHSV &hsv) : b(0), g(0), r(0) { hsv2rgb_spectrum(hsv, *this); }

enum TBlendType { NOBLEND = 0, LINEARBLEND = 1 };

struct CRGBPalette16 {
    CRGB entries[16];
    CRGBPalette16() {}
    CRGBPalette16(const uint32_t (&hex)[16]) {
        for (int i = 0; i < 16; i++) entries[i] = CRGB(hex[i]);
    }
    CRGB &operator[](int i) { return entries[i & 15]; }
    const CRGB &operator[](int i) const { return entries[i & 15]; }
};

// Real ColorFromPalette: linear interpolation between adjacent palette entries.
inline CRGB ColorFromPalette(const CRGBPalette16 &pal, uint8_t index,
                              uint8_t brightness = 255, TBlendType blend = LINEARBLEND) {
    uint8_t hi = index >> 4;
    uint8_t lo = index & 0x0F;
    CRGB a = pal[hi];
    CRGB out = a;
    if (blend == LINEARBLEND && lo != 0) {
        CRGB b = pal[(hi + 1) & 15];
        uint8_t f1 = lo;
        uint8_t f2 = 16 - lo;
        out.r = (uint8_t)(((uint16_t)a.r * f2 + (uint16_t)b.r * f1) >> 4);
        out.g = (uint8_t)(((uint16_t)a.g * f2 + (uint16_t)b.g * f1) >> 4);
        out.b = (uint8_t)(((uint16_t)a.b * f2 + (uint16_t)b.b * f1) >> 4);
    }
    if (brightness < 255) {
        out.r = (uint8_t)(((uint16_t)out.r * brightness) >> 8);
        out.g = (uint8_t)(((uint16_t)out.g * brightness) >> 8);
        out.b = (uint8_t)(((uint16_t)out.b * brightness) >> 8);
    }
    return out;
}

// ----- Real FastLED palette tables (lifted from FastLED colorpalettes.cpp) -----
// Definitions kept inline so the single-translation-unit tool builds cleanly.

namespace {
constexpr uint32_t _Heat[16] = {
    0x000000, 0x330000, 0x660000, 0x990000, 0xCC0000, 0xFF0000, 0xFF3300, 0xFF6600,
    0xFF9900, 0xFFCC00, 0xFFFF00, 0xFFFF33, 0xFFFF66, 0xFFFF99, 0xFFFFCC, 0xFFFFFF,
};
constexpr uint32_t _Rainbow[16] = {
    0xFF0000, 0xD52A00, 0xAB5500, 0xAB7F00, 0xABAB00, 0x56D500, 0x00FF00, 0x00D52A,
    0x00AB55, 0x0056AA, 0x0000FF, 0x2A00D5, 0x5500AB, 0x7F0081, 0xAB0055, 0xD5002B,
};
constexpr uint32_t _RainbowStripe[16] = {
    0xFF0000, 0x000000, 0xAB5500, 0x000000, 0xABAB00, 0x000000, 0x00FF00, 0x000000,
    0x00AB55, 0x000000, 0x0000FF, 0x000000, 0x5500AB, 0x000000, 0xAB0055, 0x000000,
};
constexpr uint32_t _Lava[16] = {
    0x000000, 0x800000, 0x000000, 0x800000, 0x8B0000, 0x800000, 0x8B0000, 0x8B0000,
    0x8B0000, 0xFF0000, 0xFFA500, 0xFFFFFF, 0xFFA500, 0xFF0000, 0x8B0000, 0x000000,
};
constexpr uint32_t _Ocean[16] = {
    0x191970, 0x00008B, 0x191970, 0x000080, 0x00008B, 0x0000CD, 0x2E8B57, 0x008080,
    0x5F9EA0, 0x0000FF, 0x008B8B, 0x6495ED, 0x7FFFD4, 0x2E8B57, 0x00FFFF, 0x87CEFA,
};
constexpr uint32_t _Forest[16] = {
    0x006400, 0x006400, 0x556B2F, 0x006400, 0x008000, 0x228B22, 0x6B8E23, 0x008000,
    0x2E8B57, 0x66CDAA, 0x32CD32, 0x9ACD32, 0x90EE90, 0x7CFC00, 0x66CDAA, 0x228B22,
};
constexpr uint32_t _Cloud[16] = {
    0x0000FF, 0x00008B, 0x00008B, 0x00008B, 0x00008B, 0x00008B, 0x00008B, 0x00008B,
    0x0000FF, 0x00008B, 0x87CEEB, 0x87CEEB, 0xADD8E6, 0xFFFFFF, 0xADD8E6, 0x87CEEB,
};
constexpr uint32_t _Party[16] = {
    0x5500AB, 0x84007C, 0xB5004B, 0xE5001B, 0xE81700, 0xB84700, 0xAB7700, 0xABAB00,
    0xAB5500, 0xDD2200, 0xF2000E, 0xC2003E, 0x8F0071, 0x5F00A1, 0x2F00D0, 0x0007F9,
};
} // namespace

static CRGBPalette16 RainbowColors_p(_Rainbow);
static CRGBPalette16 RainbowStripeColors_p(_RainbowStripe);
static CRGBPalette16 OceanColors_p(_Ocean);
static CRGBPalette16 ForestColors_p(_Forest);
static CRGBPalette16 LavaColors_p(_Lava);
static CRGBPalette16 CloudColors_p(_Cloud);
static CRGBPalette16 PartyColors_p(_Party);
static CRGBPalette16 HeatColors_p(_Heat);

// ----- FastLED math + RNG (identical to the test mock) -----

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline uint8_t sin8(uint8_t theta) {
    return (uint8_t)(128.0 + 127.0 * sin(theta * 2.0 * M_PI / 256.0));
}
inline int16_t sin16(uint16_t theta) {
    return (int16_t)(32767.0 * sin(theta * 2.0 * M_PI / 65536.0));
}
inline uint8_t cos8(uint8_t theta) { return sin8(theta + 64); }

// ----- Perlin 3D noise (Ken Perlin reference impl) -----
// FastLED's `inoise8` returns 8-bit Perlin noise sampled in 16-bit input space.
// The mock used to be a linear hash; PlasmaCloud now sees real noise.
namespace {
inline uint8_t _perlin_p(int i) {
    static const uint8_t kPerm[256] = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
        134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
        55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
        18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
        250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
        189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
        228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
        107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
    };
    return kPerm[i & 255];
}
inline float _perlin_fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
inline float _perlin_lerp(float a, float b, float t) { return a + t * (b - a); }
inline float _perlin_grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
inline float _perlin3d(float x, float y, float z) {
    int X = ((int)floor(x)) & 255;
    int Y = ((int)floor(y)) & 255;
    int Z = ((int)floor(z)) & 255;
    x -= floor(x); y -= floor(y); z -= floor(z);
    float u = _perlin_fade(x), v = _perlin_fade(y), w = _perlin_fade(z);
    int A  = _perlin_p(X)     + Y, AA = _perlin_p(A) + Z, AB = _perlin_p(A + 1) + Z;
    int B  = _perlin_p(X + 1) + Y, BA = _perlin_p(B) + Z, BB = _perlin_p(B + 1) + Z;
    return _perlin_lerp(
        _perlin_lerp(
            _perlin_lerp(_perlin_grad(_perlin_p(AA),     x,     y,     z),
                         _perlin_grad(_perlin_p(BA),     x - 1, y,     z), u),
            _perlin_lerp(_perlin_grad(_perlin_p(AB),     x,     y - 1, z),
                         _perlin_grad(_perlin_p(BB),     x - 1, y - 1, z), u), v),
        _perlin_lerp(
            _perlin_lerp(_perlin_grad(_perlin_p(AA + 1), x,     y,     z - 1),
                         _perlin_grad(_perlin_p(BA + 1), x - 1, y,     z - 1), u),
            _perlin_lerp(_perlin_grad(_perlin_p(AB + 1), x,     y - 1, z - 1),
                         _perlin_grad(_perlin_p(BB + 1), x - 1, y - 1, z - 1), u), v),
        w);
}
} // namespace

// FastLED inoise8: 16-bit input space, 8-bit output. We sample Perlin in float
// space scaled to ~256 units per integer step (matches FastLED's ~8 fractional
// bits convention).
inline uint8_t inoise8(uint16_t x, uint16_t y, uint16_t z) {
    float n = _perlin3d(x / 256.0f, y / 256.0f, z / 256.0f); // ~[-0.7..0.7]
    int v = (int)((n + 1.0f) * 127.5f);
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    return (uint8_t)v;
}

// FastLED applyGamma_video: per-channel pow(c/255, gamma)*255, with the "_video"
// guard that any non-zero input stays >=1 after correction. Matches the firmware
// rendering pipeline (DisplayManager::gammaCorrection).
inline uint8_t applyGamma_video(uint8_t v, float gamma) {
    if (v == 0) return 0;
    float adj = pow((float)v / 255.0f, gamma) * 255.0f;
    int out = (int)adj;
    if (out < 1) out = 1;
    if (out > 255) out = 255;
    return (uint8_t)out;
}
inline CRGB applyGamma_video(const CRGB& orig, float gamma) {
    CRGB out;
    out.r = applyGamma_video(orig.r, gamma);
    out.g = applyGamma_video(orig.g, gamma);
    out.b = applyGamma_video(orig.b, gamma);
    return out;
}
inline uint8_t random8() { return (uint8_t)(rand() & 0xFF); }
inline uint8_t random8(uint8_t lim) { return lim ? (uint8_t)(rand() % lim) : 0; }
inline uint8_t random8(uint8_t mn, uint8_t mx) {
    return (mx > mn) ? (mn + (uint8_t)(rand() % (mx - mn))) : mn;
}
inline uint16_t random16() { return (uint16_t)(rand() & 0xFFFF); }

#endif
