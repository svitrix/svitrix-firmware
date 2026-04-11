# src/ Structure Reorganization Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Eliminate "scattered files" feeling by moving loose managers into directories, grouping data/contrib headers, and cleaning up stubs — without changing any runtime behavior.

**Architecture:** PlatformIO resolves includes via explicit `-I` flags in `platformio.ini`. Moving files to subdirectories and adding `-Isrc/NewDir` flags means ALL existing `#include` lines remain unchanged. Zero include-path edits in consumer files.

**Tech Stack:** C++17, PlatformIO, Arduino framework

**Key constraint:** Flash is 96% full. This refactor must produce identical binary output.

---

## File Structure (before → after)

```
src/
├── main.cpp                          ← stays (composition root)
├── AppContent.h                      ← stays (shared struct, 3 consumers)
├── Globals.cpp/h                     ← stays (23 consumers, too disruptive to move)
├── Functions.cpp/h                   ← stays (11 consumers, utility)
├── effects.h                         ← DELETE (13-line stub, consumers switch to effects/)
├── icons.cpp                         ← DELETE (1 LOC, empty)
├── timer.cpp/h                       ← stays (small utility, 5 consumers)
│
├── ServerManager/                    ← NEW (was loose)
│   ├── ServerManager.cpp
│   └── ServerManager.h
├── PeripheryManager/                 ← NEW (was loose)
│   ├── PeripheryManager.cpp
│   └── PeripheryManager.h
├── MenuManager/                      ← NEW (was loose)
│   ├── MenuManager.cpp
│   └── MenuManager.h
├── UpdateManager/                    ← NEW (was loose)
│   ├── UpdateManager.cpp
│   └── UpdateManager.h
├── PowerManager/                     ← NEW (was loose)
│   ├── PowerManager.cpp
│   └── PowerManager.h
├── Overlays/                         ← NEW (was loose)
│   ├── Overlays.cpp
│   └── Overlays.h
│
├── DisplayManager/                   ← existing, add AppContentRenderer
│   ├── AppContentRenderer.cpp        ← MOVED from src/ (orphan, belongs here)
│   └── ...existing
│
├── data/                             ← NEW (large data/header-only blobs)
│   ├── SvitrixFont.h                 ← MOVED from src/
│   ├── cert.h                        ← MOVED from src/
│   ├── icons.h                       ← MOVED from src/
│   └── Dictionary.cpp/h              ← MOVED from src/
│
├── contrib/                          ← NEW (third-party header-only libs)
│   ├── GifPlayer.h                   ← MOVED from src/
│   └── ArtnetWifi.h                  ← MOVED from src/
│
├── MQTTManager/                      ← unchanged
├── MatrixDisplayUi/                  ← unchanged
├── Apps/                             ← unchanged
├── DataFetcher/                      ← unchanged
├── MelodyPlayer/                     ← unchanged (rename deferred)
├── Games/                            ← unchanged
└── effects/                          ← unchanged
```

**NOT moving (too many consumers, low benefit):**
- `Globals.h` — 23 files include it, stays at src/ root
- `Functions.h` — 11 files include it, stays at src/ root
- `timer.h` — 5 files, trivial, stays
- `AppContent.h` — 3 files, shared struct, stays

**NOT renaming MelodyPlayer files** — snake_case → PascalCase is cosmetic churn with risk of breaking includes. Defer to a separate PR if desired.

---

## Task 1: Move 5 manager modules into directories

**Files:**
- Move: `src/ServerManager.cpp/h` → `src/ServerManager/`
- Move: `src/PeripheryManager.cpp/h` → `src/PeripheryManager/`
- Move: `src/MenuManager.cpp/h` → `src/MenuManager/`
- Move: `src/UpdateManager.cpp/h` → `src/UpdateManager/`
- Move: `src/PowerManager.cpp/h` → `src/PowerManager/`
- Modify: `platformio.ini`

- [ ] **Step 1: Create directories and move files**

```bash
mkdir -p src/ServerManager src/PeripheryManager src/MenuManager src/UpdateManager src/PowerManager

git mv src/ServerManager.cpp src/ServerManager/
git mv src/ServerManager.h src/ServerManager/
git mv src/PeripheryManager.cpp src/PeripheryManager/
git mv src/PeripheryManager.h src/PeripheryManager/
git mv src/MenuManager.cpp src/MenuManager/
git mv src/MenuManager.h src/MenuManager/
git mv src/UpdateManager.cpp src/UpdateManager/
git mv src/UpdateManager.h src/UpdateManager/
git mv src/PowerManager.cpp src/PowerManager/
git mv src/PowerManager.h src/PowerManager/
```

- [ ] **Step 2: Add -I flags to platformio.ini**

In `[env:ulanzi]` `build_flags`, add after existing `-I` flags:

```
    -Isrc/ServerManager
    -Isrc/PeripheryManager
    -Isrc/MenuManager
    -Isrc/UpdateManager
    -Isrc/PowerManager
```

- [ ] **Step 3: Fix relative include in MelodyPlayer**

`src/MelodyPlayer/melody_player.cpp` uses `#include "../Globals.h"`. Since Globals stays at `src/`, this path still works. No change needed.

Verify no other relative paths break:
```bash
grep -rn '\.\.\/' src/ServerManager/ src/PeripheryManager/ src/MenuManager/ src/UpdateManager/ src/PowerManager/
```

- [ ] **Step 4: Build and test**

```bash
pio run -e ulanzi
pio test -e native_test
```

Expected: clean build, all 25 suites pass, identical binary size.

- [ ] **Step 5: Commit**

```bash
git add -A
git commit -m "refactor: move 5 manager modules into own directories

Move ServerManager, PeripheryManager, MenuManager, UpdateManager,
PowerManager from loose src/ files into dedicated directories.
Add -I flags to platformio.ini for include resolution."
```

---

## Task 2: Move Overlays into directory

**Files:**
- Move: `src/Overlays.cpp/h` → `src/Overlays/`
- Modify: `platformio.ini`

- [ ] **Step 1: Create directory and move files**

```bash
mkdir -p src/Overlays
git mv src/Overlays.cpp src/Overlays/
git mv src/Overlays.h src/Overlays/
```

- [ ] **Step 2: Add -I flag to platformio.ini**

Add to `[env:ulanzi]` `build_flags`:
```
    -Isrc/Overlays
```

- [ ] **Step 3: Build and test**

```bash
pio run -e ulanzi
pio test -e native_test
```

- [ ] **Step 4: Commit**

```bash
git add -A
git commit -m "refactor: move Overlays into own directory"
```

---

## Task 3: Move AppContentRenderer into DisplayManager

**Files:**
- Move: `src/AppContentRenderer.cpp` → `src/DisplayManager/`

- [ ] **Step 1: Move file**

```bash
git mv src/AppContentRenderer.cpp src/DisplayManager/AppContentRenderer.cpp
```

No `-I` flag needed — `DisplayManager/` is already in the include path.

- [ ] **Step 2: Build and test**

```bash
pio run -e ulanzi
pio test -e native_test
```

- [ ] **Step 3: Commit**

```bash
git add -A
git commit -m "refactor(display): move AppContentRenderer into DisplayManager"
```

---

## Task 4: Group data/header-only files into src/data/

**Files:**
- Move: `src/SvitrixFont.h` → `src/data/`
- Move: `src/cert.h` → `src/data/`
- Move: `src/icons.h` → `src/data/`
- Move: `src/Dictionary.cpp/h` → `src/data/`
- Delete: `src/icons.cpp` (1 LOC, empty implementation)
- Modify: `platformio.ini`

- [ ] **Step 1: Verify icons.cpp is truly empty**

```bash
cat src/icons.cpp
```

Expected: only `#include "icons.h"` — no actual code.

- [ ] **Step 2: Create directory, move files, delete stub**

```bash
mkdir -p src/data
git mv src/SvitrixFont.h src/data/
git mv src/cert.h src/data/
git mv src/icons.h src/data/
git mv src/Dictionary.cpp src/data/
git mv src/Dictionary.h src/data/
git rm src/icons.cpp
```

- [ ] **Step 3: Add -I flag to platformio.ini**

Add to both `[env:ulanzi]` and `[env:native_test]` `build_flags`:
```
    -Isrc/data
```

(native_test needs it because test_unicode_font, test_text_metrics, test_glyph_render include SvitrixFont.h)

- [ ] **Step 4: Build and test**

```bash
pio run -e ulanzi
pio test -e native_test
```

- [ ] **Step 5: Commit**

```bash
git add -A
git commit -m "refactor: group data headers into src/data/

Move SvitrixFont.h, cert.h, icons.h, Dictionary.cpp/h into src/data/.
Remove empty icons.cpp stub."
```

---

## Task 5: Group third-party contrib headers into src/contrib/

**Files:**
- Move: `src/GifPlayer.h` → `src/contrib/`
- Move: `src/ArtnetWifi.h` → `src/contrib/`
- Modify: `platformio.ini`

- [ ] **Step 1: Create directory and move files**

```bash
mkdir -p src/contrib
git mv src/GifPlayer.h src/contrib/
git mv src/ArtnetWifi.h src/contrib/
```

- [ ] **Step 2: Add -I flag to platformio.ini**

Add to `[env:ulanzi]` `build_flags`:
```
    -Isrc/contrib
```

- [ ] **Step 3: Build and test**

```bash
pio run -e ulanzi
pio test -e native_test
```

- [ ] **Step 4: Commit**

```bash
git add -A
git commit -m "refactor: group third-party headers into src/contrib/"
```

---

## Task 6: Delete effects.h stub

**Files:**
- Delete: `src/effects.h` (13-line stub)
- Modify: all files that `#include "effects.h"` → change to `#include "EffectTypes.h"` or the actual header from `src/effects/`

- [ ] **Step 1: Examine effects.h content**

```bash
cat src/effects.h
```

Determine what it exports — likely just includes/forwards from `src/effects/`.

- [ ] **Step 2: Check what consumers actually need**

The 11 files that include `effects.h` need access to effect types and functions. Check what `effects.h` provides and find the actual header in `src/effects/` that provides the same declarations.

If `effects.h` just includes `effects/EffectTypes.h` and `effects/EffectRegistry.h`, then consumers can include those directly. Since `src/effects` is already in the include path (`-Isrc/effects` in native_test, and effects/ files compile as part of src/), add `-Isrc/effects` to `[env:ulanzi]` if not present.

- [ ] **Step 3: Update includes in all 11 consumer files**

Replace `#include "effects.h"` with the actual headers from `src/effects/` in:
- `src/Overlays/Overlays.h`
- `src/Overlays/Overlays.cpp`
- `src/Globals.cpp`
- `src/DisplayManager/NotificationManager.cpp`
- `src/DisplayManager/DisplayManager_Settings.cpp`
- `src/DisplayManager/DisplayManager_ParseHelpers.cpp`
- `src/DisplayManager/DisplayManager_CustomApps.cpp`
- `src/DisplayManager/DisplayManager.cpp`
- `src/Apps/Apps_CustomApp.cpp`
- `src/Apps/Apps.h`

- [ ] **Step 4: Delete the stub**

```bash
git rm src/effects.h
```

- [ ] **Step 5: Add -I flag if needed**

Add to `[env:ulanzi]` `build_flags` if not already present:
```
    -Isrc/effects
```

- [ ] **Step 6: Build and test**

```bash
pio run -e ulanzi
pio test -e native_test
```

- [ ] **Step 7: Commit**

```bash
git add -A
git commit -m "refactor: remove effects.h stub, use direct includes from effects/"
```

---

## Task 7: Update documentation

**Files:**
- Modify: `CLAUDE.md` (root) — update Project Structure tree
- Modify: `src/CLAUDE.md` — update file references

- [ ] **Step 1: Update root CLAUDE.md Project Structure**

Replace the `src/` tree in the Project Structure section to reflect the new layout:
- ServerManager, PeripheryManager, MenuManager, UpdateManager, PowerManager as directories
- data/ and contrib/ directories
- AppContentRenderer inside DisplayManager/
- Remove effects.h, icons.cpp from listing

- [ ] **Step 2: Update src/CLAUDE.md**

Update file paths and any references to moved files.

- [ ] **Step 3: Commit**

```bash
git add CLAUDE.md src/CLAUDE.md
git commit -m "docs: update project structure after src/ reorganization"
```

---

## Verification Checklist

After all tasks:

- [ ] `pio run -e ulanzi` builds cleanly
- [ ] `pio test -e native_test` — all 25 suites, 445 tests pass
- [ ] Binary size unchanged (compare before/after with `stat .pio/build/ulanzi/firmware.bin`)
- [ ] No loose .cpp/.h manager files remain in src/ root (only main.cpp, Globals, Functions, AppContent.h, timer)
- [ ] `git status` clean
