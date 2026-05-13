# Architecture Decision Records

Lightweight records of architectural decisions taken on this project. Format based on [Michael Nygard's template](https://cognitect.com/blog/2011/11/15/documenting-architecture-decisions).

## Why ADRs

Code shows *what* the system does. Comments rot. Commit messages get buried. ADRs capture *why* a decision was made — the constraints, the trade-offs, the alternatives that were considered — so future-you (and future AI sessions) don't have to reverse-engineer the reasoning.

ADRs are **immutable once accepted**. If a decision is revisited, write a new ADR that supersedes the old one. Never edit history.

## When to write an ADR

Write one when you take a decision that future-you will probably question. In this project, the trigger conditions match the architecture-review agent's checklist (see [.claude/agents/firmware-architecture-reviewer.md](../.claude/agents/firmware-architecture-reviewer.md) §5):

- New interface in `lib/interfaces/src/`
- New top-level module under `src/`
- Change to persistence model (NVS layout, LittleFS structure, OTA partition table)
- New external protocol integration (new MQTT topic family, new HTTP API surface, new external data source)
- Change to wiring/composition pattern in `src/main.cpp`
- Adoption of a project-wide standard (coding standard, dependency rule, release process)

**Don't write ADRs for:** routine refactors, bug fixes, dependency bumps, doc-only changes, new visual effects (those plug into existing extension points).

## Format

Each ADR lives in `adr/NNNN-kebab-case-title.md` where `NNNN` is the next zero-padded number.

```markdown
# ADR NNNN: <Title>

**Status:** Proposed | Accepted | Superseded by ADR-XXXX | Deprecated
**Date:** YYYY-MM-DD

## Context

What forced this decision? What constraints were in play? Anything we tried and rejected before landing here?

## Decision

The chosen approach, stated clearly. One paragraph. No hedging.

## Consequences

What becomes easier, what becomes harder, what we now must live with. Include both the wins and the costs.

## Alternatives considered

Briefly: each alternative, why it was rejected.
```

## Index

| ADR | Title | Status | Date |
|-----|-------|--------|------|
| [0001](0001-interface-decoupling.md) | Interface decoupling via `lib/interfaces/` | Accepted | 2026-05-12 |
| [0002](0002-three-class-display-manager.md) | DisplayManager split into three classes | Accepted | 2026-05-12 |
| [0003](0003-stateless-services.md) | Stateless service libraries under `lib/services/` | Accepted | 2026-05-12 |
| [0004](0004-clean-code-and-architecture.md) | Clean Code & Architecture as project standard | Accepted | 2026-05-12 |
| [0005](0005-signed-ota-phase-1.md) | Signed OTA — Phase 1 (structural pre-check) | Accepted | 2026-05-13 |

## Conventions

- Numbering is monotonic — never reuse a number, even after deprecation
- File names use kebab-case, ASCII only
- "Accepted" means it's live in the codebase. "Proposed" means under discussion (rare for solo project but allowed)
- Reference ADRs from code comments only when the *why* is non-obvious and the ADR is the canonical source
