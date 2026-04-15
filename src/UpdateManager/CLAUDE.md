# UpdateManager — AI Reference

OTA firmware update manager. Checks remote server for new versions and performs HTTPS firmware updates with progress display.

## TL;DR

- **Provides:** `IUpdater` → ServerManager, MQTTManager, MenuManager
- **Consumes:** `IDisplayRenderer`
- **Entry point:** `UpdateManager_::checkUpdate()`, `::updateFirmware()`
- **Security:** HTTPS with pinned CA cert (`cert.h`)

> 📌 Auto-loads when reading files in `src/UpdateManager/`

## Files

| File | LOC | Purpose |
|------|-----|---------|
| `UpdateManager.h` | 23 | Singleton, `IUpdater` implementation |
| `UpdateManager.cpp` | 36 | Version check, firmware download, progress callbacks |

## OTA Flow

1. `checkUpdate(withScreen)` — HTTPS GET version string, compare against `VERSION` constant
2. `updateFirmware()` — download via `httpUpdate.update()` with pinned CA cert (`cert.h`)
3. Progress bar rendered via `IDisplayRenderer::drawProgressBar()` (green fill, y=7)
4. On success: automatic reboot

## Interfaces

**Provides:** `IUpdater` → ServerManager, MQTTManager, MenuManager

**Consumes:** `IDisplayRenderer`

## Config Fields

| Field | Purpose |
|-------|---------|
| `systemConfig.updateVersionUrl` | URL returning plain-text version |
| `systemConfig.updateFirmwareUrl` | URL to firmware `.bin` file |
| `systemConfig.updateAvailable` | Flag set by `checkUpdate()` |

## Don't

- Don't bypass HTTPS cert validation — we ship a pinned root CA in `cert.h`
- Don't interleave `updateFirmware()` with display/MQTT work — the device reboots on success
- Don't cache version string across boots — always fetch live from URL
