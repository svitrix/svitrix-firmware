# Games Block — AI Reference

> Interactive games playable on the 32x8 LED matrix with TCP controller input.

## Files

| File | Purpose |
|------|---------|
| `GameManager.h` / `.cpp` | Game lifecycle coordinator |
| `SlotMachine.h` / `.cpp` | 3-reel slot machine |
| `SvitrixSays.h` / `.cpp` | Simon Says memory game |

## GameManager_

**Pattern**: Singleton
**Injected**: `IDisplayRenderer*` via `setDisplay()`

### State Machine

```cpp
enum GameState { None, Slot_Machine, SVITRIX_Says };
```

### API

| Method | Purpose |
|--------|---------|
| `start(bool active)` | Set active state, reset to None |
| `ChooseGame(short game)` | 0=SlotMachine, 1=SvitrixSays; calls setup() |
| `tick()` | Render active game, or "SELECT" text when None |
| `selectPressed()` | Delegate to active game |
| `ControllerInput(cmd)` | Delegate to active game |
| `sendPoints(int pts)` | Send `{"points": N}` via TCP to ServerManager |

## SlotMachine_

### Layout
3 reels, 8px wide each, 2px left margin, 2px gap between reels.
8 symbols: cherry, lemon, bar, seven, diamond, crown, grape, watermelon.

### State Machine

```
SLOT_IDLE → (select/START) → SLOT_SPINNING → (all stopped) → SLOT_RESULT
  ↑                                                              │
  │              SLOT_BLINKWIN (6 blinks, 1.2s)                  │
  └──────────────────────────────────────────────────────────────┘
```

### Reel Physics
- Speed: base 0.4 + random(0, 2)
- Duration per reel: 1000ms + random(500, 5000)ms
- Deceleration: speed × 0.28 when time reached
- Stop: offset >= 8 (REEL_HEIGHT)

### Win Conditions
| Condition | Points | Sound |
|-----------|--------|-------|
| 3× Seven | 100 | Jackpot RTTTL |
| 3× Diamond | 75 | Jackpot RTTTL |
| 3× Crown | 50 | Jackpot RTTTL |
| 3× Other match | 25 | Jackpot RTTTL |
| 2-reel match | +10 each | two_match RTTTL |

### Input
- `selectPressed()` or `ControllerInput("START")` → start spin if IDLE

## SvitrixSays_

### Layout
4 color quadrants (16×4 each):
```
[Green ] [Red    ]
[Yellow] [Blue   ]
```

### State Machine

```
SVITRIX_READY → (start) → SVITRIX_SHOWSEQ → (done) → SVITRIX_USERINPUT
     ↑                                                       │
     │         SVITRIX_PAUSE (1s)                             │ correct → ++length
     │              ↑                                         │ wrong → SVITRIX_LOSE
     │              └─────────────────────────────────────────┘
     │
     └────── SVITRIX_LOSE (2s "LOOSE" display) ──────────────┘
```

### Sequence Playback
- MAX_SEQ = 32
- 400ms ON (highlighted + tone), 200ms OFF
- Tones per color: C, D, E, F (RTTTL strings)

### Input Mapping
| Controller | Button | Color |
|-----------|--------|-------|
| "ADOWN" | 0 | Green |
| "BDOWN" | 1 | Red |
| "CDOWN" | 2 | Yellow |
| "DDOWN" | 3 | Blue |
| "START" | — | Restart game |

### Scoring
- Correct sequence → publish `sequenceLength` points
- Win (length > 32) → 1000 points
- Wrong → 0 points, restart
