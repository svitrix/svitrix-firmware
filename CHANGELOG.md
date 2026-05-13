# Changelog

All notable changes to Svitrix firmware are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
with pre-release tags (see [.claude/git-workflow.md](.claude/git-workflow.md)).

## [Unreleased]

### Added
- ADR scaffolding under `adr/` documenting key architectural decisions (interface decoupling, 3-class DisplayManager, stateless services, Clean Code standard, Signed OTA Phase 1).
- `ResetReason` service — captures `esp_reset_reason()` at boot and surfaces it in `/stats` JSON as `reset_reason` (readable in Home Assistant via template sensor).
- `SignatureVerifier` service and Signed OTA **Phase 1** scaffolding — `verifyUpdateSignature` config flag, embedded public-key slot (`src/data/ota_pubkey.h`), `.sig` URL convention, structural pre-check in `UpdateManager`. **Not yet cryptographically protective** — see [ADR 0005](adr/0005-signed-ota-phase-1.md) Phase 2.
- Read-only PR-review sub-agents: `pr-reviewer`, `embedded-cpp-reviewer`, `firmware-architecture-reviewer`.
- Community files: `SECURITY.md` (vulnerability disclosure policy), `CONTRIBUTING.md` (local-dev + design rules), `.github/PULL_REQUEST_TEMPLATE.md`.
- CI `clang-tidy` advisory job scoped to `lib/services/src/` (continue-on-error initially).

### Fixed
- **Security:** format-string injection in UDP discovery and `DataFetcher` (#54).

## [0.3.1] — 2026-04-26

### Changed
- CI release workflow now bundles `littlefs.bin` (SPA) alongside `firmware.bin` in release artifacts (#51).

## [0.3.0] — 2026-04-26

### Added
- Scheduled night mode with brightness and color override (#45).
- Fire effect and background effect picker (#49).
- Configurable icon layout (left / right / none).
- `/parallel` and `/delegate` Claude Code slash commands for multi-agent workflows.
- "Cross-module Impact Map" and "Common Change Patterns" tables in root `CLAUDE.md` to speed up new-feature work.

### Changed
- SPA rewritten with a proper component library, settings sections, and theming.
- CI auto-injects the firmware version from a `version` file (#50).
- `src/` restructured into module directories.
- Docs deploy only on release, not on every push to `main`.
- `CLAUDE.md` hierarchy reorganized for on-demand loading (#46).

### Removed
- Games module — out of scope for a display-first device.

## [0.2.0] — 2026-04-12

### Added
- `DataFetcher` module — external HTTP data sources polled in round-robin (#18).
- New Preact + Vite SPA served from LittleFS, replacing the legacy web UI.
- Module-level `CLAUDE.md` documentation for every top-level module (#20).
- Claude Code slash commands and commit linting (#30).

### Changed
- C++ standard upgraded from C++11 to C++17.
- Font system replaced with a Unicode sparse glyph table and UTF-8 rendering.
- Documentation refreshed across `CLAUDE.md` files and project metadata (#29, #31).

### Fixed
- MQTT: Home Assistant entity memory is now freed before re-allocation in `setup()`, preventing a leak across reconnects.

## [0.1.0] — 2026-03-07

### Added
- Initial public release of Svitrix firmware for the Ulanzi TC001 Smart Pixel Clock and compatible 32×8 LED matrix devices.
- Pre-installed apps (time, date, temperature, humidity, battery), MQTT + HTTP API, Home Assistant auto-discovery, web UI with WiFi setup and OTA, 19 visual effects, weather overlays, RTTTL melodies, animated icons, drawing API, charts, indicators, mood light, on-screen menu, Artnet/DMX receiver, custom palettes, online flasher, backup/restore.

[Unreleased]: https://github.com/svitrix/svitrix-firmware/compare/v0.3.1...HEAD
[0.3.1]: https://github.com/svitrix/svitrix-firmware/compare/v0.3.0...v0.3.1
[0.3.0]: https://github.com/svitrix/svitrix-firmware/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/svitrix/svitrix-firmware/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/svitrix/svitrix-firmware/releases/tag/v0.1.0
