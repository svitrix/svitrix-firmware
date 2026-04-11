---
description: Sync public VitePress documentation (docs/) with code changes — run before release or periodically on feature branches
---

Analyze ALL changes on the current feature branch vs main and update every affected public documentation file in `docs/` to reflect the actual state of the code.

## Context

Public docs live in `docs/` (VitePress, deployed to GitHub Pages). English docs at root, Ukrainian translations in `docs/uk/`. Both languages MUST stay in sync.

## Steps

1. **Identify branch and all changes:**
   ```bash
   git log --oneline main..HEAD
   git diff --stat main..HEAD
   git diff --name-only main..HEAD
   ```
   If on `main` — use `HEAD~5` instead.
   Report: number of commits, files changed, summary of what was done.

2. **Map changed files to public documentation** using this table:

   | Changed files in... | Docs to update (EN + UK) |
   |---------------------|--------------------------|
   | `src/Apps/`, `src/DisplayManager/DisplayManager_CustomApps.cpp` | `docs/apps.md`, `docs/uk/apps.md` |
   | `src/effects/`, `src/effects/EffectRegistry.*` | `docs/effects.md`, `docs/uk/effects.md` |
   | `src/ServerManager.*`, `src/MQTTManager/` | `docs/api.md`, `docs/uk/api.md` |
   | `web/src/` | `docs/webinterface.md`, `docs/uk/webinterface.md` |
   | `src/Globals.cpp`, `lib/config/` | `docs/dev.md`, `docs/uk/dev.md` |
   | `src/DataFetcher/` | `docs/datafetcher.md`, `docs/uk/datafetcher.md` |
   | `src/MelodyPlayer/` | `docs/sounds.md`, `docs/uk/sounds.md` |
   | `src/PeripheryManager.*`, `platformio.ini` (GPIO/hardware) | `docs/hardware.md`, `docs/uk/hardware.md` |
   | `src/MenuManager.*` | `docs/onscreen.md`, `docs/uk/onscreen.md` |
   | `lib/webserver/`, backup-related changes | `docs/backup.md`, `docs/uk/backup.md` |
   | Partition layout, flashing workflow | `docs/flasher.md`, `docs/uk/flasher.md` |
   | New features, setup flow changes | `docs/quickstart.md`, `docs/uk/quickstart.md` |
   | New pages in SPA, navigation changes | `docs/.vitepress/config.mts` |

3. **For each affected doc — read code, then update doc:**
   - Read the changed source files (`git diff main..HEAD -- <path>`)
   - Read the current English doc file
   - Read the corresponding Ukrainian doc file
   - Update ONLY sections affected by the changes:
     - New/removed/renamed API endpoints (MQTT topics + HTTP routes)
     - New/changed JSON payload fields
     - New effects, apps, or menu items
     - Changed settings keys or default values
     - New web UI pages or controls
     - Changed hardware pinout or sensor support

4. **Verify key counts and lists in docs match code:**

   | What | Where to check | Docs that reference it |
   |------|---------------|----------------------|
   | Effects list + count | `src/effects/EffectRegistry.cpp` | `docs/effects.md` |
   | MQTT topics | `src/MQTTManager/MQTTManager_Messages.cpp` | `docs/api.md` |
   | HTTP endpoints | `src/ServerManager.cpp` | `docs/api.md` |
   | Custom app JSON fields | `src/DisplayManager/DisplayManager_ParseHelpers.cpp` | `docs/api.md` (Custom Apps section) |
   | Notification JSON fields | `src/DisplayManager/NotificationManager.cpp` | `docs/api.md` (Notifications section) |
   | Menu items | `src/MenuManager.cpp` | `docs/onscreen.md` |
   | `dev.json` keys | `src/Globals.cpp` (`loadDevSettings()`) | `docs/dev.md` |
   | Web UI pages | `web/src/pages/` | `docs/webinterface.md` |
   | Native apps | `src/Apps/Apps_NativeApps.cpp` | `docs/apps.md` |
   | Settings API keys | `src/DisplayManager/DisplayManager_Settings.cpp` | `docs/api.md` (Settings section) |

5. **Ukrainian translations:**
   - For every English doc change, apply the SAME structural change to the Ukrainian version
   - Translate new content to Ukrainian
   - Keep formatting, headings, and code blocks identical between EN and UK
   - Do NOT machine-translate API field names, JSON keys, or code — keep those as-is

6. **Check VitePress config if navigation changed:**
   - New doc page added → add to sidebar in `docs/.vitepress/config.mts` (both EN and UK sections)
   - Page renamed or removed → update sidebar links

7. **Report:**
   ```
   === Public Docs Sync Report ===
   Branch: feature/xxx (N commits ahead of main)

   Updated (EN):
   - docs/api.md — added new endpoint POST /api/foo, updated MQTT topic list
   - docs/effects.md — added new effect "Aurora" (19→20)

   Updated (UK):
   - docs/uk/api.md — same changes as EN, translated
   - docs/uk/effects.md — same changes as EN, translated

   No changes needed:
   - docs/quickstart.md
   - docs/hardware.md
   - ...

   Counts verified:
   - Effects in docs: 19 ✓
   - MQTT topics in docs: 20 ✓
   - HTTP endpoints in docs: 35 ✓
   - Menu items in docs: 13 ✓
   - dev.json keys in docs: 19 ✓
   ```

## Important rules
- Do NOT rewrite docs from scratch — update incrementally
- Preserve existing structure, formatting, and tone (user-facing, friendly)
- Public docs use **user-facing language** (not developer jargon) — explain what things DO, not how they're implemented
- Screenshots/GIFs in `docs/assets/` — if a visual changed significantly, note it in the report but don't generate images
- Compare against ACTUAL code, not just diff — the diff shows what changed, but the doc must reflect final state
- If a doc section references a feature that no longer exists — remove it
- If new user-facing code has no doc coverage — add a section for it
- Keep EN and UK docs structurally identical (same headings, same order, same code blocks)
- Commit doc updates separately: `docs: sync public documentation with feature/xxx changes`
