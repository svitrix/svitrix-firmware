---
description: Generate changelog from conventional commits — for a release, range, or since last tag
---

Generate a structured changelog from git commit history using Conventional Commits format.

## Input

The user may specify:
- A version tag: `/changelog v0.2.0` → commits since previous tag to v0.2.0
- A range: `/changelog v0.1.0..HEAD` → commits in that range
- Nothing: `/changelog` → commits since the last tag to HEAD

## Steps

1. **Determine commit range:**
   ```bash
   # Find the last tag
   git describe --tags --abbrev=0 HEAD 2>/dev/null
   # Find the previous tag (for a specific release)
   git describe --tags --abbrev=0 <tag>^ 2>/dev/null
   ```
   If no tags exist, use all commits on the current branch.

2. **Collect commits:**
   ```bash
   git log <range> --oneline --no-merges
   ```

3. **Parse each commit** according to Conventional Commits:
   - Extract: type, scope (optional), breaking change flag, description
   - Skip merge commits
   - Group by type into changelog sections

4. **Generate changelog** in this format:

   ```markdown
   ## [v0.2.0] — YYYY-MM-DD

   ### Breaking Changes
   - **scope:** description (#PR)

   ### Features
   - **scope:** description (#PR)

   ### Bug Fixes
   - **scope:** description (#PR)

   ### Performance
   - **scope:** description (#PR)

   ### Other Changes
   - **type(scope):** description
   ```

   **Section mapping:**
   | Commit type | Changelog section |
   |-------------|-------------------|
   | `feat` | Features |
   | `fix` | Bug Fixes |
   | `perf` | Performance |
   | Breaking (`!` or `BREAKING CHANGE:`) | Breaking Changes (always first) |
   | `refactor`, `docs`, `test`, `chore`, `build`, `ci`, `style` | Other Changes |

   **Rules:**
   - Omit empty sections
   - If scope is present, bold it: `- **mqtt:** add light entity support`
   - If no scope: `- add temperature overlay`
   - Link PR/issue numbers if present in commit message
   - Sort entries alphabetically by scope within each section

5. **Output options:**
   - Print to terminal (default)
   - If user says "save" or "write": append/prepend to `CHANGELOG.md` in project root
   - If `CHANGELOG.md` doesn't exist, create it with a header:
     ```markdown
     # Changelog

     All notable changes to this project will be documented in this file.
     Format based on [Keep a Changelog](https://keepachangelog.com/) and
     [Conventional Commits](https://www.conventionalcommits.org/).
     ```

6. **Report:**
   ```
   Changelog for v0.1.0..HEAD (12 commits):
   - 3 features, 2 fixes, 1 perf, 6 other
   - 0 breaking changes
   ```

## Example output

```markdown
## [v0.2.0] — 2026-04-15

### Features
- **datafetcher:** add JSON path array index support
- **mqtt:** add light entity support
- **web:** migrate web UI to Preact SPA

### Bug Fixes
- **display:** prevent crash on empty notification text
- **mqtt:** free HA entity memory before re-allocation

### Performance
- **display:** reduce DisplayRenderer draw calls

### Other Changes
- chore: upgrade C++ standard from C++11 to C++17
- docs: sync documentation with feature branch changes
- test: add UnicodeFont edge cases
```
