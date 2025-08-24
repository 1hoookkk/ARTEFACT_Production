# Agent Activation System

## Purpose
Bridge existing agent contracts in `docs/agents/` with BMad orchestration system.

## Agent Mapping

### RT-Audio Guardian
- **Contract**: `docs/agents/rt-audio-guardian.contract.yaml`
- **BMad Agent**: `.bmad-core/agents/rt-audio-guardian.yaml` 
- **Activation Command**: `*agent rt-audio-guardian`
- **Primary Tasks**: `scan`, `audit`, `validate`, `report`

### DSP Frequency Oracle  
- **Contract**: `docs/agents/dsp-frequency-oracle.contract.yaml`
- **BMad Agent**: `.bmad-core/agents/dsp-frequency-oracle.yaml`
- **Activation Command**: `*agent dsp-frequency-oracle`
- **Primary Tasks**: `validate-mapping`, `analyze-stft`, `check-phase`, `golden-test`

### JUCE Integration Specialist
- **Contract**: `docs/agents/juce-integration-specialist.contract.yaml`
- **BMad Agent**: `.bmad-core/agents/juce-integration-specialist.yaml`
- **Activation Command**: `*agent juce-integration-specialist`
- **Primary Tasks**: `validate-apvts`, `audit-lifecycle`, `check-threads`

### Build Stability Monitor
- **Contract**: `docs/agents/build-stability-monitor.contract.yaml`
- **BMad Agent**: `.bmad-core/agents/build-stability-monitor.yaml`
- **Activation Command**: `*agent build-stability-monitor`
- **Primary Tasks**: `predict-build`, `scan-headers`, `validate-cmake`

## Activation Protocol

### 1. Agent Transformation
When user types `*agent <name>`:
1. Load BMad agent configuration
2. Adopt specialized persona and capabilities
3. Access agent-specific tasks and templates
4. Apply domain-specific rules and constraints

### 2. Task Execution
Each agent supports specific commands:
```
*task <task-name>  # Execute specific agent task
*checklist <name>  # Run agent checklist
```

### 3. Knowledge Integration
Agents have access to:
- Project-specific knowledge in `docs/agents/`
- Agent contracts with detailed specifications
- BMad knowledge base and workflows
- Cross-agent collaboration patterns

## Usage Examples

### RT-Safety Scan
```
*agent rt-audio-guardian
*task scan
```

### Frequency Mapping Validation
```
*agent dsp-frequency-oracle  
*task validate-mapping
```

### JUCE Integration Audit
```
*agent juce-integration-specialist
*task audit-lifecycle
```

### Build Health Check
```
*agent build-stability-monitor
*task predict-build
```

## Integration Benefits
- **Existing Contracts**: Preserves detailed agent specifications
- **BMad Orchestration**: Enables workflow coordination
- **Specialized Knowledge**: Maintains domain expertise
- **Quality Gates**: Enforces project-specific safety rules