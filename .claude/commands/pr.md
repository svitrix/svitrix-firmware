---
description: Create a well-formatted GitHub PR from the current feature branch
---

Create a GitHub Pull Request from the current feature branch to main with a structured description.

## Steps

1. **Gather context:**
   ```bash
   git branch --show-current
   git log --oneline main..HEAD
   git diff --stat main..HEAD
   git diff main..HEAD
   ```
   Verify we're on a `feature/*` branch (not main). If on main — abort with a message.

2. **Ensure branch is pushed:**
   ```bash
   git rev-parse --abbrev-ref --symbolic-full-name @{u} 2>/dev/null
   ```
   If no upstream — push with `git push -u origin <branch>`.

3. **Analyze all commits** (not just the latest!) and changed files:
   - Group changes by type (feat, fix, refactor, etc.)
   - Identify breaking changes
   - Note affected modules (from file paths)
   - Check if tests were added/updated
   - Check binary size impact if firmware files changed

4. **Generate PR title:**
   - If single feat/fix commit: use that commit's subject line
   - If multiple commits of same type: summarize as single conventional commit
   - If mixed types: use the most significant change as title
   - Keep under 70 characters
   - Format: `<type>[(scope)]: <description>`

5. **Generate PR body** using this template:

   ```markdown
   ## Summary

   <2-4 bullet points describing what this PR does and WHY>

   ## Changes

   <grouped by type, one bullet per logical change>

   ### Features
   - description

   ### Bug Fixes
   - description

   ### Other
   - description

   ## Breaking Changes

   <only if applicable, otherwise omit section>

   ## Binary Size

   <if firmware .cpp/.h files changed>
   - Before: X bytes
   - After: Y bytes
   - Delta: +/- Z bytes (X.X%)

   ## Test Plan

   - [ ] `pio test -e native_test` passes
   - [ ] `pio run -e ulanzi` builds successfully
   - [ ] Binary size within partition limit
   - [ ] <specific test items based on changes>
   ```

6. **Create the PR:**
   ```bash
   gh pr create --title "<title>" --body "<body>" --base main
   ```
   Use a HEREDOC for the body to preserve formatting.

7. **Report:**
   Print the PR URL so the user can review it.

## Rules

- Omit empty sections (no "### Bug Fixes" if there are none)
- Binary Size section only if .cpp/.h/.ino files were changed
- Breaking Changes section only if any commit has `!` or `BREAKING CHANGE:`
- Test Plan checkboxes should be specific to the actual changes
- If the branch has only doc/config changes, simplify the template accordingly
- Do NOT auto-merge — just create the PR

## Done when

- [ ] Branch pushed to origin
- [ ] PR created with title in conventional commit format
- [ ] PR body has Summary, Changes, and Test Plan sections
- [ ] PR URL printed to user
