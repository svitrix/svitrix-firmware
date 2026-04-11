---
description: Interactive release workflow — beta, rc, or stable
---

Guide through the Svitrix firmware release process following the project's semver conventions.

## Steps

1. **Check prerequisites:**
   - `git status` must be clean (no uncommitted changes)
   - All tests pass: `pio test -e native_test`
   - Build succeeds: `pio run -e ulanzi`
   - If any check fails — STOP and report.

2. **Determine release type:**
   Ask the user: **beta**, **rc**, or **stable**?

3. **Calculate next version:**
   - List existing tags: `git tag --sort=-v:refname | head -20`
   - Based on the latest tag and release type, calculate next version:
     - If latest is `v0.2.0` and type is beta → `v0.3.0-beta.1`
     - If latest is `v0.3.0-beta.1` and type is beta → `v0.3.0-beta.2`
     - If latest is `v0.3.0-beta.2` and type is rc → `v0.3.0-rc.1`
     - If latest is `v0.3.0-rc.1` and type is rc → `v0.3.0-rc.2`
     - If latest is `v0.3.0-rc.2` and type is stable → `v0.3.0`
   - Show calculated version and ask user to confirm or override.

4. **Generate release notes:**
   - `git log --oneline <last-stable-tag>..HEAD`
   - Group commits by type (feat, fix, refactor, docs, etc.)
   - Present formatted release notes for review.

5. **Create annotated tag:**
   ```bash
   git tag -a vX.Y.Z -m "vX.Y.Z: <short description>"
   ```
   IMPORTANT: Use annotated tags only (`-a` flag).

6. **Show next steps** (do NOT execute automatically):
   ```
   Push tag:     git push origin vX.Y.Z
   Push branch:  git push origin <branch>
   ```
   The CI pipeline (`main.yml`) will automatically:
   - Build firmware with version embedded
   - Create GitHub Release (pre-release for beta/rc)
   - Attach firmware.bin

7. **Remind:** beta and rc tags should be created on feature branches. Stable tags go on `main` after PR merge.
