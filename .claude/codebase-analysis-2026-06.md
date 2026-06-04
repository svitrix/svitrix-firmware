# Codebase Analysis — June 2026

Point-in-time analysis of improvement opportunities across firmware, web UI, configuration, and architecture.

## Completed Fixes (June 2026)

### Phase 1: Firmware Quick Wins
- [x] `DynamicJsonDocument` → `StaticJsonDocument<8192>` in `DisplayRenderer.cpp:406`
- [x] Base64 icon size validation (16KB cap) in `DisplayManager_ParseHelpers.cpp:53`
- [x] Non-blocking MQTT subscriptions via deferred queue in `MQTTManager.cpp`

### Phase 2: Config Validation
- [x] `validateSettings()` function added to `Globals.cpp` — clamps 25+ config values
- [x] Called from `loadSettings()`, `importSettings()`, and `setNewSettings()`
- [x] Validated: brightness (0-255), night time (0-1439), durations, volumes, enums
- [x] `showAlarms` verified as active code (controls indicator 3 for alarm state)

### Phase 3: Date/Time Format Sync
- [x] Unified time formats (12 options) — same in MenuManager + Web UI
- [x] Unified date formats (9 options) — same in both
- [x] `findFormatIndex()` syncs menu index when entering format submenus
- [x] Changed `%l` → `%I` for 12-hour format consistency

### Phase 4: Web Cleanup + Auto-Save
- [x] `useAutoSave` hook with debounce (500ms) for sliders/inputs
- [x] `instantSave` for toggles/selects — immediate effect
- [x] Error logging added to all API catch blocks (was silent)
- [x] `prepareForSave` extracted to shared hook (was duplicated)
- [x] Updated: DisplaySection, NightModeSection, SoundSection (no save button needed)
- [x] Bundle size reduced: 33.35 → 33.17 kB gzip

## Critical Issues (High Impact)

### Firmware — Blocking Operations in Main Loop

| Issue | Location | Impact |
|-------|----------|--------|
| HTTP POST blocks loop during notification forwarding | `src/DisplayManager/NotificationManager.cpp:140-144` | Loop freezes 1-10s if remote endpoint hangs |
| 8KB DynamicJsonDocument allocated every frame | `src/DisplayManager/DisplayRenderer.cpp:408` | Heap fragmentation, forces GC on every draw |
| 30ms `delay()` between MQTT subscribes | `src/MQTTManager/MQTTManager.cpp:234-240` | 20 topics = 600ms blocked on reconnect |

### Architecture — Hidden Coupling

14 direct `#include` statements bypass the interface layer:

| File | Direct Includes |
|------|-----------------|
| `src/Overlays/Overlays.cpp` | DisplayManager.h, MenuManager.h, PeripheryManager.h, MQTTManager.h |
| `src/Apps/Apps_CustomApp.cpp` | MQTTManager.h, DisplayManager.h |
| `src/Apps/Apps_Helpers.cpp` | MQTTManager.h |
| `src/Apps/AppContentRenderer.cpp` | DisplayManager.h |
| `src/MenuManager/MenuManager.cpp` | ServerManager.h |
| `src/Globals.cpp` | DisplayManager.h |

**Impact:** Circular dependency risk, harder testing, tight coupling.

---

## Important Issues (Significant Improvement)

### Firmware — Memory & Safety

| Issue | Location | Risk |
|-------|----------|------|
| MQTT topic vector grows unbounded | `MQTTManager.cpp:363` | Memory leak with 100+ topics |
| Base64 decoded without size validation | `NotificationManager.cpp:92` | OOM crash with malicious payload |
| Large stack buffers in potential ISR context | `MQTTManager_StateUpdates.cpp:26+` | Stack overflow risk |
| String concatenation in loops | `main.cpp:172-176` | Multiple heap allocations |

### Config — Structure Problems

| Issue | Details |
|-------|---------|
| Runtime state in config structs | `SensorConfig.{currentTemp, currentHum, currentLux, ldrRaw}` should be separate `SensorState` |
| Dead toggle | `AppConfig.showAlarms` parsed from NVS but never rendered in UI |
| Missing NVS validation | `brightness`, `tempDecimalPlaces`, `nightStart/End` accept any value |
| Defaults mismatch | `soundVolume` default 30 in code, loaded as 25 from NVS |

### Web UI — Error Handling

| Issue | Location |
|-------|----------|
| Silent error swallowing | `SettingsContext.tsx:73-79` uses `.catch(() => {})` |
| Generic API errors | `api/client.ts:17-20` throws without status/context |
| No loading states | `IconPickerSection.tsx`, `PlaylistSection.tsx` |

### Architecture — Responsibility Leaks

**DisplayManager** implements 4 interfaces + owns 2 sub-components + spans 10 implementation files:
- `DisplayManager_CustomApps.cpp` (745 LOC)
- `DisplayManager_Settings.cpp` (270 LOC)
- `DisplayManager_Artnet.cpp` (138 LOC)
- `NotificationManager.cpp` (280 LOC)
- `AppContentRenderer.cpp` (213 LOC)

**Proposal:** Extract `PlaylistManager` + `IPlaylistProvider` interface.

---

## Quality Improvements

### Firmware — Code Duplication

- Identical callback dispatch loops repeated 4+ times across PeripheryManager and DisplayManager
- Fix: Extract template `dispatchCallback()` helper

### Web UI — Duplication & Size

| Issue | Files Affected |
|-------|----------------|
| Color conversion logic duplicated | `NotifySection.tsx`, `SettingsContext.tsx`, `ColorField.tsx` |
| App label switch duplicated | `AppOrderSection.tsx`, `PlaylistSection.tsx` |
| PlaylistSection too large | 245 lines with inline modal |
| Weather rows repeated | `WeatherAppsSection.tsx` — same layout 5× |

### Config — Simplification Opportunities

| Current | Proposed |
|---------|----------|
| 5 separate app duration fields | 1 `defaultAppDuration` + per-app overrides |
| 11 color override fields | `textColor` + `accentColor` + optional per-app |
| 21 weather config fields | Group by location/display/colors |
| Advanced LDR settings exposed | Move gamma/factor/orientation to `/dev.json` |

### Web UI — Accessibility

- ColorField missing `aria-label`
- Empty labels in WeatherAppsSection
- Modal in PlaylistSection has no focus trap or escape-key handling

---

## What's Working Well

- **Services layer** (`lib/services/`) — Clean, stateless, no circular dependencies, 100% test coverage
- **Button handler pattern** — Vector of handlers scales well for multiple subscribers
- **Interface design** — Solid foundation, just needs discipline enforcement
- **Effect system** — `IPixelCanvas` abstraction enables hardware-free testing

---

## Recommended Action Plan

### Phase 1: Quick Wins (~1 hour)
1. Fix DynamicJsonDocument per-frame allocation
2. Replace `delay()` with timer-based stagger in MQTT
3. Add base64 size validation

### Phase 2: Config Cleanup (~2 hours)
1. Extract runtime state to separate structs
2. Add NVS range validation
3. Audit and remove dead toggles

### Phase 3: Architecture Refactor (~4 hours)
1. Remove direct includes, create missing interfaces
2. Extract PlaylistManager from DisplayManager
3. Consolidate display interface setters

### Phase 4: Web Cleanup (~2 hours)
1. Extract color utilities to shared module
2. Add error handling to SettingsContext
3. Split oversized components

---

## File References

Quick access to key locations:

```
# Critical firmware files
src/DisplayManager/NotificationManager.cpp
src/DisplayManager/DisplayRenderer.cpp
src/MQTTManager/MQTTManager.cpp

# Config
lib/config/src/ConfigTypes.h
src/Globals.cpp

# Web state
web/src/context/SettingsContext.tsx
web/src/api/client.ts

# Architecture
src/main.cpp (wiring)
lib/interfaces/ (all interfaces)
```
