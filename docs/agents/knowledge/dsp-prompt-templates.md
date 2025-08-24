# SpectralCanvas Pro - Audio DSP Prompt Templates

## Overview
Specialized prompt templates designed to extract maximum expertise from each subagent for SpectralCanvas Pro development. These templates include essential context, technical requirements, and domain-specific terminology that each agent needs to provide optimal solutions.

---

## 🌈 SPECTRAL PROCESSING EXPERT TEMPLATES

### Template: Spectral Algorithm Implementation
```
Using the audio-engine-architect (Spectral Processing Expert), implement [SPECTRAL FEATURE] for SpectralCanvas Pro.

METASYNTH/CDP CONTEXT:
- SpectralCanvas Pro is a MetaSynth/CDP hybrid synthesizer
- Target: Professional spectral morphing and paint-to-audio synthesis
- Framework: JUCE with real-time audio processing constraints

SPECTRAL REQUIREMENTS:
- FFT Size: 1024-4096 points (configurable)
- Sample Rate: 44.1kHz-192kHz support  
- Overlap: 75% (4x oversampling) for smooth spectral morphing
- Window: Hann window for optimal frequency resolution
- Latency Target: <10ms total processing delay

CDP PARADIGM INTEGRATION:
- Support spectral blur, stretch, morph operations
- Implement phase vocoder with time/pitch independence  
- Enable spectral filtering and masking capabilities
- Maintain phase coherence for musical results

REAL-TIME CONSTRAINTS:
- Must process in audio thread (no allocations)
- Lock-free operation required
- SIMD optimization preferred
- Maximum CPU: 30% at 48kHz/512 samples

TECHNICAL SPECIFICATIONS:
[Include specific algorithm requirements, mathematical formulations, expected inputs/outputs]

Please implement with focus on CDP-style spectral processing excellence.
```

### Template: MetaSynth Oscillator Management
```
Using the audio-engine-architect (Spectral Processing Expert), optimize the spectral oscillator system for MetaSynth-style synthesis.

OSCILLATOR SPECIFICATIONS:
- Pool Size: Up to 1024 simultaneous oscillators
- Frequency Range: 20Hz - 20kHz (logarithmic mapping from Y-axis)
- Amplitude Control: Pressure-sensitive (0.0-1.0 range)
- Pan Position: Color-mapped (Red=Left, Green=Right, Yellow=Center)
- Lifecycle: Attack/decay based on paint stroke dynamics

PERFORMANCE REQUIREMENTS:
- Oscillator allocation: O(1) complexity
- Phase accuracy: Sample-perfect synchronization
- Memory layout: Cache-friendly for SIMD processing
- Voice stealing: LRU algorithm for smooth transitions

PAINT-TO-AUDIO INTEGRATION:
- Coordinate system: Canvas (0,0) = bottom-left, Y-axis = frequency
- Color processing: RGB to stereo position mapping
- Pressure dynamics: Real-time amplitude modulation
- Brush characteristics: Width affects spectral spread

Please implement with focus on maximum polyphony and minimal latency.
```

---

## 🎨 METASYNTH IMAGE SYNTH SPECIALIST TEMPLATES

### Template: Visual-to-Audio Mapping
```
Using the paint-to-audio-mapper (MetaSynth Image Synth Specialist), implement [VISUAL FEATURE] following authentic MetaSynth paradigm.

METASYNTH CORE MAPPING:
- X-axis: Time progression (left-to-right = temporal sequence)  
- Y-axis: Frequency/pitch (bottom=20Hz, top=20kHz, logarithmic scale)
- Brightness: Amplitude/volume (black=silence, white=full amplitude)
- Color: Stereo positioning and harmonic content
  * Red component → Left channel weighting
  * Green component → Right channel weighting  
  * Yellow (Red+Green) → Center positioning
  * Blue → Non-audible guide elements

CANVAS SPECIFICATIONS:
- Dimensions: 800x600 pixels (configurable)
- Coordinate system: Bottom-left origin (mathematical, not screen coordinates)
- Frequency mapping: Logarithmic Y-scale for musical perception
- Time resolution: Configurable zoom (ms to minutes per pixel)

PAINT STROKE DYNAMICS:
- Brush pressure → Multiple parameter control
- Brush velocity → Temporal characteristics
- Brush angle → Spectral tilt and harmonic content
- Multi-touch → Harmonic relationship creation

VISUAL FEEDBACK REQUIREMENTS:
- Real-time spectrum analyzer overlay
- Color-coded frequency zones
- Amplitude visualization with brightness
- Interactive frequency grid (optional)

Please implement maintaining authentic MetaSynth behavior and musical intuition.
```

### Template: Image Import and Analysis
```
Using the paint-to-audio-mapper (MetaSynth Image Synth Specialist), create an image-to-sound converter.

IMAGE PROCESSING PIPELINE:
- Supported formats: PNG, JPG, BMP, TIFF
- Color space: RGB with alpha channel support
- Resolution: Scalable to canvas dimensions  
- Preprocessing: Contrast, brightness, gamma correction

SPECTRAL CONVERSION STRATEGY:
- Pixel scanning: Left-to-right, bottom-to-top
- Frequency assignment: Y-position to log-frequency mapping
- Amplitude extraction: Luminance (brightness) values
- Stereo placement: Red/Green channel analysis
- Temporal spacing: X-position to time-code mapping

OPTIMIZATION REQUIREMENTS:
- Large image handling: Progressive loading/streaming
- Memory efficiency: Tile-based processing for large images  
- Real-time preview: Interactive parameter adjustment
- Quality settings: Trade-off between accuracy and performance

MUSICAL CONSIDERATIONS:
- Harmonic series detection: Identify musical relationships in images
- Noise reduction: Filter out non-musical visual noise
- Dynamic range: Optimal audio level mapping
- Frequency distribution: Ensure balanced spectral content

Please focus on creating musical results from visual content.
```

---

## 🔬 DSP ALGORITHM & ANALOG MODELING EXPERT TEMPLATES

### Template: E-mu Audity Circuit Modeling
```
Using the code-quality-guardian (DSP Algorithm & Analog Modeling Expert), model the [AUDITY COMPONENT] with authentic analog character.

E-MU AUDITY CONTEXT:
- Hardware: E-mu Audity 2000 (early 1990s digital sampler)
- Character: Known for warm analog filters and tube-like saturation
- Target circuits: SSM2040 lowpass, CEM3389 multimode filter
- Sonic signature: Musical self-oscillation, analog warmth

CIRCUIT SPECIFICATIONS:
- SSM2040: 4-pole lowpass with exponential cutoff response
- CEM3389: Multimode (LP/BP/HP/Notch) with resonance
- Tube saturation: Asymmetric clipping with even/odd harmonics
- Parameter smoothing: Analog-style zipper noise prevention

MATHEMATICAL REQUIREMENTS:
- Zero-delay feedback (ZDF) topology for stability
- Oversampling: Minimum 4x for non-linear processes  
- Frequency response: Match hardware measurements within 1dB
- Phase response: Maintain analog-like phase characteristics
- Self-oscillation: Accurate Q behavior at high resonance

IMPLEMENTATION STANDARDS:
- Numerical stability: No denormals or coefficient explosions
- Parameter ranges: Smooth response across full parameter range
- Temperature drift: Optional subtle parameter variations
- Component tolerances: Model analog component variations

Please implement with focus on authentic analog musicality and character.
```

### Template: Filter Algorithm Verification
```
Using the code-quality-guardian (DSP Algorithm & Analog Modeling Expert), validate and optimize [FILTER ALGORITHM] for mathematical correctness.

FILTER VERIFICATION REQUIREMENTS:
- Frequency response: Measure with sine sweep (20Hz-20kHz)
- Phase response: Verify phase relationships and group delay
- Impulse response: Check for overshoots, ringing, instability
- Step response: Validate transient behavior
- Pole-zero analysis: Confirm stability margins

NUMERICAL ANALYSIS:
- Coefficient precision: Double precision for calculations, float for processing
- Edge case handling: DC, Nyquist frequency, extreme parameter values
- Denormal prevention: FTZ/DAZ or add small DC offset
- Overflow protection: Saturation or scaling strategies
- Round-off error accumulation: Analysis and mitigation

TEST VECTORS:
- Pink noise: Long-term stability and statistical properties
- Sine sweep: Frequency response and harmonic distortion
- Impulse train: Transient response and ringing
- DC and Nyquist: Edge case behavior
- Extreme settings: Parameter boundary testing

REFERENCE IMPLEMENTATION:
- MATLAB/Python model for comparison
- Hardware measurements (if available)
- Published literature references
- Known good implementations for verification

Please ensure mathematical rigor and provide comprehensive validation data.
```

---

## ⚡ QUANTUM AUDIO ARCHITECT TEMPLATES

### Template: Lock-free Data Structure Implementation
```
Using the juce-integration-specialist (Quantum Audio Architect), implement a lock-free [DATA STRUCTURE] for paint-to-audio processing.

REAL-TIME SAFETY REQUIREMENTS:
- Zero blocking operations in audio thread
- No mutex, critical section, or semaphore usage
- No dynamic memory allocation (new/delete/malloc/free)
- No system calls or file I/O operations
- No unbounded loops or recursive algorithms

ATOMIC OPERATION SPECIFICATIONS:
- Memory ordering: Use acquire/release semantics appropriately
- ABA problem prevention: Use hazard pointers or epochs
- Memory barriers: Minimize while maintaining correctness
- CAS operations: Compare-and-swap for lock-free algorithms
- False sharing prevention: Align data to cache line boundaries

PERFORMANCE TARGETS:
- Latency: Sub-microsecond operation completion
- Throughput: Handle paint data at 60fps+ without dropping
- CPU usage: Minimal overhead on audio thread
- Scalability: Support multiple producer/consumer threads
- Memory footprint: Minimize cache pollution

PAINT-TO-AUDIO CONTEXT:
- Data flow: UI thread → Audio thread communication
- Queue size: Sufficient for burst painting without overflow
- Data format: Coordinate, pressure, color information
- Processing rate: Real-time audio callback requirements
- Error handling: Graceful degradation when queue full

Please implement with focus on deterministic real-time performance.
```

### Template: SIMD Optimization Implementation
```
Using the juce-integration-specialist (Quantum Audio Architect), vectorize [DSP ALGORITHM] using SIMD intrinsics.

SIMD TARGET SPECIFICATIONS:
- Primary: AVX2 (8x float, 4x double)
- Fallback: AVX (8x float, 4x double) 
- Baseline: SSE (4x float, 2x double)
- Future: AVX-512 (16x float, 8x double) where available
- Detection: Runtime CPU capability detection

VECTORIZATION STRATEGY:
- Data alignment: 32-byte (AVX2) or 64-byte (AVX-512) boundaries
- Loop structure: Minimize scalar remainder processing
- Instruction selection: Optimal intrinsic choices for operations
- Register utilization: Maximize SIMD register usage
- Memory access: Minimize loads/stores, favor streaming

SPECTRAL PROCESSING CONTEXT:
- Oscillator processing: Vectorize multiple oscillators simultaneously
- FFT operations: SIMD-optimized butterfly computations
- Filter processing: Vectorized biquad sections
- Color processing: Parallel RGB to audio parameter conversion
- Coordinate transformation: Batch processing of paint coordinates

PERFORMANCE VALIDATION:
- Benchmark: Compare against scalar implementation
- Target speedup: Minimum 2x, ideally 4-8x improvement
- CPU utilization: Monitor SIMD unit usage efficiency
- Cache impact: Measure cache hit/miss ratios
- Power consumption: Consider mobile device constraints

Please implement with focus on maximum throughput and efficiency.
```

---

## 🎭 SONIC ALCHEMIST TEMPLATES

### Template: Creative Workflow Interface Design
```
Using the ui-ux-design-specialist (Sonic Alchemist), design [CREATIVE INTERFACE] that bridges visual art and music composition.

ARTIST/MUSICIAN WORKFLOW CONTEXT:
- Visual artists: Familiar with painting tools, color theory, canvas metaphors
- Musicians: Understand frequency, rhythm, harmony, musical time
- Hybrid users: Want to explore the intersection of visual and sonic art
- Performance users: Need real-time control with minimal interface distraction

METASYNTH PARADIGM INTERFACE REQUIREMENTS:
- Canvas orientation: Mathematical coordinates (Y-up for frequency)
- Frequency visualization: Logarithmic Y-axis with musical note markers
- Time representation: Left-to-right temporal flow with tempo grid
- Color interface: Intuitive RGB to stereo positioning
- Brush dynamics: Pressure, velocity, angle affecting multiple parameters

VISUAL-AUDIO HARMONY PRINCIPLES:
- Frequency ↔ Color: Warm colors for low frequencies, cool for high
- Amplitude ↔ Brightness: Luminance directly maps to volume
- Stereo field ↔ Color temperature: Red=warm/left, Blue=cool/right  
- Harmonic content ↔ Saturation: Pure tones bright, complex sounds saturated
- Temporal flow ↔ Animation: Visual elements that move with audio progression

INTERACTION DESIGN PATTERNS:
- Painting gestures: Natural brush strokes create musical phrases
- Multi-touch: Harmonic intervals and chord progressions
- Pressure sensitivity: Dynamic expression and amplitude control
- Gesture recognition: Musical gestures translated to visual strokes
- Context menus: Appear based on current tool and canvas content

Please design interfaces that make complex synthesis feel intuitive and musical.
```

### Template: Performance Mode Interface
```
Using the ui-ux-design-specialist (Sonic Alchemist), create a performance-optimized interface for live SpectralCanvas Pro use.

PERFORMANCE CONTEXT:
- Environment: Stage lighting, audience pressure, limited setup time
- Hardware: MIDI controllers, touch screens, tablet/stylus combinations
- Timing: Real-time performance with zero latency tolerance
- Visual: Minimal distraction, maximum expression capability
- Reliability: Bulletproof operation under performance stress

INTERFACE MINIMIZATION STRATEGY:
- Essential controls only: Hide all non-performance-critical elements
- Large targets: Touch-friendly controls for stage lighting conditions
- Visual feedback: Clear parameter state indication without complexity
- Gesture shortcuts: Quick access to common performance techniques
- Emergency controls: Instant mute, panic button, quick preset recall

REAL-TIME CONTROL REQUIREMENTS:
- Parameter automation: Real-time recording and playback of control data
- Macro controls: Single controls affecting multiple synthesis parameters  
- Preset morphing: Smooth transitions between different sonic landscapes
- Performance memory: Recall complete performance states instantly
- MIDI integration: Seamless controller mapping and real-time adjustment

VISUAL PERFORMANCE ELEMENTS:
- Spectrum display: Real-time frequency content visualization
- Paint history: Visual representation of performed gestures
- Parameter feedback: Current synthesis state display
- Audio metering: Level and frequency analysis for monitoring
- Visual effects: Optional audience-visible projection mapping

Please design for maximum expression with minimum complexity during performance.
```

---

## 🕵️ SONIC INVESTIGATOR TEMPLATES

### Template: Audio Debugging Investigation  
```
Using the debug-specialist (Sonic Investigator), investigate [AUDIO ISSUE] in SpectralCanvas Pro's real-time processing system.

AUDIO DEBUGGING CONTEXT:
- Real-time audio: 48kHz/256 sample processing with <5ms latency requirement
- Spectral synthesis: Complex FFT/IFFT with phase vocoder processing
- Paint-to-audio: Visual interaction triggering audio synthesis
- Multi-threading: UI thread, audio thread, and background processing coordination
- DSP complexity: Multiple synthesis engines running simultaneously

AUDIO-SPECIFIC INVESTIGATION AREAS:
- Buffer underruns/overruns: Audio callback timing and processing efficiency
- Sample rate issues: Incorrect sample rate handling or conversion
- Phase cancellation: Spectral processing phase coherence problems
- Denormal numbers: CPU spike investigation in floating-point DSP
- Click/pop artifacts: Discontinuities in audio processing chain
- Frequency response errors: Incorrect spectral mapping or filtering

REAL-TIME DEBUGGING TECHNIQUES:
- Audio thread safety: Check for locks, allocations, or blocking operations
- Performance profiling: CPU usage analysis during audio processing
- Signal flow tracing: Track audio from paint input through synthesis output
- Parameter validation: Ensure DSP parameters remain within valid ranges  
- Memory barriers: Verify atomic operations and thread synchronization
- SIMD debugging: Validate vectorized DSP operation correctness

DIAGNOSTIC TOOLS AND APPROACHES:
- Spectrum analyzer: Visual analysis of output frequency content
- Oscilloscope view: Time-domain analysis of audio signals
- Performance counters: Real-time CPU and memory usage monitoring
- Debug logging: Strategic logging that doesn't impact audio performance
- A/B testing: Compare known-good reference against current implementation

Please provide comprehensive analysis with specific evidence and actionable solutions.
```

### Template: Performance Profiling Investigation
```
Using the debug-specialist (Sonic Investigator), profile and optimize [PERFORMANCE BOTTLENECK] affecting real-time audio performance.

PERFORMANCE PROFILING CONTEXT:
- Target performance: <50% CPU usage at 48kHz/512 samples
- Hardware constraints: Support from mobile devices to high-end workstations
- Real-time requirements: Consistent performance without dropouts or glitches
- Complex processing: Spectral synthesis, paint-to-audio, and analog modeling
- Multi-core utilization: Optimal thread distribution and load balancing

PROFILING METHODOLOGY:
- CPU profiling: Identify hotspots using sampling and instrumentation profilers
- Memory profiling: Track allocations, cache misses, and memory access patterns
- Cache analysis: Measure L1/L2/L3 cache hit ratios and optimization opportunities
- Branch prediction: Identify mispredicted branches causing pipeline stalls
- SIMD efficiency: Measure vectorization effectiveness and register utilization

AUDIO-SPECIFIC PERFORMANCE METRICS:
- Audio callback timing: Measure actual vs. expected callback intervals
- Processing latency: End-to-end latency from paint input to audio output
- Thread synchronization: Measure time spent in atomic operations and barriers
- DSP algorithm efficiency: Per-algorithm CPU usage and optimization potential
- Memory bandwidth: Track memory throughput requirements and bottlenecks

OPTIMIZATION STRATEGY DEVELOPMENT:
- Algorithm complexity: Analyze O(n) characteristics and scaling behavior
- Data structure efficiency: Evaluate cache-friendly vs. computation-friendly layouts
- Parallelization opportunities: Identify tasks suitable for multi-threading
- SIMD vectorization: Determine optimal vectorization strategies
- Approximation techniques: Evaluate trade-offs between accuracy and performance

Please provide detailed performance analysis with specific optimization recommendations and expected improvements.
```

---

## Multi-Agent Collaboration Templates

### Template: Complex Feature Implementation
```
Multi-agent collaboration for implementing [COMPLEX FEATURE] in SpectralCanvas Pro.

COLLABORATION SEQUENCE:
1. Sonic Alchemist: Design user interface and workflow requirements
2. MetaSynth Image Synth Specialist: Define visual-to-audio mapping specifications  
3. Spectral Processing Expert: Implement core DSP algorithms and audio engine integration
4. DSP Algorithm & Analog Modeling Expert: Validate mathematical correctness and add E-mu character
5. Quantum Audio Architect: Optimize for real-time performance and implement SIMD acceleration
6. Sonic Investigator: Profile performance, debug issues, and validate final implementation

CROSS-AGENT COMMUNICATION:
- Technical specifications document for consistent implementation
- Interface contracts defining API boundaries between components
- Performance requirements shared across all agents
- Testing criteria that each agent should validate
- Integration checkpoints for collaborative verification

QUALITY ASSURANCE:
- Each agent validates their domain expertise requirements
- Cross-functional testing ensures seamless integration
- Performance benchmarking against established targets
- User experience validation through workflow testing
- Long-term stability testing under various load conditions
```

---

## Template Customization Guidelines

### Agent-Specific Context Enhancement
1. **Always include SpectralCanvas Pro project context** - mention MetaSynth/CDP hybrid nature
2. **Specify real-time constraints** - audio thread safety, latency requirements, CPU usage targets
3. **Reference domain expertise** - use terminology and concepts specific to each agent's specialization
4. **Provide technical specifications** - include specific parameters, ranges, and quality requirements
5. **Define success criteria** - clear metrics for evaluating implementation success

### Adaptive Template Usage
- **Modify complexity** based on task scope (simple parameter change vs. major feature)
- **Adjust technical depth** based on implementation phase (research vs. optimization)
- **Include collaboration cues** when multiple agents need to work together
- **Reference existing codebase** with specific file names and function signatures when relevant
- **Provide context continuity** by referencing previous work and decisions

### Template Evolution
These templates should evolve based on:
- **Agent performance feedback** - refine prompts that produce better results
- **Project phase changes** - adapt templates as SpectralCanvas Pro development progresses
- **New feature requirements** - create specialized templates for emerging functionality
- **Performance discoveries** - incorporate new optimization techniques and approaches
- **User feedback integration** - reflect real-world usage patterns in template design

Remember: The goal is to provide each specialized agent with the rich context and domain-specific information they need to deliver expert-level solutions for SpectralCanvas Pro development.