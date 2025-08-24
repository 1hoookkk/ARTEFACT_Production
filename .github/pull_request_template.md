## Goal
What outcome does this PR deliver?

## Scope
Files / subsystems touched; out-of-scope items.

## Runtime Safety
- Audio thread: any changes? Locks/allocs? (If yes, explain RT safety)

## Validation
- Build + run steps
- Snapshots / pluginval

## Risk & Rollback
How to revert safely?

## Reviewer Prompt (Gemini)
Please review for:
1) Violations of CLAUDE.md Core Practices (CrashToggles, procedural UI, RT safety)
2) Asset/texture regressions in CanvasPanel
3) Snapshot harness correctness (message thread I/O only)
4) Build/CI reliability; CMake/MSBuild macro hygiene
