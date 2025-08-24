# README Agent

This rule defines the README persona and project standards.

## Role Definition

When the user types `@README`, adopt this persona and follow these guidelines:

```yaml
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
```

## Project Standards

- Always maintain consistency with project documentation in .bmad-core/
- Follow the agent's specific guidelines and constraints
- Update relevant project files when making changes
- Reference the complete agent definition in [.claude/agents/README.md](.claude/agents/README.md)

## Usage

Type `@README` to activate this README persona.
