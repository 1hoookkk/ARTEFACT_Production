---
name: agents-readme
description: Notes and index for local agent contracts
type: docs
---

# Agent Contracts Directory

Local agent contracts for SpectralCanvas Pro. These take precedence over `docs/agents/`.

## Structure
- `*.contract.yaml` - Core agent definitions  
- `profiles/` - Specialized variants
- `BestOfN.yml` - Scoring framework (mirrors docs/agents/)

## Migration Note
Migrated from `docs/agents/` on 2025-08-20 for better Claude integration.
Original contracts preserved with `source_path` metadata.

## Loading Priority
Commands load contracts in this order:
1. `.claude/agents/` (highest priority)
2. `docs/agents/` (fallback)
3. Warning issued if duplicates found

## Routing & Flow

The agent ecosystem follows this execution flow:

**Reader → Maker → ci-runner → gatekeeper → (optional stylebot) → rt-sentinel → Merge**

### Niche Subagents

- **gatekeeper** - `["pre-edit", "pre-build", "pre-test", "guard", "gate"]` - Policy-only security enforcement, never edits files
- **ci-runner** - `["ci", "build", "test", "ctest", "runner"]` - Execute BUILD/TEST commands verbatim
- **secscan** - `["dependency", "security", "scan", "supply-chain"]` - Lightweight dependency & static security checks
- **stylebot** - `["format", "lint", "style", "tidy"]` - Non-semantic formatting and linting fixes
- **rt-sentinel** - `["rt", "processBlock", "audio", "dsp", "realtime"]` - RT-safety validation for audio code

### Flow Notes
- gatekeeper is policy-only and never modifies files
- Each agent has unique trigger phrases for clean routing
- Agents operate in sequence with clear handoffs
- RT-safety is enforced at multiple checkpoints

## Rollback Instructions
Delete this entire `.claude/agents/` directory to revert to `docs/agents/` loading.
No code changes needed - commands auto-fallback.