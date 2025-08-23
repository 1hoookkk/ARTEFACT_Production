# Agent Contract Migration Log

**Migration Date**: 2025-08-20  
**Migration Reason**: Relocate agent contracts to `.claude/agents/` for better Claude integration

## What Moved

### Core Contracts
- `docs/agents/rt-audio-guardian.contract.yaml` → `.claude/agents/rt-audio-guardian.contract.yaml`
- `docs/agents/dsp-frequency-oracle.contract.yaml` → `.claude/agents/dsp-frequency-oracle.contract.yaml`  
- `docs/agents/juce-integration-specialist.contract.yaml` → `.claude/agents/juce-integration-specialist.contract.yaml`
- `docs/agents/build-stability-monitor.contract.yaml` → `.claude/agents/build-stability-monitor.contract.yaml`

### Profile Variants  
- `docs/agents/profiles/` → `.claude/agents/profiles/`
  - All RT-Audio Guardian variants (3 files)
  - All DSP Frequency Oracle variants (2 files) 
  - All JUCE Integration Specialist variants (1 file)
  - All Build Stability Monitor variants (1 file)

### Framework Files
- `docs/agents/BestOfN.yml` → `.claude/agents/BestOfN.yml` (mirrored, original kept)

## What Stayed

### Human-Facing Files (kept in docs/agents/)
- `docs/agents/BestOfN.yml` - Original for human review/editing
- `docs/agents/reports/` - Analysis output directory unchanged
- `docs/agents/proposals/` - Variant proposals backup location

## Loader Resolution Priority

Commands now use dual-location loading:

1. **Primary Search**: `.claude/agents/*.contract.yaml`
2. **Fallback Search**: `docs/agents/*.contract.yaml`  
3. **Profile Search**: `.claude/agents/profiles/` then `docs/agents/profiles/`
4. **Duplicate Handling**: Warning issued, `.claude/agents/` version takes precedence

## Migration Metadata

All migrated files include provenance tracking:
```yaml
# Migration Metadata
source_path: "docs/agents/[original-path]"
migration_date: "2025-08-20"
```

## Backward Compatibility

✅ **Commands remain compatible**: `/agents-run` and `/agents-propose-niche` unchanged  
✅ **Reports unchanged**: Output continues to `docs/agents/reports/`  
✅ **No breaking changes**: Existing workflows continue to work  

## Rollback Instructions

To revert to pre-migration state:

1. **Delete migrated directory**:
   ```bash
   rm -rf .claude/agents/
   ```

2. **Commands auto-fallback**: No code changes needed, commands automatically use `docs/agents/`

3. **Verify rollback**: Run `/agents-run` to confirm loading from `docs/agents/`

## Benefits of Migration

- **Better Claude integration**: Contracts accessible in Claude's preferred directory
- **Reduced file tree clutter**: Technical contracts moved to `.claude/`  
- **Preserved human access**: Reports and documentation remain in `docs/`
- **Dual-location safety**: Fallback prevents total failure if `.claude/` unavailable

## Migration Integrity

- ✅ All 4 core contracts migrated with metadata
- ✅ All 7 profile variants migrated with metadata  
- ✅ BestOfN framework mirrored
- ✅ Commands updated for dual-location loading
- ✅ Original files preserved intact
- ✅ Anti-hallucination guardrails maintained

## Testing Migration

Run these commands to verify migration success:

```bash
# Test contract discovery
/agents-run --dry-run

# Verify dual loading (should show both locations)
ls .claude/agents/*.contract.yaml
ls docs/agents/*.contract.yaml

# Confirm reports still output correctly  
/agents-run --quick
ls docs/agents/reports/
```