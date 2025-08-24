# SpectralCanvas Pro Specialist Agent - Integration Guide

## Overview

The **SpectralCanvas Pro Specialist Agent** is a comprehensive development agent specifically designed for SpectralCanvas Pro - a JUCE-based audio plugin for spectral synthesis and canvas-based sound design. This agent understands the unique architecture, RT-safety constraints, and MetaSynth-inspired vision of the project.

## Agent Capabilities

### Core Expertise Areas

- **RT-Safe Audio Processing**: Lock-free SPSC queues, atomic parameters, zero-allocation synthesis
- **Canvas-Based Spectral Synthesis**: Paint→audio pipeline, MetaSynth workflows, real-time parameter mapping
- **EMU/Tube Analog Character**: Always-on character philosophy, vintage workflow preservation  
- **Brownfield Recovery**: 173+ archived component restoration into 49-file stable foundation
- **JUCE Plugin Architecture**: APVTS management, AudioProcessor lifecycle, VST3/Standalone builds

### Specialized Knowledge Domains

1. **Paint→Audio Pipeline**
   - Lock-free SPSC queue implementation (`PaintQueue<PaintEvent, 4096>`)
   - Real-time paint gesture capture and synthesis parameter control
   - Canvas Y-axis logarithmic frequency mapping (20Hz-20kHz)

2. **RT-Safety Enforcement** 
   - Zero locks/allocations/logging on audio thread
   - Atomic parameter snapshots with `memory_order_relaxed`
   - ProcessBlock() chain validation and violation detection

3. **Component Architecture**
   - SpectralSynthEngine: Master synthesis coordinator
   - EMUFilter: Ladder filter with tube saturation 
   - CEM3389Filter: Secret Audity-style processing
   - AtomicOscillator: RT-safe oscillator implementation

## When to Use This Agent

### Primary Use Cases

- **Canvas Synthesis Features**: Any work involving paint→audio parameter mapping
- **RT-Safety Issues**: Audio thread modifications requiring violation analysis
- **Component Restoration**: Integrating archived components into active codebase
- **EMU Character Work**: Analog modeling and vintage workflow preservation
- **Build Configuration**: SpectralCanvas-specific JUCE integration issues

### Activation Triggers

Use this agent when working on:
- Files in `Source/Core/` (audio processing engine)
- Files in `Source/Spectral/` (STFT and spectral analysis) 
- Component restoration from `/_archive/` directory
- RT-safety violations in `processBlock()` chain
- Canvas-based synthesis feature development
- MetaSynth workflow implementation

## Integration with Other Agents

### Coordination Protocol

The SpectralCanvas Pro Specialist coordinates with other specialized agents:

#### RT-Audio Guardian
- **Purpose**: RT-safety violation detection and analysis
- **Coordination**: Request comprehensive scans before audio thread modifications
- **Handoff**: Share SpectralCanvas-specific RT patterns and constraints

#### Build Stability Monitor  
- **Purpose**: Windows build configuration and macro hygiene
- **Coordination**: Validate CMakeLists.txt changes and dependency management
- **Handoff**: JUCE-specific build patterns and component integration

#### DSP Frequency Oracle
- **Purpose**: Logarithmic frequency mapping and STFT analysis validation
- **Coordination**: Canvas Y-axis frequency response verification
- **Handoff**: Spectral synthesis algorithm correctness review

### Agent Delegation Strategy

```
User Request → SpectralCanvas Pro Specialist Assessment
    ↓
├── RT-Safety Issues → Coordinate with rt-audio-guardian
├── Build Problems → Coordinate with build-stability-monitor  
├── DSP Algorithm → Consult dsp-frequency-oracle
└── Complex Feature → Use TodoWrite + multi-agent coordination
```

## Usage Patterns and Workflows

### Feature Development Workflow

1. **Analysis Phase**
   ```
   - Use Glob to explore existing canvas components
   - Use Read to analyze PaintQueue and SpectralSynthEngine interfaces  
   - Use Grep to find paint→audio integration points
   ```

2. **Planning Phase**
   ```
   - Use TodoWrite for multi-step feature planning
   - Design RT-safe parameter mapping strategy
   - Plan SPSC queue payload structure
   ```

3. **Implementation Phase**
   ```
   - Use Edit/MultiEdit for RT-safe code modifications
   - Implement UI thread paint capture logic
   - Create audio thread parameter consumption
   ```

4. **Validation Phase**
   ```
   - Test with: ctest --test-dir build -C RelWithDebInfo
   - Coordinate with rt-audio-guardian for violation analysis
   - Validate canvas responsiveness under 10ms
   ```

### Component Restoration Workflow

1. **Discovery**
   - Locate archived components: `Glob "**/_archive/**/*.{cpp,h}"`
   - Analyze functionality: `Read` archived component files
   - Find usage patterns: `Grep` historical integration points

2. **Integration Planning**
   - Map dependencies and interface requirements
   - Identify RT-safety issues in archived code  
   - Use `TodoWrite` for step-by-step restoration plan

3. **Safe Integration**  
   - Use `Edit` for careful integration preserving interfaces
   - Update CMakeLists.txt and build configuration
   - Add comprehensive test coverage

4. **Validation**
   - Ensure no regression with existing functionality
   - Validate RT-safety with specialized test suite
   - Test VST3 and Standalone build compatibility

## RT-Safety Patterns and Constraints

### Critical RT-Safety Rules

The agent enforces strict RT-safety constraints for SpectralCanvas Pro:

#### Audio Thread Forbidden Patterns
```cpp
// NEVER on audio thread:
std::lock_guard<std::mutex> lock(mutex_);  // Blocking
auto ptr = new float[1024];               // Allocation  
printf("Debug: %f\n", value);            // Logging
std::string name = "oscillator";         // String construction
```

#### Audio Thread Safe Patterns  
```cpp
// SAFE on audio thread:
auto freq = frequency_.load(std::memory_order_relaxed);  // Atomic load
bool success = paintQueue_.pop(event);                  // Lock-free pop
alignas(64) float buffer[512];                         // Stack allocation
const auto snapshot = paramSnapshot_;                   // Pre-taken snapshot
```

### Paint→Audio Pipeline Safety

```cpp
// UI Thread (Producer)
PaintEvent event{nx, ny, pressure, flags, color};
paintQueue_.push(event);  // Lock-free push, may fail if full

// Audio Thread (Consumer)  
PaintEvent event;
while (paintQueue_.pop(event)) {         // Lock-free pop
    spectralEngine_.updateTarget(        // Atomic parameter update
        logFreqFromY(event.ny),         // Canvas Y → frequency
        event.pressure * maxAmplitude_   // Pressure → amplitude  
    );
}
spectralEngine_.processBlock(buffer);    // RT-safe synthesis
```

## Quality Standards and Validation

### Testing Requirements

Every modification must include:

- **RT-Safety Test**: Validate audio thread compliance
- **Canvas Integration Test**: Verify paint→audio pipeline  
- **Regression Test**: Ensure no breaking changes
- **Performance Test**: Maintain sub-5ms processing latency

### Code Quality Standards

- Follow existing SpectralCanvas Pro naming conventions
- Comprehensive inline documentation for RT-safety decisions  
- Error handling for all failure modes
- Parameter validation and bounds checking

### Validation Commands

```bash
# Run RT-safety focused tests
ctest --test-dir build -C RelWithDebInfo -R "RT|Paint|Audio" --output-on-failure

# Quick validation pipeline  
tools/test/phase7-quick-validation.bat

# Comprehensive test suite
ctest --test-dir build -C RelWithDebInfo --output-on-failure
```

## Emergency Response Protocols

### RT-Safety Violations

1. **Immediate Response**:
   - Coordinate with rt-audio-guardian for comprehensive violation scan
   - Implement atomic parameter access fix
   - Add RT-safety test coverage to prevent regression

2. **Validation**:
   - Test with full ctest RT-safety suite
   - Verify sub-5ms processing latency maintenance
   - Validate canvas responsiveness under 10ms

### Build System Failures

1. **Diagnostic Approach**:
   - Examine CMakeLists.txt and JUCE configuration  
   - Check Windows macro hygiene issues
   - Coordinate with build-stability-monitor

2. **Resolution Strategy**:
   - Fix configuration issues maintaining JUCE compatibility
   - Test with `tools/test/phase7-quick-validation.bat`
   - Validate VST3 and Standalone builds

## Best Practices

### Development Guidelines

1. **Always Start with Analysis**: Use Read/Grep to understand existing patterns before implementing
2. **Plan Complex Features**: Use TodoWrite for multi-step features requiring coordination
3. **Validate RT-Safety**: Every audio thread modification needs rt-audio-guardian analysis
4. **Test Comprehensively**: Add test coverage before considering features complete
5. **Preserve Character**: Maintain EMU/Tube analog character in all audio paths

### Anti-Patterns to Avoid

- **Direct File Writes**: Always use Read first to understand existing code
- **RT-Safety Assumptions**: Never assume code is RT-safe without analysis
- **Isolated Development**: Coordinate with specialized agents for domain expertise
- **Insufficient Testing**: Never ship features without comprehensive test coverage

## Configuration Files

### Agent Contract
- **Location**: `docs/agents/spectralcanvas-pro-specialist.contract.yaml`
- **Purpose**: Defines agent capabilities, responsibilities, and constraints
- **Key Sections**: Architecture knowledge, RT-safety patterns, coordination protocols

### Operational Profile  
- **Location**: `docs/agents/profiles/spectralcanvas-pro-specialist/comprehensive-development-profile.yaml`
- **Purpose**: Detailed operational workflows and quality standards
- **Key Sections**: Expertise domains, problem-solving workflows, performance metrics

## Success Metrics

### Development Efficiency
- New canvas synthesis features per development cycle
- Archived components successfully integrated per cycle  
- 100% RT-safety compliance for audio modifications

### Code Quality
- Comprehensive test coverage for new features and restorations
- Zero build failures across supported configurations
- Sub-5ms audio processing latency preserved

### User Experience  
- Paint→audio feedback latency under 10ms
- EMU/Tube character preserved in all audio paths
- Faithful MetaSynth spectral synthesis workflow implementation

---

*This agent represents the culmination of SpectralCanvas Pro's architectural knowledge and development best practices, designed to maintain the project's RT-safety standards while implementing sophisticated spectral synthesis features with authentic analog character.*