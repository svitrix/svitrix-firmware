# Effects Block — AI Reference

> 20 visual effects + weather overlays, all decoupled from hardware via IPixelCanvas.

## Files

| File | Purpose |
|------|---------|
| `EffectTypes.h` | Effect struct, callback typedef, matrix constants (32x8) |
| `EffectRegistry.h` / `.cpp` | Dispatch array, callEffect(), settings update, state reset |
| `WaveEffects.h` / `.cpp` | 6 wave effects (Pacifica, Plasma, ColorWaves, etc.) |
| `PatternEffects.h` / `.cpp` | 5 pattern effects (TheaterChase, Checkerboard, etc.) |
| `ParticleEffects.h` / `.cpp` | 5 particle effects (Stars, Fireworks, Ripple, Matrix, Fire) |
| `GameEffects.h` / `.cpp` | 4 game effects (Snake, PingPong, BrickBreaker, Eyes) |
| `WeatherOverlay.h` / `.cpp` | Weather overlays (Rain, Snow, Storm, Frost, Thunder) |
| `PaletteUtils.h` / `.cpp` | Palette loading (built-in, LittleFS, JSON array) |
| `NeoMatrixCanvas.h` | IPixelCanvas adapter for FastLED_NeoMatrix |
| `effects.h` (parent) | Thin facade, includes all effect headers |

## Registry & Dispatch

```cpp
struct EffectSettings {
  double speed;            // 1-8 typical
  CRGBPalette16 palette;   // FastLED color palette
  bool blend;              // LINEARBLEND vs NOBLEND
};

struct Effect {
  const char *name;
  void (*callback)(IPixelCanvas*, int16_t, int16_t, EffectSettings*);
  EffectSettings settings;
};
```

**API**:
- `callEffect(canvas, x, y, index)` — execute effect by index
- `getEffectIndex(name)` — linear lookup, -1 if not found
- `updateEffectSettings(index, json)` — dynamic speed/palette/blend update
- `resetAllEffectState()` — clear all stateful effects (called on effect switch)

## Effects Table (20 effects)

| # | Name | Category | Speed | Palette | Blend | Algorithm |
|---|------|----------|-------|---------|-------|-----------|
| 0 | Fade | Pattern | 1 | Rainbow | yes | Row-based palette shift |
| 1 | MovingLine | Pattern | 1 | Rainbow | yes | Bouncing horizontal line |
| 2 | BrickBreaker | Game | — | — | — | Autonomous breakout game |
| 3 | PingPong | Game | 8 | Rainbow | yes | Autonomous pong simulation |
| 4 | Radar | Pattern | 1 | Rainbow | yes | Spinning beam with trail decay |
| 5 | Checkerboard | Pattern | 1 | Rainbow | yes | Alternating two-color grid |
| 6 | Fireworks | Particle | 1 | Rainbow | no | Projectile ascent + explosion |
| 7 | PlasmaCloud | Wave | 3 | Rainbow | yes | Perlin noise + hue rotation |
| 8 | Ripple | Particle | 3 | Rainbow | yes | Expanding ring from random origin |
| 9 | Snake | Game | 3 | Rainbow | yes | Autonomous AI snake |
| 10 | Pacifica | Wave | 3 | Ocean | yes | Multi-layer ocean sine waves |
| 11 | TheaterChase | Pattern | 3 | Rainbow | yes | Every-3rd-column chase |
| 12 | Plasma | Wave | 2 | Rainbow | yes | XY sine interference |
| 13 | Matrix | Particle | 8 | Forest | no | Digital rain (hardcoded green) |
| 14 | SwirlIn | Wave | 5 | Rainbow | yes | Radial swirl to center |
| 15 | SwirlOut | Wave | 5 | Rainbow | yes | Radial swirl from center |
| 16 | LookingEyes | Game | — | — | — | Eye sprites with gaze + blink |
| 17 | TwinklingStars | Particle | 4 | Ocean | yes | Random spawn + brightness decay |
| 18 | ColorWaves | Wave | 5 | Rainbow | yes | Horizontal color sweep |
| 19 | Fire | Particle | 5 | Heat | yes | Heat-cell propagation, sparks at bottom row rise upward (Kriegsman fire) |

## Wave Effects (6)

| Effect | State Variables | Algorithm |
|--------|----------------|-----------|
| Pacifica | `pacificaTime` (uint32_t) | 3 sine layers at different frequencies/phases |
| Plasma | `plasmaTime` (double) | `sin8(i*10+t) + sin8(j*10+t/2) + sin8((i+j)*10+t/3)` |
| ColorWaves | none (uses millis) | Column-based palette index: `i * colorStep + millis * speed / 100` |
| PlasmaCloud | `time` + `hueShift` (float) | `inoise8(i*16, j*16, time)` + hue rotation |
| SwirlIn | `angle` (uint16_t) | Distance-from-center(16,4) mapping + rotating hue |
| SwirlOut | `angle` (uint16_t) | Inverted distance mapping (255 - dist) |

## Pattern Effects (5)

| Effect | State | Algorithm |
|--------|-------|-----------|
| TheaterChase | `offset`, `lastUpdate` | `(i + offset) % 3 == 0` → palette color, else black |
| Checkerboard | `index1`, `index2` (float) | `(i + j) % 2 == 0` → color1, else color2 |
| Fade | `hue` (uint8_t) | Row-based: `palette[hue + j * 256/8]` |
| MovingLine | `pos`, `dir`, `palIdx` | Single horizontal line bouncing top↔bottom |
| Radar | `angle` (float), `tempLeds[32][8]` | Radial line from center, framebuffer with fade-by-20 |

## Particle Effects (5)

| Effect | State | Algorithm |
|--------|-------|-----------|
| TwinklingStars | `stars[32][8]{color, brightness}` | Brightness decay 0.01/frame, random spawn 1-5 |
| Fireworks | `fireworks[5]{x,y,life,exploded,color,speed,peak}` | Ascent → explosion (plus-sign), life decay |
| Ripple | `{x,y,life,color}`, `tempLeds[32][8]` | Ring at expanding radius, fade-by-45 trail |
| Matrix | `matrixLedState[32][8]` | Column shift down, spawnColor → trailColor decay |
| Fire | `fireHeat[32][8]` | Per-column cool→rise→spark; render via palette (HeatColors_p default) |

## Game Effects (4, autonomous — no user input)

| Effect | State | Algorithm |
|--------|-------|-----------|
| Snake | body[64], direction, apple | Greedy AI: prefer apple direction, collision avoidance |
| PingPong | 2 paddles(y,dy) + ball(x,y,dx,dy) | Auto-center paddles on ball.y, bounce physics |
| BrickBreaker | paddle, ball, bricks[3][16] | Paddle follows ball, grid collision, reset on win |
| LookingEyes | EyeState{blinkCountdown, gazeX/Y, mood} | 7-frame blink + saccade movement, 5 eye sprites |

## Weather Overlays

| Effect | Spawn | Gravity | Wind | Special |
|--------|-------|---------|------|---------|
| RAIN | 50/255 per frame | Every 2 frames | None | Cyan-blue particles |
| DRIZZLE | 15/255 | Every 2 frames | None | Lighter, sparse |
| STORM | 60/255 | Every 2 frames | Right shift every 3 frames | Same color as rain |
| SNOW | 20/255 | Every 5 frames | None | White particles |
| THUNDER | — | — | — | Random lightning flash + RAIN overlay |
| FROST | 25/255 | None (static) | None | 6-frame per-pixel color cycle |

## Palette System

**Built-in**: Cloud, Lava, Ocean, Forest, Stripe (RainbowStripe), Party, Heat
**Custom**: LittleFS `/PALETTES/{name}.txt` (16 hex colors, one per line)
**Inline**: JSON array `["FF0000", "00FF00", ...]`

```cpp
CRGBPalette16 getPalette(JsonVariant variant);
// string → check builtins, else load from file
// array → parse hex colors
// default → RainbowColors_p
```

## Canvas Abstraction (NeoMatrixCanvas.h)

```cpp
class NeoMatrixCanvas : public IPixelCanvas {
  FastLED_NeoMatrix *matrix_;
  void drawPixel(x, y, CRGB/CHSV/uint16_t);
  void fillRect(x, y, w, h, color);
  void drawRGBBitmap(x, y, bitmap, w, h);
  uint16_t Color(r, g, b);
};
```
