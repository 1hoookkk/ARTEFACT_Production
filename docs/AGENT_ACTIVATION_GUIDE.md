# SpectralCanvas Pro Agent Activation Guide

## 🎯 Quick Start - Using Your Agents

Your agents are now fully integrated with BMad orchestration! Here's how to activate and use them:

### Available Specialist Agents

| Command | Agent | Purpose |
|---------|-------|---------|
| `*agent rt-audio-guardian` | RT-Audio Safety Guardian 🔴 | Real-time audio thread safety enforcement |
| `*agent dsp-frequency-oracle` | DSP & Frequency Oracle 🔵 | DSP mathematics and frequency mapping validation |
| `*agent juce-integration-specialist` | JUCE Integration Specialist 🟢 | JUCE framework best practices and lifecycle management |
| `*agent build-stability-monitor` | Build Stability Monitor 🟠 | Compilation prediction and cross-platform stability |

## 🚀 Agent Activation Examples

### 1. RT-Safety Audit (Critical for Audio Development)
```
*agent rt-audio-guardian
*task scan
```
**What it does**: Scans your entire codebase for real-time audio violations (locks, allocations, logging on audio thread)

### 2. Frequency Mapping Validation (Essential for Spectral Accuracy)
```
*agent dsp-frequency-oracle
*task validate-mapping
```
**What it does**: Validates the logarithmic Y→frequency mapping mathematics and STFT processing integrity

### 3. JUCE Integration Health Check
```
*agent juce-integration-specialist
*task audit-lifecycle
```
**What it does**: Analyzes APVTS usage, component lifecycle safety, and GUI↔Audio thread boundaries

### 4. Build System Health Assessment
```
*agent build-stability-monitor
*task predict-build
```
**What it does**: Predicts compilation failures, detects header pollution, validates CMake configuration

## 🔄 Available Workflows

### Comprehensive RT-Safety Audit
```
*workflow rt-safety-audit
```
**Multi-agent workflow**: Complete real-time safety validation across your entire audio processing chain

### Spectral Processing Validation
```
*workflow spectral-validation
```
**Multi-agent workflow**: Mathematical validation, STFT analysis, golden reference testing, and build validation

## 📚 Agent Knowledge Integration

Your agents now have access to:
- **Restored Documentation**: Critical analysis from `_archive/` now in `docs/architecture/`
- **Domain Expertise**: Specialized knowledge in `docs/agents/knowledge/`
- **Project Context**: Your existing agent contracts in `docs/agents/`
- **Cross-References**: Links between architecture, DSP, and implementation details

## 🎯 When to Use Each Agent

### RT-Audio Guardian 🔴
**Use when:**
- Making changes to `processBlock()` or audio processing code
- Adding new synthesis engines or oscillators
- Implementing paint-to-audio pipeline modifications
- Debugging audio dropouts or performance issues

**Triggers automatically on:**
- Changes to `Source/Core/PluginProcessor.cpp`
- Modifications to `SpectralSynthEngine`
- Updates to `AtomicOscillator` or paint processing

### DSP Frequency Oracle 🔵
**Use when:**
- Modifying frequency mapping (`ColorToSpectralMapper`)
- Changing spectral processing algorithms
- Implementing new STFT features
- Validating mathematical correctness of DSP

**Triggers automatically on:**
- Changes to spectral processing files
- Modifications to frequency mapping logic
- Updates to FFT/STFT processing

### JUCE Integration Specialist 🟢
**Use when:**
- Working with APVTS parameter management
- Modifying UI components or editor code
- Implementing new GUI features
- Debugging component lifecycle issues

**Triggers automatically on:**
- Changes to `PluginEditor` or GUI components
- APVTS parameter modifications
- Component lifecycle updates

### Build Stability Monitor 🟠
**Use when:**
- Modifying `CMakeLists.txt` or build configuration
- Adding new source files or headers
- Changing include dependencies
- Working on cross-platform compatibility

**Triggers automatically on:**
- CMake configuration changes
- Header file modifications
- Include dependency updates

## 🔧 Advanced Usage

### Custom Task Execution
```
*agent rt-audio-guardian
*task audit-critical-paths    # Deep analysis of audio processing
*task generate-rt-report      # Comprehensive compliance report
```

### Multi-Agent Collaboration
```
*workflow-guidance           # Get help choosing the right workflow
*plan                       # Create detailed workflow plan
*plan-status               # Check workflow progress
```

### Knowledge Base Access
```
*kb-mode                   # Access full BMad knowledge base
*agent <name>              # Transform to specialist then use their knowledge
*doc-out                   # Generate comprehensive documentation
```

## 📖 Documentation Structure

```
docs/
├── README.md ..................... Quick navigation and overview
├── AGENT_ACTIVATION_GUIDE.md ..... This guide
├── RECOVERY_PLAN.md .............. Documentation recovery status
├── architecture/ ................. Critical technical analysis
│   ├── paint-to-audio-analysis.md .. Restored from archive
│   ├── dsp-algorithm-review.md ..... Restored from archive  
│   └── [other architecture docs]
├── agents/ ....................... AI collaboration system
│   ├── [existing contracts] ........ Your original agent specs
│   ├── knowledge/ ................ Domain-specific knowledge
│   │   └── dsp-prompt-templates.md . Restored prompt templates
│   └── workflows/ ................ Multi-agent procedures
└── operations/ ................... Development procedures
```

## ✅ Success Metrics

### Agent Effectiveness
- **RT-Audio Guardian**: Zero locks/allocations/logging on audio thread
- **DSP Frequency Oracle**: Mathematical accuracy within tolerance
- **JUCE Integration Specialist**: APVTS best practices compliance
- **Build Stability Monitor**: Successful cross-platform builds

### Workflow Integration
- **Response Time**: Agents activate and respond within seconds
- **Knowledge Access**: Full access to project-specific expertise
- **Quality Gates**: Automated enforcement of safety rules
- **Documentation**: Comprehensive reports and recommendations

## 🎯 Next Steps

1. **Try an Agent**: Start with `*agent rt-audio-guardian` and `*task scan`
2. **Explore Workflows**: Use `*workflow rt-safety-audit` for comprehensive analysis
3. **Access Knowledge**: Try `*kb-mode` to explore the full knowledge base
4. **Customize**: Modify agent configurations in `.bmad-core/agents/` as needed

---

**Your agents are ready to help maintain the highest standards of real-time audio safety, DSP accuracy, JUCE integration, and build stability for SpectralCanvas Pro!**