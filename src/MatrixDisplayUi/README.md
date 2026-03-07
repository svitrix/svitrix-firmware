# MatrixDisplayUi Block — AI Reference

> App framework managing app lifecycle, animated transitions, overlays, and status indicators on a 32x8 LED matrix.

## Files

| File | Purpose |
|------|---------|
| `MatrixDisplayUi.h` | Public API, enums, state struct, callback types |
| `MatrixDisplayUi.cpp` | Constructor, state machine, app management |
| `MatrixDisplayUi_internal.h` | Shared globals (gif1, gif2, currentTransition) |
| `MatrixDisplayUi_Transitions.cpp` | 10 transition effects + rotate helper |
| `MatrixDisplayUi_Indicators.cpp` | 3 RGB indicator rendering + fadeColor |

## Callback Signatures

```cpp
typedef void (*AppCallback)(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state,
                           int16_t x, int16_t y, GifPlayer *gifPlayer);

typedef void (*OverlayCallback)(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state,
                               GifPlayer *gifPlayer);

typedef void (*BackgroundCallback)(FastLED_NeoMatrix *matrix);
```

## State Machine

```
              auto-cycle timer or manual trigger
    FIXED ─────────────────────────────────────────► IN_TRANSITION
      ▲                                                    │
      │         ticksPerTransition elapsed                 │
      └────────────────────────────────────────────────────┘
```

### MatrixDisplayUiState

```cpp
struct MatrixDisplayUiState {
  u_int64_t lastUpdate = 0;                     // Frame timestamp (millis)
  unsigned long ticksSinceLastStateSwitch = 0;   // Ticks in current state
  AppState appState = FIXED;                     // FIXED or IN_TRANSITION
  uint8_t currentApp = 0;                        // Active app index
  int8_t appTransitionDirection = 1;             // +1 forward, -1 backward
  bool lastFrameShown = false;
  bool manualControl = false;                    // User-triggered transition
  void *userData = NULL;
};
```

## Rendering Pipeline (per tick)

```
1. matrix->clear()
2. Background effect (if BackgroundEffect > -1) → callEffect()
3. App rendering → drawApp() (transition or current app)
4. Overlays → overlayFunctions[i]() (Menu, Notify, Status)
5. Indicators → drawIndicators() (3 RGB LEDs)
6. Global weather overlay (if set) → EffectOverlay()
7. host_->gammaCorrection()
8. matrix->show()
```

## Transition Effects (10)

| # | Name | Algorithm |
|---|------|-----------|
| 1 | SLIDE | Vertical slide with direction control (SLIDE_UP/DOWN) |
| 2 | FADE | Fade through black with quadratic easing |
| 3 | ZOOM | Scale to/from center (16, 4) |
| 4 | ROTATE | 360-degree spin around center (16, 4) |
| 5 | PIXELATE | Random pixel-by-pixel replacement |
| 6 | CURTAIN | Center-outward wipe reveal |
| 7 | RIPPLE | Checkerboard dissolve |
| 8 | BLINK | 3 flashes with black gaps |
| 9 | RELOAD | Horizontal wipe right then left |
| 10 | CROSSFADE | Linear per-pixel lerp8 blend |

All transitions use: `progress = ticksSinceLastStateSwitch / ticksPerTransition` (0.0 to 1.0).
Double buffering: `ledsCopy[256]` captures old frame for blending.

## API Methods

### App Management

| Method | Signature | Notes |
|--------|-----------|-------|
| `setApps` | `(vector<pair<String, AppCallback>>)` | Allocate callbacks, resetState, sendAppLoop |
| `nextApp` | `()` | Guard: only in FIXED state, direction=+1 |
| `previousApp` | `()` | Guard: only in FIXED state, direction=-1 |
| `switchToApp` | `(uint8_t app) → bool` | Instant switch, no animation |
| `transitionToApp` | `(uint8_t app)` | Animated transition to specific app |

### Timing

| Method | Signature | Notes |
|--------|-----------|-------|
| `setTargetFPS` | `(uint8_t fps)` | Recalculates updateInterval + ticksPerTransition |
| `setTimePerApp` | `(long ms)` | Convert to ticks: `ms / updateInterval` |
| `setTimePerTransition` | `(uint16_t ms)` | Convert to ticks: `ms / updateInterval` |

### Effects & Overlays

| Method | Signature | Notes |
|--------|-----------|-------|
| `setOverlays` | `(OverlayCallback*, uint8_t count)` | Register overlay callbacks |
| `setBackgroundEffect` | `(int effect)` | -1 = none, else callEffect(index) |
| `setGlobalOverlay` | `(OverlayEffect e)` | Weather overlay for all apps |

## Status Indicators

3 RGB indicators at right edge, each supports solid/blink/fade modes:

| Indicator | Pixels | Default Color |
|-----------|--------|---------------|
| 1 (top-right) | (31,0), (30,0), (31,1) | Red |
| 2 (middle-right) | (31,3), (31,4) | Green |
| 3 (bottom-right) | (31,7), (31,6), (30,7) | Blue |

**Blink**: toggle every `blink` ms → `millis() % (2 * blink) < blink`
**Fade**: sine-wave brightness → `(sin(2π * millis / interval) + 1) * 0.5`

## Defaults

| Setting | Default | Notes |
|---------|---------|-------|
| ticksPerApp | 151 | ~5000ms @ 30 FPS |
| ticksPerTransition | 15 | ~500ms @ 30 FPS |
| updateInterval | 33ms | ~30 FPS |
| autoTransition | true | |
| appAnimationDirection | SLIDE_DOWN | |
| BackgroundEffect | -1 | None |

## Shared Globals (internal.h)

```cpp
extern GifPlayer gif1;             // Current app GIF
extern GifPlayer gif2;             // Next/overlay GIF
extern uint8_t currentTransition;  // Active TransitionType
extern bool swapped;               // True after FIXED app rendered
```
