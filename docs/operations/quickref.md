# Quick Reference — Bedrock Flow

## TL;DR Checksum (start every slice)
TASK • FILES (locked) • ENTITIES • DESIRED STATE • CONSTRAINTS • DoD (one sentence)

## Step‑0 Recon (≥6 file:line citations)
ParamIDs • Processor methods • Test names • CMake target & `add_test` • JUCE module includes • Bus config assumptions

## SME Brief — Plugin Designer GPT (SPEC‑ONLY)
- You supply FILES (locked) + ENTITIES + numeric acceptance.
- SME returns: Intent; API delta; Parameter plan (IDs/ranges/units/defaults); Patch plan (≤8 steps tied to files/functions); Acceptance (named test + numeric thresholds); RT safety checklist; Risks & rollback.
- SME may include "How to apply" (steps) — **no code**. Maker turns it into a diff.

## Advisor Audit
GO/NO‑GO • Mismatch risks • Files Allowed • ≤10‑step checklist • Perspective Pass • SESSION_SNAPSHOT

## Maker (PATCH‑ONLY)
Unified diff only • exact build/ctest commands • raw output (≤200 lines) • if blocked → return reason, no improvising.

## Definition of Done
- `ctest` runs, 0 “Not Run”
- Exit code 0 (unless intended RED)
- No RT hazards
- Output matches TL;DR

## Stop‑the‑Line (halt + return to Advisor)
- `***Not Run***` in ctest
- Duplicate `main()`
- Non‑atomic APVTS in `processBlock`
- New locks/allocs in `processBlock`
- Silent audio where test expects signal

## RT Safety Mini‑Checklist
- No locks/allocs/logging in audio thread
- Pre‑allocate in `prepareToPlay`; reuse in `processBlock`
- Read params via `getRawParameterValue()` (atomics)
- No `buffer.clear()` unless test says so
- Block‑based processing unless a RED requires sample accuracy