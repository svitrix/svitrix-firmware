---
description: Split a feature into parallel agents working in isolated worktrees
---

Decompose a feature into independent sub-tasks and dispatch them as parallel agents, each in its own git worktree. Merge results when all agents complete.

## Input

The user provides:
- Feature description
- Optional: explicit sub-task split

## Steps

1. **Decompose the feature:**
   Read "Common Change Patterns" in root CLAUDE.md.
   Split into sub-tasks where each task touches **different files**.

   Good splits (no file overlap):
   - Agent 1: firmware C++ (src/effects/, src/Apps/)
   - Agent 2: web UI (web/src/)
   - Agent 3: tests (test/test_native/)

   Bad splits (will cause merge conflicts):
   - Two agents editing the same .cpp file
   - Agent depends on output of another agent

2. **Verify independence:**
   For each pair of agents, confirm their file lists don't overlap.
   If overlap exists — merge those tasks into one agent.

3. **Dispatch agents:**
   Launch each agent with the `Agent` tool using `isolation: "worktree"`:
   - Include the full sub-task description
   - Include the relevant module CLAUDE.md paths to read
   - Include the specific files to create/modify
   - Tell each agent to commit their changes

4. **Wait for completion:**
   All agents run in parallel. Wait for all to finish.

5. **Merge results:**
   After all agents complete:
   ```bash
   # Review each agent's changes
   git diff main..<agent-branch>
   ```
   Merge each agent's branch into the feature branch.
   Resolve any unexpected conflicts.

6. **Verify combined result:**
   Run /build on the merged result:
   ```bash
   pio test -e native_test && pio run -e ulanzi
   ```

7. **Create PR:**
   Follow /pr workflow on the combined feature branch.

## Agent prompt template

Each agent gets a prompt like:

```
You are implementing part of a feature for svitrix-firmware.

Your task: [specific sub-task]
Files to create/modify: [exact list]

Before coding, read these docs:
- CLAUDE.md (root) — architecture rules
- [relevant module CLAUDE.md]

After implementing:
1. Run: pio test -e native_test (if you created tests)
2. Commit with conventional commit format

Do NOT modify files outside your assigned list.
```

## Rules

- Minimum 2 agents, maximum 5
- Each agent MUST have a non-overlapping file list
- Documentation updates go to whichever agent owns that module
- Root CLAUDE.md count updates (effect count, entity count) go to a final cleanup step after merge
- Always run /build after merging all agent results
