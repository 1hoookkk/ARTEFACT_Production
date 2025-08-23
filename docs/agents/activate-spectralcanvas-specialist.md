# Activating the SpectralCanvas Pro Specialist Agent

## Quick Start

To activate the SpectralCanvas Pro Specialist Agent for your development session:

### 1. Automatic Activation

The agent automatically activates when working on:
- Files in `Source/Core/` (audio processing)
- Files in `Source/Spectral/` (STFT processing) 
- Component restoration from `/_archive/`
- RT-safety issues in audio processing
- Canvas synthesis feature development

### 2. Manual Activation Command

```
@spectralcanvas-pro-specialist: [your request here]

Examples:
- @spectralcanvas-pro-specialist: Help me implement a new canvas brush that controls spectral harmonics
- @spectralcanvas-pro-specialist: Restore the GranularEngine component from the archive safely  
- @spectralcanvas-pro-specialist: Analyze this processBlock() method for RT-safety violations
- @spectralcanvas-pro-specialist: Plan integration of EMU character into the new filter chain
```

### 3. Complex Feature Development

For complex features requiring coordination:

```
I need to implement a MetaSynth-style spectral painting feature that:
1. Captures paint strokes on a frequency-time canvas
2. Maps paint pressure to harmonic amplitude
3. Maintains RT-safety throughout the audio pipeline
4. Preserves the EMU analog character in all processing

Use comprehensive planning with TodoWrite and coordinate with RT-safety analysis.
```

## Agent Capabilities Summary

### Primary Expertise
- **Canvas→Audio Pipeline**: Lock-free SPSC queues, real-time parameter mapping
- **RT-Safety Enforcement**: Violation detection, atomic parameter patterns
- **Component Restoration**: Archive analysis, incremental integration, test-driven activation
- **EMU Analog Character**: Always-on character preservation, vintage workflow fidelity
- **MetaSynth Integration**: Spectral synthesis, additive algorithms, canvas control

### Coordination Partners
- **rt-audio-guardian**: RT-safety violation analysis and enforcement
- **build-stability-monitor**: Windows build configuration and JUCE compatibility
- **dsp-frequency-oracle**: Frequency mapping validation and STFT analysis

### Tool Specialization
- **Read/Grep**: Architectural analysis and code pattern discovery
- **Edit/MultiEdit**: RT-safe code modifications with violation prevention
- **TodoWrite**: Complex feature planning and multi-step coordination
- **Glob**: Archive exploration and component discovery

## Common Usage Patterns

### Canvas Feature Development
```
I want to add a new spectral brush that responds to paint velocity and creates sweeping frequency modulation. Ensure RT-safety and maintain EMU character throughout.

Expected workflow:
1. Analyze existing PaintQueue and SpectralSynthEngine interfaces
2. Plan RT-safe parameter mapping for velocity→modulation
3. Implement UI thread paint capture with velocity calculation
4. Create audio thread modulation synthesis with atomic parameters
5. Test with comprehensive RT-safety validation
```

### Component Restoration  
```
Restore the advanced granular synthesis engine from the archive. It's located at /_archive/sophisticated_components/GranularEngine.cpp and needs to integrate with the current RT-safe architecture.

Expected workflow:
1. Use TodoWrite for restoration planning
2. Analyze archived component dependencies and interfaces
3. Map integration points in current codebase
4. Implement incremental integration preserving RT-safety
5. Add comprehensive test coverage and validation
```

### RT-Safety Troubleshooting
```
I'm getting audio dropouts and suspect RT-safety violations in the new filter chain. Can you analyze the processBlock() methods and coordinate with rt-audio-guardian for comprehensive violation detection?

Expected workflow:
1. Grep for RT violation patterns in audio processing files
2. Read processBlock() implementations for detailed analysis  
3. Coordinate with rt-audio-guardian for automated violation scan
4. Implement atomic parameter fixes with memory ordering
5. Validate with RT-safety test suite
```

## Quality Standards

All agent interactions maintain:

- **RT-Safety Compliance**: Zero locks/allocations/logging on audio thread
- **Test Coverage**: Comprehensive testing for all new features and restorations
- **EMU Character**: Preservation of analog character throughout audio paths
- **Performance**: Sub-5ms audio processing latency maintenance
- **Architecture Consistency**: Adherence to existing patterns and interfaces

## Success Indicators

- Canvas features respond in real-time (< 10ms paint→audio latency)
- RT-safety tests pass 100% for all audio thread modifications  
- Restored components integrate without breaking existing functionality
- EMU/Tube analog character preserved in all audio processing paths
- Build stability maintained across VST3 and Standalone configurations