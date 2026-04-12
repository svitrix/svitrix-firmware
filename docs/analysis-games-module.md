> **Note:** The Games module was removed in v0.3.0. This document is kept for historical reference.

# Games Module — Code Review & Analysis

## Overview

The `src/Games/` module contains two mini-games for the SVITRIX LED matrix (32x8 pixels), managed by a singleton `GameManager`.

## Files

| File | Lines | Description |
|---|---|---|
| `GameManager.h/cpp` | 159 | Singleton orchestrator — game selection, lifecycle, score reporting |
| `SlotMachine.h/cpp` | 486 | 3-reel slot machine with 8 pixel-art symbols |
| `SvitrixSays.h/cpp` | 295 | Simon Says clone — 4-color memory sequence game |
| **Total** | **940** | **~6% of the codebase (15,608 LOC)** |

## Architecture

```
GameManager (singleton)
├── start(active)          — activate/deactivate game mode
├── ChooseGame(gameID)     — select game (0=Slot, 1=SvitrixSays)
├── tick()                 — per-frame update (called from DisplayManager main loop)
├── selectPressed()        — handle button input
├── ControllerInput(cmd)   — handle network controller commands
└── sendPoints(score)      — report score via JSON/TCP to ServerManager
        │
        ├── SlotMachine_   — 8 symbols, spin animation, win detection
        └── SvitrixSays_   — 4 buttons, 32-step max sequence
```

## Integration Points

Games are wired into **5 subsystems**:

| Subsystem | File | How |
|---|---|---|
| Main loop | `DisplayManager.cpp:171` | `GameManager.tick()` called when active |
| Settings API | `DisplayManager_Settings.cpp:139` | `{"GAMEMODE": true}`, `{"GAME": 0}` JSON commands |
| Menu | `MenuManager.cpp:434` | Long-press exits active game |
| Network | `ServerManager.cpp:365` | Controller commands routed to `ControllerInput()` |
| Setup | `main.cpp` | Display interface wired via `setDisplay()` |

**No dedicated menu entry** — games are only activatable via JSON API or network controller, never from the on-device menu.

## Dependencies

Games pull in **zero additional libraries**. They use only shared infrastructure already present:

- `IDisplayRenderer` — drawing primitives (fillRect, drawRGBBitmap, printText)
- `PeripheryManager` — RTTTL melody playback for audio feedback
- `ServerManager` — TCP score transmission
- `ArduinoJson` — score serialization (already a project dependency)

No external game engines, no extra assets on filesystem.

## Resource Footprint

| Resource | Cost |
|---|---|
| Flash (code) | ~940 LOC compiled — estimated 8-12 KB |
| Flash (data) | 8 slot symbols × 64 × 4 bytes = ~2 KB pixel data |
| RAM | Two game objects + state enums — estimated <500 bytes |
| Filesystem | Zero — no external files |
| Libraries | Zero additional |

## Code Quality Issues

### Naming
- Typo in SlotMachine.cpp: `"LOOSE"` → should be `"LOSE"`
- Magic numbers throughout: `32` (matrix width), `8` (matrix height), color values like `0xFF0000`

### Design
- `GameManager` hardcodes game selection via switch-case — adding a third game requires modifying the manager
- No game interface/base class — SlotMachine and SvitrixSays have similar APIs but no shared abstraction
- `sendPoints()` uses `DynamicJsonDocument(1024)` for a 20-byte payload — oversized allocation

### Dead-ish Behavior
- Score reporting via `sendPoints()` sends JSON over TCP, but there is no visible server-side handler or leaderboard — scores are sent into the void unless an external system listens
- No menu entry means users can only discover games via API documentation or companion app

## Verdict: Keep or Remove?

### Arguments to KEEP

1. **Low cost** — 940 lines (6%), no extra dependencies, ~12 KB flash, <500 bytes RAM
2. **Clean isolation** — well-contained in `src/Games/`, no circular dependencies
3. **Uses dependency injection** — `IDisplayRenderer` interface makes it testable
4. **Differentiating feature** — games on an LED matrix are a novelty that competitors (AWTRIX, LaMetric) also offer
5. **Already integrated** — removing it requires touching 5 files to clean up references

### Arguments to REMOVE

1. **Undiscoverable** — no menu entry, only accessible via JSON API or network controller
2. **No score persistence** — points are sent via TCP but nothing stores or displays them
3. **Limited gameplay** — 32x8 pixel matrix severely constrains game design
4. **Maintenance cost** — any refactoring of DisplayManager or ServerManager must account for game integration
5. **No user demand signal** — no evidence of user requests for more games or game improvements

### Recommendation

**Keep, but make it conditional.** The module is lightweight and clean enough to justify its existence. However:

1. **Add a compile-time flag** (`#define GAMES_ENABLED 1`) to let users strip games from builds if flash space is needed
2. **Add a menu entry** under APPS — currently games are invisible to users who don't know the API
3. **Fix the typo** (`LOOSE` → `LOSE`)
4. **Consider extracting a `IGame` interface** if a third game is ever added — right now two games don't justify the abstraction
