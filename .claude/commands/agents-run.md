# /agents-run

Run all configured analysis agents and execute their posthook tests.

## Purpose
Execute the autonomous subagent suite for SpectralCanvas Pro, collecting analysis results and running validation tests. Agents operate in READ-ONLY mode with ctest posthooks only.

## Agent Execution Flow
1. **Load Contracts**: Dual-location loading with priority:
   - Primary: `.claude/agents/*.contract.yaml`
   - Fallback: `docs/agents/*.contract.yaml` 
   - Warning: Log if duplicates found (prefer .claude/agents)
2. **Parallel Analysis**: Run agents against current repository state
3. **Collect Results**: Gather analysis outputs in deterministic format
4. **Execute Posthooks**: Run only `Task(ctest ...)` commands from contracts
5. **Generate Report**: Consolidate findings and test results to `docs/agents/reports/`

## Usage
```bash
# Run all agents with default configuration
/agents-run

# Run specific agent category
/agents-run --category rt-safety

# Run with performance profiling
/agents-run --profile

# Generate detailed report
/agents-run --verbose
```

## Agent Categories
- **rt-audio-guardian**: Real-time safety violations
- **dsp-frequency-oracle**: Frequency mapping and DSP validation  
- **juce-integration-specialist**: JUCE patterns and lifecycle
- **build-stability-monitor**: Compilation and dependency issues

## Safety Guarantees
- **READ-ONLY**: Agents use only Read and Grep tools
- **No File Modifications**: Zero write operations outside allowed paths
- **Controlled Testing**: Posthooks limited to `Task(ctest ...)` only
- **Deterministic Output**: Schema-compliant, repeatable results

## Contract Loading Priority
Agents loaded in this search order:
1. `.claude/agents/*.contract.yaml` (highest priority)
2. `docs/agents/*.contract.yaml` (fallback)  
3. Profiles: `.claude/agents/profiles/` then `docs/agents/profiles/`
4. Duplicates: Warning issued, `.claude/agents/` version used

## Output Structure
```
docs/agents/reports/<timestamp>/
├── rt-audio-guardian.json
├── dsp-frequency-oracle.json  
├── juce-integration-specialist.md
├── build-stability-monitor.json
├── contract-sources.log  # Shows which contracts were loaded from where
├── test-results/
│   ├── RT-safety-tests.log
│   ├── spectral-core-tests.log
│   └── integration-tests.log
└── consolidated-report.md
```

## Anti-Hallucination Measures
- Agents emit `{status:"uncertain", reason:"..."}` when evidence insufficient
- All findings must reference specific files/lines/patterns
- Confidence levels included in numerical assessments
- No assumptions or guesses permitted

## Example Output
```json
{
  "agent": "rt-audio-guardian",
  "timestamp": "2025-08-20T15:30:00Z",
  "violations": [
    {
      "file": "Source/Core/PluginProcessor.cpp",
      "line": 127,
      "rule": "blocking_operations",
      "excerpt": "std::lock_guard<std::mutex> lock(mutex_);",
      "severity": "critical",
      "confidence": 0.95
    }
  ],
  "summary": {
    "total_violations": 1,
    "critical_count": 1,
    "files_scanned": 45,
    "scan_duration_ms": 2847
  }
}
```

## Integration with TDD-Guard
Agents can be triggered by TDD-Guard for continuous validation:
```yaml
# tdd-guard.yaml integration
posthooks:
  - name: "RT-Safety Analysis"  
    command: "/agents-run --category rt-safety --quick"
    triggers: ["Source/Core/**", "Source/Spectral/**"]
```