# ADR 0001: Interface decoupling via `lib/interfaces/`

**Status:** Accepted
**Date:** 2026-05-12

## Context

Svitrix firmware composes ~15 stateful modules (`DisplayManager`, `MQTTManager`, `ServerManager`, `PeripheryManager`, etc.) that must collaborate at runtime: the web server needs to push notifications to the display; the periphery manager needs to report button presses to both MQTT and HTTP; the display needs sensor readings; and so on.

Three pressures shaped this decision:

1. **Test isolation.** Stateless services in `lib/services/` are testable with `pio test -e native_test`, but stateful modules talk to FastLED, WiFi, AsyncWebServer — none of which run on host. Direct cross-module includes would make the entire dependency tree of any test target pull in Arduino, which doesn't compile natively.
2. **Compile-time coupling cost.** A direct `#include "DisplayManager.h"` in `MQTTManager.cpp` makes touching `DisplayManager`'s header trigger recompilation of MQTT code. With ~15 modules and a ~7-second clean build, transitive recompiles add up quickly.
3. **Cyclic dependencies.** Display → notifications → MQTT → display (re-broadcast state) is a real flow. Without indirection, the include graph forms a cycle that the linker tolerates but humans cannot reason about.

## Decision

Every cross-module call goes through a pure-virtual interface declared in `lib/interfaces/src/I*.h`. Concrete modules implement the interfaces they provide and accept interface references for the consumers they need. **All wiring lives in `src/main.cpp`** — modules never reach out to each other directly.

Concretely:
- Each `I*.h` is header-only, pure-virtual, no implementation.
- Producer module exposes its collaborators through getters returning the concrete sub-object reference — e.g. `DisplayManager_::getRenderer()` → `DisplayRenderer_&`, `DisplayManager_::getNotifier()` → `NotificationManager_&`. The consumer takes the pointer typed as the interface; implicit upcast happens at the call site.
- Consumer module has a setter taking interface pointers — e.g. `MQTTManager_::setDisplay(IDisplayControl*, IDisplayNavigation*, IDisplayNotifier*)`. Setters whose collaborators must be live before `tick()` start `assert()` their arguments; setters whose collaborator is optional (no-op when null) skip the assert.
- `main.cpp::setup()` calls all setters in dependency order before any module starts ticking (see [src/main.cpp](../src/main.cpp) `setup()`).
- The top-level publicly-instantiated module singletons (`DisplayManager_`, `ServerManager_`, `MQTTManager_`, `PeripheryManager_`, `PowerManager_`, `UpdateManager_`, `DataFetcher_`, `MenuManager_`) `= delete` copy/move to prevent accidental duplication. Sub-objects that live by composition inside a module (e.g. `DisplayRenderer_`, `NotificationManager_` inside `DisplayManager_`) don't need the same treatment — they are not standalone singletons.

The complete provider→consumer wiring table is documented in [root CLAUDE.md](../CLAUDE.md#interface-wiring).

## Consequences

**Easier:**
- Any module can be replaced with a `Mock*` for native tests (`test/test_native/test_idisplay/` exercises this).
- Adding a new consumer requires no changes to the producer module's code — just a new wiring line in `main.cpp`.
- Compile-time coupling is bounded by the interface header, which changes infrequently.
- Cycles are broken by definition — the interface is a one-way contract.

**Harder:**
- Indirection adds boilerplate per interface (header + getter + setter + assert).
- A function call goes through a vtable instead of being inlined — measurable cost only in hot paths (effects use the same pattern via `IPixelCanvas` and it's been fine at 30 FPS).
- Wiring errors surface as `assert()` failures at startup rather than link errors — costs one boot cycle to discover.

**Must live with:**
- Discipline. The rule only works if it's never bypassed. The architecture-review agent ([.claude/agents/firmware-architecture-reviewer.md](../.claude/agents/firmware-architecture-reviewer.md)) enforces this on every PR.

## Alternatives considered

- **Global singletons accessed via `extern`**: simpler, what the original Awtrix firmware does. Rejected — makes testing impossible without `#ifdef UNIT_TEST` islands throughout production code, and creates implicit init-order dependencies that surface as random boot crashes when modules grow.
- **Compile-time templates (CRTP / static polymorphism)**: zero-overhead but every consumer becomes a template, propagating into headers, and inflating binary size on a flash-constrained target. Rejected — the runtime cost of vtables is negligible here and the readability cost of templates is high.
- **Mediator object**: one big `App` god-object holding all references and routing calls. Rejected — collapses SRP into a single 500-line class that everyone depends on, defeating the point.
- **Event bus**: pub/sub with stringly-typed topics. Rejected for in-process calls — adds allocation, loses compile-time checks, and is harder to trace. MQTT already serves this role for external integrations.

## References

- See [lib/interfaces/CLAUDE.md](../lib/interfaces/CLAUDE.md) for the full interface catalog.
- See [src/main.cpp](../src/main.cpp) `setup()` for the wiring sequence.
- Related: [ADR 0003](0003-stateless-services.md) (stateless services use a simpler pattern — no interfaces needed since pure functions have no state to inject).
