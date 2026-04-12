---
description: Delegate a full feature implementation — branch, code, tests, build, PR
---

End-to-end feature implementation from a single description. Creates a branch, implements the feature, verifies with build + tests, and creates a PR.

## Input

The user provides:
- Feature description (what to build)
- Optional: reference file/lines for algorithm or visual spec
- Optional: architecture hint (new effect, new TMODE, new config, etc.)

## Steps

1. **Identify change pattern:**
   Read the "Common Change Patterns" section in root CLAUDE.md.
   Match the feature description to a pattern to get the file list.
   If no pattern matches — read relevant module CLAUDE.md files to determine affected files.

2. **Create feature branch:**
   ```bash
   git checkout main
   git pull origin main
   git checkout -b feature/<feature-name>
   ```

3. **Read reference material:**
   - If the user provided a reference file — read it first
   - Read CLAUDE.md files for all affected modules
   - Read existing code that will be modified

4. **Implement the feature:**
   - Follow the architecture rules from root CLAUDE.md
   - Use IPixelCanvas for effects (never direct FastLED access)
   - Use existing patterns from adjacent code as templates
   - Write tests alongside implementation

5. **Verify with /build:**
   Run the full build pipeline:
   ```bash
   pio test -e native_test && pio run -e ulanzi
   ```
   If tests or build fail — fix before proceeding.
   Check flash size thresholds.

6. **Update documentation:**
   Check Cross-module Impact Map in root CLAUDE.md.
   Update all affected docs (CLAUDE.md, README.md, counts).

7. **Commit changes:**
   Use conventional commit format from root CLAUDE.md.
   Group logically: implementation + tests in one commit, docs in another if substantial.

8. **Create PR:**
   Follow the /pr command workflow:
   - Push branch
   - Generate title + body
   - Create PR via `gh pr create`

## Rules

- ALWAYS read "Common Change Patterns" before writing code
- ALWAYS check "Cross-module Impact Map" before committing
- ALWAYS run build + tests before creating PR
- If flash size > 95% — warn user and suggest optimizations
- If reference file provided — adapt the algorithm, don't copy JS/Python to C++ literally
- Commit messages follow conventional commits (see root CLAUDE.md)
