# MenuManager

On-device settings menu rendered on the 32x8 LED matrix. Provides 13 menu items
for adjusting brightness, color, time format, date format, app behavior, audio,
and triggering OTA updates -- all via physical buttons without needing a phone or PC.

## Key Files

| File | Purpose |
|------|---------|
| `MenuManager.h` | Singleton, implements `IButtonHandler` |
| `MenuManager.cpp` | Menu state machine, 13 items, rendering, button navigation |

## Navigation

- Left/Right buttons: cycle menu items (or adjust values in submenu)
- Select (short): enter submenu / toggle value
- Select (long): save and exit menu

## Detailed Reference

See the MenuManager section in `src/CLAUDE.md` for full menu item table,
interface wiring, and navigation matrix.
