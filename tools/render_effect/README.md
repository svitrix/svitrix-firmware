# render_effect

Native CLI that renders any Svitrix visual effect to an animated GIF — no
device or Arduino runtime required. Reuses the firmware's effect implementations
through `MockPixelCanvas`, the same gamma curve as `DisplayManager::gammaCorrection`,
and real FastLED palettes — so output is frame-for-frame identical to what
the Ulanzi matrix produces at the same brightness.

## Build

```bash
make            # builds ./render_effect
make clean      # removes binary + *.gif
```

Single translation unit, no external deps beyond a C++17 compiler.

## Usage

```bash
./render_effect --list                        # list all 20 effects
./render_effect --effect Plasma               # render Plasma.gif (defaults)
./render_effect --effect Fire --out fire.gif --frames 252 --fps 42 --scale 16
```

| Flag | Default | Notes |
|------|---------|-------|
| `--effect <name>` | `Fire` | One of the names from `--list` |
| `--out <path>` | `<effect>.gif` | Output GIF path |
| `--frames <n>` | `90` | Number of frames |
| `--fps <n>` | `50` | Frame rate written into the GIF |
| `--scale <n>` | `32` | Pixel scale factor (32 → 1024×256) |
| `--seed <n>` | `1` | RNG seed (deterministic effects) |
| `--brightness <n>` | `120` | Simulated LED brightness (2..180) for gamma stage. Matches `BrightnessConfig` default. |
| `--no-gamma` | off | Skip gamma correction (raw effect colors) |

## Make targets

```bash
make Plasma.gif                       # build + render one effect
make all-gifs                         # render all 20
make all-gifs FPS=21 FRAMES=126 SCALE=16   # half-speed @ 512×128 (used in docs/assets)
```

`FPS`, `FRAMES`, `SCALE` are make variables — override on the command line.

## Refreshing docs/assets/

GIFs in [docs/assets/](../../docs/assets/) come from this tool. To regenerate
after adding a new effect or tweaking palettes:

```bash
make all-gifs FPS=21 FRAMES=126 SCALE=16
cp *.gif ../../docs/assets/
```

The size check in `.pre-commit-config.yaml` already excludes
`docs/assets/*.gif`, so even >500 KB demos commit cleanly.

## How it works

1. Includes the actual effect `.cpp` files from `src/effects/` (compile-time
   inclusion — no static lib).
2. Drives the effect with `MockPixelCanvas` from [test/mocks/](../../test/mocks/),
   stepping mock `millis()` once per frame.
3. Applies firmware gamma via `applyGamma_video` from the same
   [GammaUtils](../../lib/services/src/GammaUtils.cpp) the device uses.
4. Writes a GIF89a with proper LZW compression and per-frame local color
   tables (≤256 unique colors per frame, padded to 7-bit LCT).

A local `FastLED.h` shim provides real `RainbowColors_p` / `HeatColors_p` /
etc. (the regular test mock returns generic stubs), plus a Perlin-noise
`inoise8` so `PlasmaCloud` looks correct.

## Known gotchas

- LZW `code_size` bump uses the same off-by-one rule as `gif_lib`
  (`next_code > (1 << code_size)`, not `==`). The `==` form drifts the
  encoder one emit ahead of the decoder and produces an unreadable
  bitstream after ~256 pixels. macOS Quick Look tolerates the corruption;
  PIL, browsers, and ImageMagick do not.
- `MockPixelCanvas` caps at 1024 stored pixel writes — fine for 32×8
  frames but worth knowing if you raise `kMatrixWidth/Height`.
- Effects that gate on `millis()` (e.g. `Fire`) update less often per
  frame as `--fps` rises, since the mock millis step shrinks. For motion
  matching the device, render at `--fps 42`.
