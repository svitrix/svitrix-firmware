# UpdateManager — AI Reference

OTA firmware update manager. Checks remote server for new versions and performs HTTPS firmware updates with progress display.

## TL;DR

- **Provides:** `IUpdater` → ServerManager, MQTTManager, MenuManager
- **Consumes:** `IDisplayRenderer`
- **Entry point:** `UpdateManager_::checkUpdate()`, `::updateFirmware()`
- **Security:** HTTPS with pinned CA cert (`cert.h`). Optional signed-OTA Phase 1 structural pre-check (`systemConfig.verifyUpdateSignature`) — see [ADR 0005](../../adr/0005-signed-ota-phase-1.md).

> 📌 Auto-loads when reading files in `src/UpdateManager/`

## Files

| File | Purpose |
|------|---------|
| `UpdateManager.h` | Singleton, `IUpdater` implementation |
| `UpdateManager.cpp` | Version check, firmware download, progress callbacks |

## OTA Flow

1. `checkUpdate(withScreen)` — HTTPS GET version string, compare against `VERSION` constant
2. `updateFirmware()` — download via `httpUpdate.update()` with pinned CA cert ([src/data/cert.h](../data/cert.h))
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
| `systemConfig.verifyUpdateSignature` | Phase 1 signed-OTA toggle (NVS `VFYUPD`, dev.json `verify_update_signature`); see ADR 0005 |

## Don't

- Don't bypass HTTPS cert validation — we ship a pinned root CA in `cert.h`
- Don't interleave `updateFirmware()` with display/MQTT work — the device reboots on success
- Don't cache version string across boots — always fetch live from URL
- Don't treat Phase 1 signature verification as full cryptographic protection — it only checks signature presence + structural plausibility. Phase 2 (ADR 0005) adds RSA-PSS verification.
