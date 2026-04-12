# UpdateManager

OTA firmware update manager. Checks a remote server for new firmware versions
and performs HTTPS downloads with a progress bar rendered on the LED matrix.

## Key Files

| File | Purpose |
|------|---------|
| `UpdateManager.h` | Singleton, implements `IUpdater` |
| `UpdateManager.cpp` | Version check, HTTPS firmware download, progress callbacks |

## OTA Flow

1. `checkUpdate()` -- HTTPS GET version string, compare against `VERSION`
2. `updateFirmware()` -- download via `httpUpdate.update()` with pinned CA cert
3. Progress bar rendered via `IDisplayRenderer::drawProgressBar()` (green, y=7)
4. Automatic reboot on success

## Detailed Reference

See the UpdateManager section in [src/CLAUDE.md](../CLAUDE.md) for config
fields, interface wiring, and update URLs.
