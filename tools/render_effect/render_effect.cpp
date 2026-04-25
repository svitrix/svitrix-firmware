// render_effect — native CLI that runs a Svitrix visual effect through the
// same MockPixelCanvas the unit tests use, scales each frame, and writes the
// result as an animated GIF89a. No external encoder dependencies.
//
// Build:  make
// Run:    ./render_effect --effect Fire --frames 90 --fps 25 --scale 32 --out Fire.gif

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#define UNIT_TEST 1

#include "Arduino.h"
#include "FastLED.h"
#include "MockPixelCanvas.h"
#include "EffectTypes.h"
#include "ParticleEffects.h"
#include "PatternEffects.h"
#include "WaveEffects.h"
#include "GameEffects.h"
#include "MathUtils.h"
#include "GammaUtils.h"

// Compile effect implementations directly (build_src_filter excludes src/).
#include "PatternEffects.cpp"
#include "WaveEffects.cpp"
#include "ParticleEffects.cpp"
#include "GameEffects.cpp"
// Pull firmware gamma curve from the same source files DisplayManager uses, so
// the tool's gamma stage stays in lock-step with future curve changes.
#include "MathUtils.cpp"
#include "GammaUtils.cpp"

// ---------------------------------------------------------------------------
// Effect lookup — mirror EffectRegistry.cpp without dragging in ArduinoJson.
// ---------------------------------------------------------------------------
struct ToolEffect {
    const char *name;
    EffectFunc func;
    EffectSettings settings;
};

static ToolEffect kEffects[] = {
    {"Fade", Fade, EffectSettings(1, RainbowColors_p, true)},
    {"MovingLine", MovingLine, EffectSettings(1, RainbowColors_p, true)},
    {"BrickBreaker", BrickBreakerEffect, EffectSettings()},
    {"PingPong", PingPongEffect, EffectSettings(8, RainbowColors_p, true)},
    {"Radar", RadarEffect, EffectSettings(1, RainbowColors_p, true)},
    {"Checkerboard", CheckerboardEffect, EffectSettings(1, RainbowColors_p, true)},
    {"Fireworks", Fireworks, EffectSettings(1, RainbowColors_p)},
    {"PlasmaCloud", PlasmaCloudEffect, EffectSettings(3, RainbowColors_p, true)},
    {"Ripple", RippleEffect, EffectSettings(3, RainbowColors_p, true)},
    {"Snake", SnakeGame, EffectSettings(3, RainbowColors_p, true)},
    {"Pacifica", Pacifica, EffectSettings(3, OceanColors_p, true)},
    {"TheaterChase", TheaterChase, EffectSettings(3, RainbowColors_p, true)},
    {"Plasma", Plasma, EffectSettings(2, RainbowColors_p, true)},
    {"Matrix", Matrix, EffectSettings(8, ForestColors_p, false)},
    {"SwirlIn", SwirlIn, EffectSettings(5, RainbowColors_p, true)},
    {"SwirlOut", SwirlOut, EffectSettings(5, RainbowColors_p, true)},
    {"LookingEyes", LookingEyes, EffectSettings()},
    {"TwinklingStars", TwinklingStars, EffectSettings(4, OceanColors_p, true)},
    {"ColorWaves", ColorWaves, EffectSettings(5, RainbowColors_p, true)},
    {"Fire", Fire, EffectSettings(5, HeatColors_p, true)},
};
static constexpr size_t kEffectCount = sizeof(kEffects) / sizeof(kEffects[0]);

// ---------------------------------------------------------------------------
// GIF89a writer with proper LZW compression. Each frame carries its own Local
// Color Table built from up to 256 unique colors observed in that frame.
// ---------------------------------------------------------------------------
class GifWriter {
public:
    GifWriter(const std::string &path, int width, int height, int fps)
        : width_(width), height_(height), fps_(fps) {
        fp_ = std::fopen(path.c_str(), "wb");
        if (!fp_) {
            std::fprintf(stderr, "render_effect: cannot open %s for writing\n", path.c_str());
            std::exit(1);
        }
        writeHeader();
    }

    ~GifWriter() { finalize(); }

    // pixels: width_ * height_ RGB triples, row-major.
    void writeFrame(const uint8_t *pixels) {
        // Build local color table (max 256 unique colors).
        std::unordered_map<uint32_t, uint8_t> color_to_idx;
        std::vector<uint32_t> palette; // packed RGB per index
        palette.reserve(256);

        size_t n = (size_t)width_ * height_;
        std::vector<uint8_t> indices(n);
        for (size_t i = 0; i < n; i++) {
            uint32_t key = ((uint32_t)pixels[3 * i] << 16) |
                           ((uint32_t)pixels[3 * i + 1] << 8) |
                           (uint32_t)pixels[3 * i + 2];
            auto it = color_to_idx.find(key);
            uint8_t idx;
            if (it == color_to_idx.end()) {
                if (palette.size() >= 256) {
                    // Should not happen on a 32x8 source, but degrade gracefully.
                    idx = nearestIndex(palette, key);
                } else {
                    idx = (uint8_t)palette.size();
                    color_to_idx[key] = idx;
                    palette.push_back(key);
                }
            } else {
                idx = it->second;
            }
            indices[i] = idx;
        }

        // Pad palette up to power-of-two (256) so LCT size = 7 bits.
        while (palette.size() < 256) palette.push_back(0);

        writeFrameBlocks(indices, palette);
    }

    void finalize() {
        if (!fp_) return;
        std::fputc(0x3B, fp_); // GIF trailer
        std::fclose(fp_);
        fp_ = nullptr;
    }

private:
    FILE *fp_ = nullptr;
    int width_, height_, fps_;

    void put8(uint8_t v) { std::fputc(v, fp_); }
    void put16(uint16_t v) { put8(v & 0xFF); put8((v >> 8) & 0xFF); }
    void putBytes(const uint8_t *p, size_t n) { std::fwrite(p, 1, n, fp_); }

    void writeHeader() {
        std::fwrite("GIF89a", 1, 6, fp_);
        // Logical Screen Descriptor: no global color table.
        put16((uint16_t)width_);
        put16((uint16_t)height_);
        put8(0x77);  // packed: GCT=0, color_resolution=7, sort=0, GCT_size=7
        put8(0);     // background index
        put8(0);     // pixel aspect ratio

        // Application Extension (NETSCAPE2.0) — infinite loop.
        const uint8_t app[] = {
            0x21, 0xFF, 0x0B,
            'N','E','T','S','C','A','P','E','2','.','0',
            0x03, 0x01, 0x00, 0x00,
            0x00,
        };
        putBytes(app, sizeof(app));
    }

    void writeFrameBlocks(const std::vector<uint8_t> &indices,
                          const std::vector<uint32_t> &palette) {
        // Graphic Control Extension.
        put8(0x21); put8(0xF9); put8(0x04);
        put8(0x04);                                  // dispose=1, no transp, no user input
        uint16_t delay = (uint16_t)((100 + fps_ / 2) / fps_); // centiseconds per frame
        put16(delay);
        put8(0x00);                                  // transparent index
        put8(0x00);                                  // block terminator

        // Image Descriptor.
        put8(0x2C);
        put16(0); put16(0);                          // x, y
        put16((uint16_t)width_);
        put16((uint16_t)height_);
        put8(0x87);                                  // packed: LCT=1, interlace=0, sort=0, size=7

        // Local Color Table — 256 entries × RGB.
        for (uint32_t c : palette) {
            put8((c >> 16) & 0xFF);
            put8((c >> 8) & 0xFF);
            put8(c & 0xFF);
        }

        // LZW-compressed image data.
        put8(0x08); // LZW min code size
        std::vector<uint8_t> lzw = lzwEncode(indices);
        // Split into ≤255-byte sub-blocks.
        size_t off = 0;
        while (off < lzw.size()) {
            size_t take = std::min<size_t>(255, lzw.size() - off);
            put8((uint8_t)take);
            putBytes(lzw.data() + off, take);
            off += take;
        }
        put8(0x00); // sub-block terminator
    }

    static std::vector<uint8_t> lzwEncode(const std::vector<uint8_t> &input) {
        constexpr int kClear = 256;
        constexpr int kEoi = 257;
        constexpr int kMaxCode = 4096;

        std::vector<uint8_t> out;
        uint32_t bit_buffer = 0;
        int bit_count = 0;
        int code_size = 9;
        int next_code = 258;

        // (prefix << 8) | next byte → assigned dictionary code.
        std::unordered_map<uint32_t, int> dict;
        dict.reserve(4096);

        auto emit = [&](int code) {
            bit_buffer |= (uint32_t)code << bit_count;
            bit_count += code_size;
            while (bit_count >= 8) {
                out.push_back((uint8_t)(bit_buffer & 0xFF));
                bit_buffer >>= 8;
                bit_count -= 8;
            }
        };

        emit(kClear);
        if (input.empty()) {
            emit(kEoi);
            if (bit_count > 0) out.push_back((uint8_t)(bit_buffer & 0xFF));
            return out;
        }

        int current = input[0];
        for (size_t i = 1; i < input.size(); i++) {
            uint8_t p = input[i];
            uint32_t key = ((uint32_t)current << 8) | p;
            auto it = dict.find(key);
            if (it != dict.end()) {
                current = it->second;
            } else {
                emit(current);
                if (next_code < kMaxCode) {
                    dict[key] = next_code++;
                    // Bump when the *just-assigned* code no longer fits the
                    // current code size — matches gif_lib semantics. Using
                    // `==` here drifts one emit ahead of the decoder and
                    // produces an unreadable bitstream after ~256 pixels.
                    if (next_code > (1 << code_size) && code_size < 12) {
                        code_size++;
                    }
                }
                if (next_code >= kMaxCode) {
                    emit(kClear);
                    dict.clear();
                    next_code = 258;
                    code_size = 9;
                }
                current = p;
            }
        }
        emit(current);
        emit(kEoi);
        if (bit_count > 0) out.push_back((uint8_t)(bit_buffer & 0xFF));
        return out;
    }

    static uint8_t nearestIndex(const std::vector<uint32_t> &palette, uint32_t key) {
        uint8_t r = (key >> 16) & 0xFF, g = (key >> 8) & 0xFF, b = key & 0xFF;
        int best = 0, bestDist = INT32_MAX;
        for (size_t i = 0; i < palette.size(); i++) {
            int pr = (palette[i] >> 16) & 0xFF;
            int pg = (palette[i] >> 8) & 0xFF;
            int pb = palette[i] & 0xFF;
            int d = (pr - r) * (pr - r) + (pg - g) * (pg - g) + (pb - b) * (pb - b);
            if (d < bestDist) { bestDist = d; best = (int)i; }
        }
        return (uint8_t)best;
    }
};

// ---------------------------------------------------------------------------
// Frame extraction + scaling
// ---------------------------------------------------------------------------
static void extractFrame(const MockPixelCanvas &canvas, CRGB grid[kMatrixHeight][kMatrixWidth]) {
    for (int y = 0; y < kMatrixHeight; y++)
        for (int x = 0; x < kMatrixWidth; x++)
            grid[y][x] = CRGB(0, 0, 0);
    for (int i = 0; i < canvas.pixelCount; i++) {
        const auto &p = canvas.pixels[i];
        if (p.x >= 0 && p.x < (int)kMatrixWidth && p.y >= 0 && p.y < (int)kMatrixHeight)
            grid[p.y][p.x] = p.color;
    }
}

static void scalePixels(const CRGB grid[kMatrixHeight][kMatrixWidth], int scale,
                        std::vector<uint8_t> &out_rgb) {
    int W = (int)kMatrixWidth * scale;
    int H = (int)kMatrixHeight * scale;
    out_rgb.resize((size_t)W * H * 3);
    for (int y = 0; y < H; y++) {
        int sy = y / scale;
        for (int x = 0; x < W; x++) {
            int sx = x / scale;
            const CRGB &c = grid[sy][sx];
            size_t idx = ((size_t)y * W + x) * 3;
            out_rgb[idx + 0] = c.r;
            out_rgb[idx + 1] = c.g;
            out_rgb[idx + 2] = c.b;
        }
    }
}

// ---------------------------------------------------------------------------
// CLI
// ---------------------------------------------------------------------------
struct Args {
    std::string effect = "Fire";
    std::string out;
    int frames = 90;
    int fps = 50;
    int scale = 32;
    int seed = 1;
    int brightness = 120; // matches BrightnessConfig default in src/Globals.cpp
    bool no_gamma = false;
    bool list = false;
};

static void printUsage() {
    std::fprintf(stderr,
        "Usage: render_effect --effect <name> [options]\n"
        "  --effect <name>     Effect to render (default: Fire)\n"
        "  --out <path>        Output GIF path (default: <effect>.gif)\n"
        "  --frames <n>        Number of frames (default: 90)\n"
        "  --fps <n>           Frames per second (default: 50, clean GIF cs)\n"
        "  --scale <n>         Pixel scale factor (default: 32 → 1024x256)\n"
        "  --seed <n>          RNG seed for reproducible output (default: 1)\n"
        "  --brightness <n>    Simulated LED brightness 2..180 for gamma\n"
        "                      (default: 120, matches device factory default)\n"
        "  --no-gamma          Skip gamma correction (output raw effect colors)\n"
        "  --list              List available effects and exit\n"
    );
}

static bool parseArgs(int argc, char **argv, Args &a) {
    for (int i = 1; i < argc; i++) {
        std::string s = argv[i];
        auto need = [&](const char *) -> const char * {
            if (i + 1 >= argc) { printUsage(); std::exit(1); }
            return argv[++i];
        };
        if (s == "--effect") a.effect = need("--effect");
        else if (s == "--out") a.out = need("--out");
        else if (s == "--frames") a.frames = std::atoi(need("--frames"));
        else if (s == "--fps") a.fps = std::atoi(need("--fps"));
        else if (s == "--scale") a.scale = std::atoi(need("--scale"));
        else if (s == "--seed") a.seed = std::atoi(need("--seed"));
        else if (s == "--brightness") a.brightness = std::atoi(need("--brightness"));
        else if (s == "--no-gamma") a.no_gamma = true;
        else if (s == "--list") a.list = true;
        else if (s == "-h" || s == "--help") { printUsage(); std::exit(0); }
        else { std::fprintf(stderr, "Unknown arg: %s\n", s.c_str()); printUsage(); return false; }
    }
    if (a.out.empty()) a.out = a.effect + ".gif";
    return true;
}

static int findEffect(const std::string &name) {
    for (size_t i = 0; i < kEffectCount; i++)
        if (name == kEffects[i].name) return (int)i;
    return -1;
}

int main(int argc, char **argv) {
    Args a;
    if (!parseArgs(argc, argv, a)) return 1;

    if (a.list) {
        for (size_t i = 0; i < kEffectCount; i++)
            std::printf("%s\n", kEffects[i].name);
        return 0;
    }

    int idx = findEffect(a.effect);
    if (idx < 0) {
        std::fprintf(stderr, "Unknown effect: %s (use --list to see all)\n", a.effect.c_str());
        return 1;
    }

    std::srand((unsigned)a.seed);
    int W = (int)kMatrixWidth * a.scale;
    int H = (int)kMatrixHeight * a.scale;

    float gamma = a.no_gamma ? 1.0f : calculateGamma((float)a.brightness);
    if (a.no_gamma) {
        std::printf("Rendering %s — %d frames @ %d fps, %dx%d, gamma=off → %s\n",
                    a.effect.c_str(), a.frames, a.fps, W, H, a.out.c_str());
    } else {
        std::printf("Rendering %s — %d frames @ %d fps, %dx%d, brightness=%d gamma=%.3f → %s\n",
                    a.effect.c_str(), a.frames, a.fps, W, H, a.brightness, gamma, a.out.c_str());
    }

    GifWriter gif(a.out, W, H, a.fps);
    MockPixelCanvas canvas;
    CRGB grid[kMatrixHeight][kMatrixWidth];
    std::vector<uint8_t> rgb;

    int ms_per_frame = 1000 / a.fps;
    for (int f = 0; f < a.frames; f++) {
        setMockMillis((uint32_t)(f * ms_per_frame));
        canvas.reset();
        kEffects[idx].func(canvas, 0, 0, &kEffects[idx].settings);
        extractFrame(canvas, grid);
        if (!a.no_gamma) {
            for (int y = 0; y < (int)kMatrixHeight; y++)
                for (int x = 0; x < (int)kMatrixWidth; x++)
                    grid[y][x] = applyGamma_video(grid[y][x], gamma);
        }
        scalePixels(grid, a.scale, rgb);
        gif.writeFrame(rgb.data());
        if ((f + 1) % 20 == 0) std::printf("  frame %d/%d\n", f + 1, a.frames);
    }
    gif.finalize();
    std::printf("Done.\n");
    return 0;
}
