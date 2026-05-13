# ADR 0003: Stateless service libraries under `lib/services/`

**Status:** Accepted
**Date:** 2026-05-12

## Context

Many useful computations in the firmware are pure functions that don't depend on hardware: color math, gamma correction, sensor calibration curves, MQTT message routing, HA discovery payload generation, text layout, format-string validation. Originally these were inline methods on stateful modules (`DisplayManager_::applyGamma()`, `MQTTManager_::routeMessage()`).

Two problems with that arrangement:

1. **Untestable on host.** A test for gamma correction shouldn't require linking FastLED, WiFi, and the AsyncWebServer. But because the methods were members of stateful classes pulled into those frameworks, native tests for pure logic were impossible.
2. **Reuse blocked by ownership.** When `MQTTManager_` and `ServerManager_` both needed to format the same stats payload, the code was either duplicated or one module had to ask the other — creating an artificial dependency between two adapters.

## Decision

Pure logic lives in `lib/services/` as **stateless** libraries. Each service is an `.h` exporting free or namespaced functions, with a matching `.cpp` when bodies aren't trivially `inline` / `constexpr`. Rules:

- **No state.** No globals, no statics holding mutable data, no `millis()`, no `WiFi.*`. Time and state come in via parameters.
- **No Arduino-only headers** outside `#ifdef UNIT_TEST` blocks. Services must compile in the `native_test` environment.
- **No ArduinoJson dependency in the service core** — build JSON manually if needed, or accept a `JsonObject&` from the caller. (ArduinoJson works in native_test but we keep services framework-free as a discipline.)
- **Every service has a Unity test suite** under `test/test_native/test_<snake_case>/`. (Line/branch coverage is not measured in CI today — the rule is "no service ships without its own suite", enforced at PR review.)

The current service catalog lives in [lib/services/CLAUDE.md](../lib/services/CLAUDE.md). Services are added without rewriting this ADR — what's fixed here is the rule, not the inventory.

## Consequences

**Easier:**
- The whole native test suite runs on host in well under a minute, no hardware loop — fast enough to keep `pio test -e native_test` in the inner refactor loop.
- A new contributor can read any service's `.h` + its test file in one sitting and understand the contract without grepping for hidden state.
- Refactoring pure logic is safe — the test suite catches regressions immediately.
- New consumers can use existing services without negotiating with their original "owner" module.
- Services are easy to reason about: same input → same output, always.

**Harder:**
- Service functions take more parameters than equivalent member functions (`millis()` becomes `uint32_t nowMs`). Verbose at call sites; acceptable for the clarity gain.
- A new service requires a new test suite. Friction by design — discourages putting one-off logic in `lib/services/` when it belongs in a module.

**Must live with:**
- The stateless rule is non-negotiable. Once a service calls `millis()` or reads a global, the test isolation collapses and the rule's value evaporates. The architecture-review agent enforces this.

## Alternatives considered

- **Static methods on stateful classes** — what we had. Rejected for the reasons above.
- **Singletons under `lib/services/`** with init/teardown — gives stateful behavior to services. Rejected: defeats the purpose. If state is needed, it belongs in a module under `src/`, not in services.
- **Free functions in `src/`** — would work but mixes "pure logic" with "modules". The `lib/` separation makes the rule visible at the directory level and gates inclusion of Arduino headers via `library.json`.

## References

- [lib/services/CLAUDE.md](../lib/services/CLAUDE.md) — service catalog and rules
- [/add-service slash command](../.claude/commands/add-service.md) — recipe for adding a new service
- [ADR 0001](0001-interface-decoupling.md) — interfaces for stateful modules (services don't need interfaces; pure functions have no state to inject)
