<!--
Thanks for contributing to Svitrix! Please fill in the sections below.
Keep it concise — reviewers care about WHY more than WHAT.
Delete sections that don't apply.
-->

## Summary

<!-- 1–3 bullets: what changes and why. Link the issue if there is one. -->

-
-

## Changes

<!-- High-level list of touched modules / behavior. Group by area when useful. -->

-
-

## Test plan

<!-- How did you verify this works? Include exact commands and what you saw. -->

- [ ] `pio test -e native_test` passes
- [ ] `pio run -e ulanzi` builds successfully
- [ ] Manual verification on hardware (if behavior is observable on device)

<!-- Add module-specific checks below, e.g.:
- [ ] Effect rendered correctly in `tools/render_effect/` GIF output
- [ ] HA entity appears in MQTT discovery and reacts to commands
- [ ] Settings page in SPA round-trips values to NVS
-->

## Binary size

<!-- Run `/flash-size` (Claude Code) or `pio run -e ulanzi -v` and paste app/spiffs deltas.
Significant size changes (>1%) should be explained. Delete this section for docs-only PRs. -->

| Section | Before | After | Delta |
|---------|--------|-------|-------|
| app     |        |       |       |

## Breaking changes

<!-- Anything that breaks existing configs, MQTT topics, REST endpoints, NVS layout,
or HA entity IDs? If so, describe the migration path. Otherwise: "None." -->

None.

## Documentation

<!-- Which docs were updated alongside the code? -->

- [ ] Module-level `CLAUDE.md` updated (if architecture changed)
- [ ] Root `CLAUDE.md` Cross-module Impact Map / wiring tables updated (if interfaces changed)
- [ ] Public docs in `docs/` updated (if user-visible behavior changed) — use `/sync-public-docs`
- [ ] `CHANGELOG.md` `[Unreleased]` section updated
- [ ] ADR added under `adr/` (if architectural decision)

## Checklist

- [ ] Branch name follows `feat/…`, `fix/…`, `refactor/…`, `docs/…` convention
- [ ] Commits follow [Conventional Commits](https://www.conventionalcommits.org/)
- [ ] No `Co-Authored-By:` lines (per [git-workflow](.claude/git-workflow.md))
- [ ] No secrets, API keys, or personal Wi-Fi/MQTT credentials in the diff
- [ ] Respects embedded constraints (no runtime heap, no blocking in ISR, fixed-width types)
