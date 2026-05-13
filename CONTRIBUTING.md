# Contributing to Svitrix

Thanks for your interest in contributing. Svitrix is custom firmware for the [Ulanzi TC001 Smart Pixel Clock](https://www.ulanzi.com/) (and compatible 32×8 LED matrix devices), built around an ESP32 with Arduino / PlatformIO.

This document covers the local-dev setup and the conventions reviewers will check. If you're looking for the *what* and the *why* of the architecture, read [CLAUDE.md](CLAUDE.md) at the repo root and the per-module docs it links to.

## Quick start

```bash
# clone + open
git clone https://github.com/svitrix/svitrix-firmware.git
cd svitrix-firmware

# native unit tests (no hardware needed) — should pass before any PR
pio test -e native_test

# build firmware (auto-builds the web SPA via tools/build_web.py)
pio run -e ulanzi

# optional: full local CI pipeline (tests + build + flash-size check)
# inside a Claude Code session: /build
```

The two things that matter for a clean PR:
1. `pio test -e native_test` passes
2. `pio run -e ulanzi` succeeds and stays under ~90% flash usage

A pre-commit hook ([.pre-commit-config.yaml](.pre-commit-config.yaml)) catches the common issues — install with `pip install pre-commit && pre-commit install`.

## Project layout

See [CLAUDE.md § Project Structure](CLAUDE.md#project-structure). High-level:

| Directory | What's there |
|-----------|--------------|
| `src/` | Stateful modules (DisplayManager, MQTTManager, ServerManager, …) |
| `lib/interfaces/` | Pure-virtual `I*.h` decoupling layer ([ADR 0001](adr/0001-interface-decoupling.md)) |
| `lib/services/` | Stateless utility libraries with 100% test coverage ([ADR 0003](adr/0003-stateless-services.md)) |
| `lib/config/` | Config struct definitions |
| `web/` | Preact SPA served from LittleFS |
| `test/test_native/` | Native unit tests (Unity, runs on host) |
| `adr/` | [Architecture Decision Records](adr/README.md) — read these before proposing structural changes |
| `docs/` | Public VitePress documentation |

## Design rules

These are non-negotiable — PRs that violate them will be sent back for revision.

### Embedded C++ constraints
See `~/Work/CLAUDE.md` (auto-loaded for AI sessions, but applies to humans too): no `malloc` after init, no runtime `std::string`/`std::vector` in hot paths, no exceptions, fixed-width types only, ISR safety. Summary in [CLAUDE.md § Architecture Rules](CLAUDE.md#architecture-rules).

### Architecture
- **Modules talk through interfaces, not direct includes.** See [ADR 0001](adr/0001-interface-decoupling.md).
- **Effects use `IPixelCanvas`, never FastLED/NeoMatrix directly.** Effects must be testable with `MockPixelCanvas` ([ADR 0001](adr/0001-interface-decoupling.md)).
- **Stateless services don't call `millis()` or read globals.** Pass time/state as parameters ([ADR 0003](adr/0003-stateless-services.md)).
- **DisplayManager stays split into 3 classes** — don't collapse them ([ADR 0002](adr/0002-three-class-display-manager.md)).
- **All wiring lives in `src/main.cpp`** — modules don't reach out to other modules at construction time.

### Code style
- **Clean Code & SOLID** ([ADR 0004](adr/0004-clean-code-and-architecture.md)) — meaningful names, small functions, one reason to change per file.
- **`clang-format`** runs in pre-commit and CI. Style file at [.clang-format](.clang-format).
- **`clang-tidy`** advisory check in CI ([.clang-tidy](.clang-tidy)). New issues will be flagged but don't (yet) block merge — that's coming.
- **Comments** — only when WHY isn't obvious. Don't restate WHAT the code does.

## Tests

- Every new function in `lib/services/` needs a Unity test under `test/test_native/test_<name>/`. Coverage stays at 100%.
- Every new effect in `src/effects/` needs a test that uses `MockPixelCanvas`.
- For module-level changes (DisplayManager, MQTTManager, …), tests are encouraged where the surface is testable without a physical device.

Run with `pio test -e native_test`.

## Commits and PRs

Conventional Commits format. Detailed rules in [.claude/git-workflow.md](.claude/git-workflow.md). Short version:

```
<type>(<scope>): <description>

<body — explain WHY, not WHAT>

<optional: Refs: #123>
```

- `type` ∈ `feat`, `fix`, `refactor`, `perf`, `docs`, `test`, `chore`, `build`, `ci`, `style`
- Subject ≤ 72 chars, imperative mood, no trailing period
- **No `Co-Authored-By:` lines** — we don't use them on this project

Branch from `main`, work on `feature/<descriptive-name>`, squash-merge via PR. PR title follows the same Conventional Commits format.

Use the [PR template](.github/PULL_REQUEST_TEMPLATE.md) — it has a checklist.

## Documentation

If your change affects user-visible behavior, update the relevant docs in the same PR:

- **Public docs (`docs/`):** end-user facing, VitePress, English + Ukrainian translations. Use `/sync-public-docs` slash command in Claude Code, or update manually.
- **AI-reference docs (`CLAUDE.md`, `README.md`):** internal architecture docs, used by Claude Code sessions. Use `/sync-docs` slash command, or update manually.
- **ADRs:** propose a new `adr/000N-...md` when introducing a meaningful architectural change (new top-level module, new interface, new persistence layer, new protocol integration). Template and rules in [adr/README.md](adr/README.md).

## Sub-agents and slash commands (Claude Code)

If you use Claude Code, this project ships:
- `/build` — full local CI (tests + firmware + size check)
- `/add-effect`, `/add-ha-entity`, `/add-service` — opinionated recipes
- `/pr`, `/release`, `/changelog` — release workflow
- `pr-reviewer`, `embedded-cpp-reviewer`, `firmware-architecture-reviewer` — read-only review sub-agents

Listed in [CLAUDE.md § Available Skills](CLAUDE.md#available-skills) and [CLAUDE.md § Sub-agents](CLAUDE.md#sub-agents-review).

## Reporting bugs and proposing features

- **Bugs** → [Issue template](.github/ISSUE_TEMPLATE/bug_report.yml)
- **Features** → [Issue template](.github/ISSUE_TEMPLATE/feature_request.yml)
- **Questions / ideas** → [GitHub Discussions](https://github.com/svitrix/svitrix-firmware/discussions)
- **Security** → see [SECURITY.md](SECURITY.md) (please don't use public issues)

## Code of conduct

Participation is governed by [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) — be respectful, assume good faith, focus on the work.

## Questions

Open a [Discussion](https://github.com/svitrix/svitrix-firmware/discussions) — there's no such thing as a question too small for a young project.
