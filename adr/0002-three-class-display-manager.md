# ADR 0002: DisplayManager split into three classes

**Status:** Accepted
**Date:** 2026-05-12

## Context

The display subsystem has three distinct responsibilities that originally lived in one class:

1. **Coordination** — running the app rotation, deciding what to draw on each frame, holding the `MatrixDisplayUi` state machine, owning brightness/power state.
2. **Rendering** — converting logical draw operations into pixel writes on the `NeoMatrix` LED buffer (text, icons, transitions, gamma correction).
3. **Notifications** — queueing transient overlays (incoming MQTT/HTTP notifications), priority handling, expiration timers.

These had grown into a single ~1500-line class with mixed concerns. Symptoms:

- Touching notification queueing logic could break app rendering (and did, repeatedly).
- The class was untestable as a whole — its public API was a leaky abstraction over three unrelated state machines.
- New developers (and AI sessions) couldn't predict where to make a change.

## Decision

Split into three cooperating classes, each in its own file under `src/DisplayManager/`:

- **`DisplayManager_`** — coordinator. Owns the app rotation, the active `MatrixDisplayUi` instance, brightness state, and lifecycle (`tick()`, `setup()`). Holds references to the other two and dispatches to them.
- **`DisplayRenderer_`** — pure rendering. All `matrixPrint()`, icon draws, transition blends, gamma, color correction. Implements `IDisplayRenderer`. No knowledge of apps or notifications.
- **`NotificationManager_`** — notification lifecycle. Queue, priority, expiration. Implements `IDisplayNotifier`. Asks the renderer to draw when active, asks the coordinator for screen time.

`DisplayManager_` is the publicly-instantiated singleton (`= delete` copy/move). `DisplayRenderer_` and `NotificationManager_` live by composition inside it — they are owned, never directly constructed by callers — so they don't carry the `= delete` boilerplate themselves. Wiring in [src/main.cpp](../src/main.cpp).

## Consequences

**Easier:**
- Each class has one reason to change — SRP applies cleanly.
- `DisplayRenderer_` is reusable: `UpdateManager`, `MenuManager`, `ServerManager` all consume `IDisplayRenderer` for their own UI without going through coordination logic.
- Notification logic is testable in isolation through the `IDisplayNotifier` interface — `test/test_native/test_idisplay/` exercises `test_notifier_dismiss`, `test_notifier_indicator_state`, `test_notifier_indicator_color` against a mock implementation, no FastLED / NeoMatrix link required.
- The split keeps the coordinator and notification manager small (≲300 LOC each). The renderer and the custom-apps file drift longer because they accumulate effect / parser tables — that's acceptable: they grow along a single axis (more effects / more parser branches), not along multiple responsibilities.

**Harder:**
- Three classes to onboard instead of one. Mitigated by [src/DisplayManager/CLAUDE.md](../src/DisplayManager/CLAUDE.md) explaining the split.
- Coordinator must mediate between the other two — adds some delegation boilerplate.

**Must live with:**
- The split must not be re-collapsed. The architecture-review agent flags any attempt to merge two of the three or add a fourth class to this module.

## Alternatives considered

- **Keep as one class, refactor internally** — would only paper over the problem; SRP doesn't apply when one type holds three unrelated responsibilities behind a "convenient" public API.
- **Two classes (`DisplayManager_` + `Renderer_`)** with notifications kept in the coordinator — rejected. Notification expiration logic is its own state machine and pollutes coordinator code; same testability problem.
- **Four+ classes (split renderer into TextRenderer / GraphicsRenderer / TransitionRenderer)** — rejected as premature. The renderer is ~600 lines, navigable, and the sub-responsibilities are tightly coupled by shared canvas state.

## References

- [src/DisplayManager/CLAUDE.md](../src/DisplayManager/CLAUDE.md) — module doc explaining the three classes
- [ADR 0001](0001-interface-decoupling.md) — the interface pattern this split relies on
- Provider→consumer wiring in [root CLAUDE.md](../CLAUDE.md#interface-wiring)
