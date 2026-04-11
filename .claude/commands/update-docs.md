---
description: Update CLAUDE.md documentation files after code changes
---

Scan recent code changes and update the corresponding CLAUDE.md / README.md files to keep AI documentation in sync.

## Process

1. **Identify what changed:**
   ```bash
   git diff --name-only HEAD~5
   ```
   Or if on a feature branch:
   ```bash
   git diff --name-only main...HEAD
   ```

2. **Map changes to documentation files:**
   Use this mapping to determine which docs need updating:

   | Changed files in... | Update doc |
   |---------------------|------------|
   | `src/DisplayManager/` | `src/DisplayManager/CLAUDE.md` |
   | `src/MQTTManager/` | `src/MQTTManager/CLAUDE.md` |
   | `src/MatrixDisplayUi/` | `src/MatrixDisplayUi/CLAUDE.md` |
   | `src/DataFetcher/` | `src/DataFetcher/CLAUDE.md` |
   | `src/Apps/` | `src/Apps/README.md` |
   | `src/Games/` | `src/Games/README.md` |
   | `src/MelodyPlayer/` | `src/MelodyPlayer/README.md` |
   | `src/effects/` | `src/effects/README.md` |
   | `src/ServerManager.*`, `src/PeripheryManager.*`, `src/MenuManager.*`, `src/Globals.*`, `src/AppContentRenderer.*` | `src/CLAUDE.md` |
   | `lib/interfaces/` | `lib/interfaces/CLAUDE.md` |
   | `lib/services/` | `lib/services/CLAUDE.md` |
   | `lib/config/` | `lib/config/CLAUDE.md` |
   | `lib/home-assistant-integration/` | `lib/home-assistant-integration/CLAUDE.md` |
   | `lib/webserver/` | `lib/webserver/CLAUDE.md` |
   | `web/` | `web/README.md` |
   | Architecture changes | Root `CLAUDE.md` |

3. **For each affected doc file:**
   - Read the current doc
   - Read the changed source files
   - Update the doc to reflect:
     - New/removed/renamed classes, methods, or interfaces
     - Changed function signatures or behavior
     - New config fields or MQTT topics
     - Updated counts (entities, effects, endpoints, tests)
     - New dependencies or wiring changes

4. **Verify consistency:**
   - Cross-check counts: "25 HA entities", "19 effects", "33 endpoints", etc.
   - Verify interface lists match actual code
   - Check that module dependency graph in root CLAUDE.md is still accurate

5. **Report what was updated:**
   List each doc file changed with a brief summary of what was updated.

## Important
- Do NOT rewrite docs from scratch — update incrementally
- Preserve the existing structure and formatting of each doc
- Only update sections that are affected by the code changes
- If a doc has no changes needed, skip it
