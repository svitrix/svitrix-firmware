# ADR 0004: Clean Code & Architecture as project standard

**Status:** Accepted
**Date:** 2026-05-12

## Context

The project has accumulated three layers of conventions:

1. **Embedded C++ rules** in `~/Work/CLAUDE.md` (no malloc after init, fixed-width types, no exceptions, ISR discipline, banned functions) — non-negotiable, derived from the platform constraints of the ESP32 / FreeRTOS environment.
2. **Project architecture rules** in [root CLAUDE.md](../CLAUDE.md) (interface decoupling, stateless services, `#ifdef ULANZI` for hardware-specific code) — already documented per [ADR 0001](0001-interface-decoupling.md), [ADR 0002](0002-three-class-display-manager.md), [ADR 0003](0003-stateless-services.md).
3. **General software engineering quality** — was implicit. Tied to the developer's individual judgment, with no shared vocabulary or checklist.

That third layer needs to be made explicit so that reviews, code-review agents, and future contributions converge on the same notion of "good code" rather than rediscovering it case by case.

## Decision

All implementations and design choices follow the principles from Robert C. Martin's **Clean Code** and **Clean Architecture**, applied *within* the embedded constraints.

**At the implementation level (Clean Code):**
- Meaningful names — functions and variables reveal intent
- Small functions — one thing, one level of abstraction, ≤ ~40 lines (aligns with embedded rule §[12])
- Comments explain *why*, not *what* — code that needs *what*-comments should be renamed instead
- Pure functions / no hidden side effects in `lib/services/` (already enforced)
- Errors as return values, no silent ignores (already enforced)
- DRY, but not premature: three similar lines beat a wrong abstraction
- Boy Scout Rule: leave touched code cleaner than found — within scope

**At the design level (Clean Architecture / SOLID):**
- **Dependency Rule** — source-code dependencies point inward; high-level policy doesn't depend on low-level detail. `src/<X>/` depends on `lib/interfaces/I*`, not on other `src/` modules.
- **SRP** — one reason to change per class/file
- **OCP** — extend via new effects / policies / services without modifying existing ones (e.g., `EffectRegistry`, `IDisplayPolicy`, `lib/services/`)
- **LSP** — interface implementors honor the contract; no surprise asserts or null returns
- **ISP** — small focused interfaces (`IDisplayControl` / `IDisplayNavigation` split is the canonical example)
- **DIP** — depend on abstractions; setter-injected interfaces in `main.cpp` enforce this

**Precedence when rules conflict:** embedded rules win. Clean Code idioms that contradict embedded constraints (exceptions, polymorphism in hot paths, virtual calls in ISR, dynamic dispatch in tight loops) are disallowed by the platform — apply Clean Code *within* that envelope, not around it.

## Consequences

**Easier:**
- Reviewers (human and agent) have a named, shared standard to point at — instead of "this feels off", they can say "SRP: this class has three responsibilities".
- Onboarding gets cheaper: a developer who already knows SOLID maps directly onto this codebase.
- Decisions are easier to justify in commit messages and ADRs.

**Harder:**
- Higher bar for "good enough". Some quick hacks that would have shipped before will now get pushback.
- Some Clean Code idioms (polymorphism via interfaces everywhere) need translation to the embedded reality (vtable cost, no exceptions). Requires judgment per case.

**Must live with:**
- The standard is enforced by [.claude/agents/pr-reviewer.md](../.claude/agents/pr-reviewer.md) §3a and [.claude/agents/firmware-architecture-reviewer.md](../.claude/agents/firmware-architecture-reviewer.md) §4 on every PR.

## Alternatives considered

- **No explicit standard, rely on taste** — what we had. Rejected: doesn't scale beyond the original author, and even for solo work the AI agents need a written reference.
- **Adopt MISRA C++ 2008 or AUTOSAR C++14 in full** — too rigid for a hobby/prosumer LED clock; large parts of MISRA target safety-critical certification, not consumer firmware. The embedded rules in `~/Work/CLAUDE.md` already cover the MISRA subset that matters here.
- **Google C++ Style Guide** — has useful pieces but is opinionated about choices (no exceptions, no streams) that we already enforce, and on others (smart pointers, ownership) that don't fit this environment. Cherry-picking from it is not a coherent standard.
- **"Clean Code" only, no architecture-level principles** — leaves the bigger question of how modules relate unspoken. Architecture is where the costly mistakes are made; covering it explicitly is the higher-leverage half.

## References

- Robert C. Martin, *Clean Code* (2008) and *Clean Architecture* (2017)
- Memory record: `feedback_clean_code_architecture.md` (project-level feedback)
- [.claude/agents/pr-reviewer.md](../.claude/agents/pr-reviewer.md) §3a — Clean Code / SOLID audit checklist
- [.claude/agents/firmware-architecture-reviewer.md](../.claude/agents/firmware-architecture-reviewer.md) §4 — SOLID at module boundaries
