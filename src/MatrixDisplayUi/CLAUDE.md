# MatrixDisplayUi — AI Reference

App framework for the 32x8 LED matrix. Manages app lifecycle, animated transitions, overlays, background effects, and status indicators.

## File Map

| File | LOC | Purpose |
|------|-----|---------|
| `MatrixDisplayUi.h` | 316 | Public API, enums, callbacks, state struct |
| `MatrixDisplayUi.cpp` | 428 | State machine, update loop, app management |
| `MatrixDisplayUi_Transitions.cpp` | 388 | 10 transition effects + rotate helper |
| `MatrixDisplayUi_Indicators.cpp` | 180 | 3 RGB indicators with blink/fade |
| `MatrixDisplayUi_internal.h` | 30 | Shared globals (GIF players, transition state) |

## State Machine

```
FIXED (displaying app)
  │ ticksSinceLastStateSwitch >= ticksPerApp (auto)
  │ or nextApp()/previousApp()/transitionToApp() (manual)
  ▼
IN_TRANSITION (animating)
  │ ticksSinceLastStateSwitch >= ticksPerTransition
  ▼
FIXED (next app)
```

### State (MatrixDisplayUiState)
```cpp
AppState appState;           // FIXED or IN_TRANSITION
uint8_t currentApp;          // Active app index
int8_t appTransitionDirection; // +1 forward, -1 backward
bool manualControl;          // True when user-triggered
unsigned long ticksSinceLastStateSwitch;
```

## Rendering Pipeline (per tick)

```
1. matrix->clear()
2. Background effect (if BackgroundEffect > -1)
3. App rendering (drawApp)
   ├─ FIXED: AppFunctions[currentApp](matrix, state, 0, 0, gif1)
   └─ IN_TRANSITION: transition effect blending old ↔ new app
4. Overlays (drawOverlays) — menu, notifications, status
5. Indicators (drawIndicators) — 3 corner RGB LEDs
6. Global weather overlay (if set)
7. host_->gammaCorrection()
8. matrix->show()
```

## 10 Transition Effects

| Enum | Effect | Description |
|------|--------|-------------|
| SLIDE (1) | Vertical slide | Direction via appAnimationDirection |
| FADE (2) | Fade through black | Quadratic easing |
| ZOOM (3) | Scale to/from center | Center (16,4) |
| ROTATE (4) | 360° spin | Around center (16,4) |
| PIXELATE (5) | Random pixel dissolve | Per-pixel probability |
| CURTAIN (6) | Center-outward wipe | Reveals from center |
| RIPPLE (7) | Checkerboard dissolve | Even/odd pixel pattern |
| BLINK (8) | 3 flashes | With black gaps |
| RELOAD (9) | Horizontal wipe | Right-out, then left-in |
| CROSSFADE (10) | Linear pixel blend | lerp8 per pixel |

RANDOM (0) selects a random effect per cycle.

Progress: `ticksSinceLastStateSwitch / ticksPerTransition` (0.0 → 1.0)

## App Framework

### Callback Types
```cpp
// App: renders one page on the matrix
using AppCallback = std::function<void(FastLED_NeoMatrix*, MatrixDisplayUiState*, int16_t x, int16_t y, GifPlayer*)>;

// Overlay: drawn on top of current app every frame
typedef void (*OverlayCallback)(FastLED_NeoMatrix*, MatrixDisplayUiState*, GifPlayer*);

// Background: drawn behind all apps
typedef void (*BackgroundCallback)(FastLED_NeoMatrix*);
```

### App Management
```cpp
void setApps(const vector<pair<String, AppCallback>>&);  // Register all apps
void nextApp();           // Transition forward (FIXED state only)
void previousApp();       // Transition backward (FIXED state only)
bool switchToApp(uint8_t); // Instant switch, no animation
void transitionToApp(uint8_t); // Animated transition to specific app
```

### Timing
```cpp
void setTargetFPS(uint8_t fps);          // Default ~30 FPS (33ms interval)
void setTimePerApp(long ms);             // How long each app displays
void setTimePerTransition(uint16_t ms);  // Transition animation duration
```

Default: ~5s per app, ~500ms per transition at 30 FPS.

## Indicators (3 RGB LEDs)

| Indicator | Position | Default Color |
|-----------|----------|---------------|
| 1 | Top-right (31,0), (30,0), (31,1) | Red |
| 2 | Mid-right (31,3), (31,4) | Green |
| 3 | Bottom-right (31,7), (31,6), (30,7) | Blue |

Each supports 3 modes:
- **Solid** — blink=0, fade=0
- **Blink** — toggles on/off every `blink` ms
- **Fade** — sine-wave brightness oscillation over `fade` ms

```cpp
void setIndicator{1,2,3}Color(uint32_t);
void setIndicator{1,2,3}State(bool);
void setIndicator{1,2,3}Blink(int ms);
void setIndicator{1,2,3}Fade(int ms);
```

## Effects & Overlays

```cpp
void setBackground(BackgroundCallback fn);     // Register background renderer
void setBackgroundEffect(int effect);          // Activate by index, -1 = none
void setOverlays(OverlayCallback* arr, uint8_t count);  // Register overlay array
void setGlobalOverlay(OverlayEffect e);        // Weather overlay on all apps
```

## Ownership

```
DisplayManager_ (creates and owns MatrixDisplayUi)
  └── MatrixDisplayUi
        ├── receives: FastLED_NeoMatrix* (not owned)
        ├── receives: IMatrixHost* (not owned, = DisplayManager_)
        ├── owns: AppCallback vector
        ├── owns: gif1, gif2 (GifPlayer instances)
        └── receives: overlay/background function pointers (not owned)
```

## IMatrixHost Dependency

MatrixDisplayUi calls back to its host (DisplayManager_) for:
```cpp
CRGB* getLeds();              // Access LED buffer (for transitions)
void gammaCorrection();       // Apply brightness curve
void sendAppLoop();           // Notify app list changed
bool setAutoTransition(bool); // Called after setApps()
```
