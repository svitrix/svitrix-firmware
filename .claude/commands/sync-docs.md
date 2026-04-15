---
description: Sync all CLAUDE.md and README.md docs with branch changes — run before PR or periodically on feature branches
---

Analyze ALL changes on the current feature branch vs main and update every affected documentation file to reflect the actual state of the code.

## Steps

1. **Identify branch and all changes:**
   ```bash
   git log --oneline main..HEAD
   git diff --stat main..HEAD
   git diff --name-only main..HEAD
   ```
   If on `main` — use `HEAD~5` instead.
   Report: number of commits, files changed, summary of what was done.

2. **Map changed files to documentation** using this table:

   | Changed files in... | Doc to update |
   |---------------------|---------------|
   | `src/DisplayManager/` | `src/DisplayManager/CLAUDE.md` |
   | `src/MQTTManager/` | `src/MQTTManager/CLAUDE.md` |
   | `src/MatrixDisplayUi/` | `src/MatrixDisplayUi/CLAUDE.md` |
   | `src/DataFetcher/` | `src/DataFetcher/CLAUDE.md` |
   | `src/Apps/` | `src/Apps/README.md` |
   | `src/MelodyPlayer/` | `src/MelodyPlayer/README.md` |
   | `src/effects/` | `src/effects/README.md` |
   | `src/ServerManager/` | `src/ServerManager/CLAUDE.md` |
   | `src/PeripheryManager/` | `src/PeripheryManager/CLAUDE.md` |
   | `src/MenuManager/` | `src/MenuManager/CLAUDE.md` |
   | `src/PowerManager/` | `src/PowerManager/CLAUDE.md` |
   | `src/UpdateManager/` | `src/UpdateManager/CLAUDE.md` |
   | `src/Globals.*`, `src/AppContent.h` | `src/CLAUDE.md` |
   | `lib/interfaces/` | `lib/interfaces/CLAUDE.md` |
   | `lib/services/` | `lib/services/CLAUDE.md` |
   | `lib/config/` | `lib/config/CLAUDE.md` |
   | `lib/home-assistant-integration/` | `lib/home-assistant-integration/CLAUDE.md` |
   | `lib/webserver/` | `lib/webserver/CLAUDE.md` |
   | `web/` | `web/README.md` |
   | `platformio.ini`, `main.cpp`, new modules, interface changes | Root `CLAUDE.md` |
   | `test/` | Update test counts in relevant docs |
   | `.claude/` | Root `CLAUDE.md` (if orchestrator section exists) |

3. **For each affected doc — read code, then update doc:**
   - Read the changed source files (`git diff main..HEAD -- <path>`)
   - Read the current doc file
   - Update ONLY sections affected by the changes:
     - New/removed/renamed classes, methods, interfaces, config fields
     - Changed function signatures, behaviors, endpoints
     - New MQTT topics or HA entities
     - New effects, apps, or test suites
     - Changed dependency wiring in `main.cpp`

4. **Update counts across all docs:**
   Verify these numbers match reality:
   - HA entities count (currently 25) — check `src/MQTTManager/`
   - Effects count (currently 19) — check `src/effects/`
   - API endpoints count — check `src/ServerManager.cpp`
   - Test suites/tests count — run `pio test -e native_test --list-tests` or count test files
   - Interface count (currently 13) — check `lib/interfaces/`
   - Service count (currently 14) — check `lib/services/`

5. **Update root CLAUDE.md if architecture changed:**
   - New module added → update Project Structure tree
   - New interface → update Module Dependency Graph + Interface wiring table
   - New service → update Service consumption map
   - New CLAUDE.md file → update CLAUDE.md reference map

6. **Report:**
   ```
   === Docs Sync Report ===
   Branch: feature/xxx (N commits ahead of main)

   Updated:
   - src/MQTTManager/CLAUDE.md — added new entity X, updated topic list
   - lib/services/CLAUDE.md — added new service Y
   - CLAUDE.md (root) — updated entity count 25→26

   No changes needed:
   - src/DisplayManager/CLAUDE.md
   - ...

   Counts verified:
   - HA entities: 26 ✓
   - Effects: 19 ✓
   - Tests: 24 suites, 440 tests ✓
   ```

## Important rules
- Do NOT rewrite docs from scratch — update incrementally
- Preserve existing structure and formatting
- Compare against ACTUAL code, not just diff — the diff shows what changed, but the doc must reflect final state
- If a doc section references code that no longer exists — remove it
- If new code has no doc coverage — add a section for it
- Commit doc updates separately: `docs: sync documentation with feature/xxx changes`
