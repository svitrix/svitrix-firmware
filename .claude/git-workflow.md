# Git & Release Workflow

## Commits — [Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0/)

- Do NOT add `Co-Authored-By` lines to commit messages.

### Format

```
<type>[(scope)][!]: <description>

[body]

[footer(s)]
```

- **Subject:** under 72 chars, lowercase type, imperative mood, no trailing period
- **Body** (optional, blank line after subject): explain **why**, not **what**; wrap at 72 chars
- **Footer** (optional): `BREAKING CHANGE: <desc>` or `Refs: #123`

### Types

| Type | When | Changelog |
|------|------|-----------|
| `feat` | New user-facing feature | **Features** |
| `fix` | Bug fix | **Bug Fixes** |
| `refactor` | Restructure (no behavior change) | — |
| `perf` | Performance improvement | **Performance** |
| `docs` | Documentation only | — |
| `test` | Adding/fixing tests | — |
| `chore` | Maintenance, deps, tooling | — |
| `build` | Build system, platformio.ini | — |
| `ci` | CI/CD pipeline | — |
| `style` | Formatting only | — |

### Scope

Lowercase module name. Examples: `feat(mqtt): add light entity`, `fix(display): correct gamma`.

Common scopes: `mqtt`, `display`, `effects`, `web`, `server`, `menu`, `periphery`, `power`, `datafetcher`, `apps`, `config`, `font`.

### Breaking changes

Either `!` after type/scope (`refactor(mqtt)!: rename INotifier methods`) or `BREAKING CHANGE:` footer. For detailed rename/migration docs, use the footer.

### Example

```
feat(datafetcher): add JSON path array index support

Allow data sources to use array indices in jsonPath (e.g., "data.0.price").
Enables fetching from APIs that return arrays instead of objects.

Refs: #25
```

## Branching

- `main` — always stable, every commit is a tagged release
- `feature/*` — short-lived, merged via PR
- **Squash merge only** (`gh pr merge --squash`)
- No `develop` branch (solo/small team)

## Versioning — SemVer with pre-release tags

Format: `vMAJOR.MINOR.PATCH[-pre.N]`

```
v0.2.0-beta.1   first beta
v0.2.0-beta.2   beta bug fixes
v0.2.0-rc.1     release candidate (feature freeze)
v0.2.0          stable release
```

- **MAJOR** — breaking API/MQTT/config changes
- **MINOR** — new features, new HA entities, new effects
- **PATCH** — bug fixes, perf

## Release process

1. Branch `feature/*` from `main`
2. Develop + test (`pio run -e ulanzi && pio test -e native_test`)
3. Beta: tag `v0.X.0-beta.1` on branch
4. Feature-complete: tag `v0.X.0-rc.1` (bug fixes only after this)
5. Merge PR, tag `v0.X.0` on `main`
6. GitHub Release from tag, attach `firmware.bin`

## GitHub Releases

- **Stable** — full release, marked "Latest"
- **Beta / RC** — mark as pre-release
- Attach `.pio/build/ulanzi/firmware.bin`
- Notes grouped by type (Features, Fixes, Breaking)

## Tags

- Annotated only: `git tag -a v0.2.0 -m "v0.2.0: short description"`
- Push explicitly: `git push origin v0.2.0`
- Never delete or move published tags
