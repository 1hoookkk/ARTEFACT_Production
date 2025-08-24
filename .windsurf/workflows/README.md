---
description: README
auto_execution_mode: 3
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

## Rollback Instructions
Delete this entire `.claude/agents/` directory to revert to `docs/agents/` loading.
No code changes needed - commands auto-fallback.