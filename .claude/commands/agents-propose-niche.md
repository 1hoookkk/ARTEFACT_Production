# /agents-propose-niche

Generate and score niche variant proposals for existing core agents.

## Purpose  
Synthesize specialized agent variants based on Best-of-N analysis, project evolution, and emerging patterns. Creates focused variants for specific contexts while maintaining the core agent hierarchy.

## Variant Generation Process
1. **Analyze Core Performance**: Review core agent effectiveness on recent changes
2. **Identify Specialization Opportunities**: Find patterns requiring focused analysis  
3. **Generate Variant Proposals**: Create 2-3 specialized profiles per core agent
4. **Score via BestOfN Framework**: Apply scoring rubric to validate proposals
5. **Output Profile Diffs**: Generate variant configurations for review

## Usage
```bash
# Propose variants for all core agents
/agents-propose-niche

# Focus on specific agent
/agents-propose-niche --agent rt-audio-guardian

# Generate based on recent repository changes
/agents-propose-niche --window last_20_commits

# Include performance analysis
/agents-propose-niche --with-scoring
```

## Core Agent Targets
- **rt-audio-guardian** → Lock-free patterns, atomic ordering, denormal handling
- **dsp-frequency-oracle** → Mapping validation, phase continuity, aliasing detection  
- **juce-integration-specialist** → Parameter management, lifecycle validation, thread boundaries
- **build-stability-monitor** → Platform-specific issues, dependency validation, test infrastructure

## Variant Discovery Methods
### Pattern-Based Analysis
- **Frequency Analysis**: Most common violation types → specialized detectors
- **Complexity Scoring**: Complex scenarios → focused validators  
- **Performance Hotspots**: Slow analysis areas → optimized variants

### Repository Evolution
- **Recent Changes**: New patterns in commits → emerging validators
- **Error Patterns**: Build/test failures → preventive specialists
- **Performance Issues**: Identified bottlenecks → targeted analyzers

### Best-of-N Optimization
- **Gap Analysis**: Areas where core agents struggle → specialized coverage
- **Performance Tuning**: Speed vs accuracy tradeoffs → optimized variants
- **Context Specialization**: Specific scenarios → context-aware variants

## Variant Proposal Format
```yaml
# Example: SPSC Queue Validator variant
name: spsc-queue-validator
parent: rt-audio-guardian
specialization: "Lock-free SPSC queue correctness validation"
justification: |
  Core rt-audio-guardian detected 85% of lock-free violations but missed
  subtle SPSC pattern violations. Specialized variant needed for paint-to-audio
  queue correctness with higher precision on memory ordering.
  
performance_improvement:
  precision_gain: "+0.12 (0.85 → 0.97)"
  speed_impact: "+15% analysis time"
  coverage_expansion: "SPSC-specific patterns, memory ordering details"
  
deployment_trigger: "Changes to PaintQueue.h or SpscRing usage"
```

## Scoring Integration
Proposals automatically scored using BestOfN framework:
- **Quality**: Projected improvement in precision/recall
- **Safety**: Compliance with read-only + posthook restrictions  
- **Speed**: Estimated performance impact
- **Utility**: Frequency of applicable scenarios

## Contract Discovery
Agents discovered from dual locations:
1. **Primary**: `.claude/agents/*.contract.yaml` and `.claude/agents/profiles/`
2. **Fallback**: `docs/agents/*.contract.yaml` and `docs/agents/profiles/`
3. **Output**: Proposals saved to `.claude/agents/profiles/` (primary)
4. **Backup**: Legacy proposals remain in `docs/agents/proposals/`

## Output Structure
```
.claude/agents/profiles/<agent>/
├── <variant>.proposal.yaml  # New proposals (primary location)
└── <variant>.profile.yaml   # Activated profiles

docs/agents/proposals/<timestamp>/  # Legacy/backup location
├── rt-audio-guardian/
│   ├── spsc-queue-validator.proposal.yaml
│   ├── atomic-ordering-checker.proposal.yaml  
│   └── denormal-detector.proposal.yaml
├── dsp-frequency-oracle/
│   ├── logarithmic-mapping-validator.proposal.yaml
│   └── stft-phase-continuity.proposal.yaml
├── scoring-report.md
└── deployment-recommendations.md
```

## Deployment Workflow
1. **Review Proposals**: Human review of generated variants
2. **Select Champions**: Choose best variants per core agent  
3. **Create Profiles**: Convert proposals to `profiles/*.profile.yaml`
4. **Update Contracts**: Modify core contracts to reference variants
5. **Test Integration**: Validate variants with BestOfN scoring

## Anti-Hallucination Safeguards
- **Evidence-Based**: All proposals grounded in repository analysis
- **Performance Data**: Concrete metrics from core agent runs
- **Uncertainty Handling**: Mark speculative improvements as uncertain
- **Validation Required**: All proposals require human approval

## Integration Patterns
### Continuous Evolution
```yaml
# Auto-trigger after significant repository changes
triggers:
  commit_threshold: 50
  new_file_patterns: ["**/*.h", "**/*.cpp"]
  build_failures: "> 3 in window"
```

### Performance-Based Activation  
```yaml
# Deploy variants when core agents show degradation
activation_criteria:
  precision_drop: "> 0.1"
  recall_drop: "> 0.05"
  speed_degradation: "> 50%"
```

## Example Proposal Generation
```bash
# Generate SPSC queue validator based on recent PaintQueue changes
/agents-propose-niche --focus PaintQueue --parent rt-audio-guardian

# Output: spsc-queue-validator.proposal.yaml with:
# - Specialized SPSC pattern detection
# - Memory ordering validation  
# - Paint gesture queue analysis
# - BestOfN scoring projection
```