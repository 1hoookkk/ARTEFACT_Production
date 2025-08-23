# Logarithmic Y→Frequency Mapping Design

## Executive Summary

Design for implementing logarithmic Y-coordinate to frequency mapping for the paint→audio system. This refactor will replace any existing linear mapping with perceptually-accurate logarithmic mapping aligned with human pitch perception.

## Background & Rationale

### Musical/Psychoacoustic Justification
- **Human Perception**: Hearing perceives frequency changes logarithmically
- **Musical Intervals**: Equal visual spacing = equal musical intervals (octaves)
- **Professional Standard**: All spectral editors use logarithmic frequency scales
- **Range Coverage**: 20Hz-20kHz covers full human hearing range

### Existing Codebase Patterns
The codebase already uses logarithmic frequency mapping in multiple locations:
- `SpectralSynthEngine.cpp`: `std::log2()` approach with 20Hz-20kHz range
- `PluginProcessor.cpp`: Power-of-1000 method 
- JUCE NormalisableRange with 0.3f skew factor for frequency parameters

## Implementation Options

### Option A: Direct Log Math (RECOMMENDED)
```cpp
namespace SC_Mapping {
    inline float mapYToFrequency(int y, int imageHeight, float minFreq = 20.0f, float maxFreq = 20000.0f) {
        const float normalizedY = static_cast<float>(y) / static_cast<float>(imageHeight);
        const float minLog2 = std::log2(minFreq);
        const float maxLog2 = std::log2(maxFreq);
        const float logFreq = minLog2 + normalizedY * (maxLog2 - minLog2);
        return std::pow(2.0f, logFreq);
    }
}
```

**Advantages:**
- Explicit, readable mathematics
- Consistent with existing SpectralSynthEngine patterns
- Excellent numerical stability
- No JUCE dependency for core mapping

### Option B: JUCE NormalisableRange with Skew
```cpp
namespace SC_Mapping {
    inline float mapYToFrequency(int y, int imageHeight, float minFreq = 20.0f, float maxFreq = 20000.0f) {
        static const juce::NormalisableRange<float> freqRange(minFreq, maxFreq, 1.0f, 0.3f);
        const float normalizedY = static_cast<float>(y) / static_cast<float>(imageHeight);
        return freqRange.convertFrom0to1(normalizedY);
    }
}
```

**Advantages:**
- Integrates with existing JUCE parameter system
- Matches existing parameter definitions
- Built-in value formatting

## Expected Values & Test Points

For standard 20Hz-20kHz range with 1024-pixel height:

| Y Position | Normalized Y | Expected Frequency | Test Tolerance |
|------------|--------------|-------------------|----------------|
| Y=0 (top) | 0.0 | 20.0 Hz | ±0.1 Hz |
| Y=512 (middle) | 0.5 | ~447 Hz | ±5 Hz |
| Y=1023 (bottom) | ~1.0 | 20000.0 Hz | ±10 Hz |

**Note**: 447Hz is the geometric mean of 20Hz and 20kHz in log space, approximately one semitone above A4 (440Hz).

## Unit Test Structure

```cpp
TEST_CASE(MappingLogic, YtoFrequencyLogarithmic) {
    // Boundary conditions
    ASSERT_NEAR(20.0f, SC_Mapping::mapYToFrequency(0, 1024), 0.1f);
    ASSERT_NEAR(20000.0f, SC_Mapping::mapYToFrequency(1023, 1024), 10.0f);
    
    // Middle point (geometric mean)
    ASSERT_NEAR(447.0f, SC_Mapping::mapYToFrequency(512, 1024), 5.0f);
    
    // Octave relationships
    float freq1 = SC_Mapping::mapYToFrequency(256, 1024);
    float freq2 = SC_Mapping::mapYToFrequency(512, 1024);
    ASSERT_NEAR(2.0f, freq2 / freq1, 0.1f);  // One octave ratio
}
```

## Implementation Plan

### Phase 1: Create Header
- **File**: `Source/DSP/SC_Mapping.h` (header-only for performance)
- **Approach**: Option A (Direct Log Math)
- **Interface**: `mapYToFrequency(y, imageHeight, minFreq, maxFreq)`

### Phase 2: Add Tests
- **File**: `Source/Tests/TestMappingLogic.cpp`
- **Framework**: TDD-Guard (consistent with SpectralSmoke)
- **Coverage**: Boundary conditions, middle point, octave relationships

### Phase 3: Integration
- Replace any existing linear mapping calls
- Maintain backward compatibility with configurable min/max frequencies
- Verify audio output quality

## Success Criteria

1. **Mathematical Accuracy**: All unit tests pass within specified tolerances
2. **Perceptual Quality**: Logarithmic mapping produces musically-intuitive results
3. **Performance**: Header-only implementation with no runtime overhead
4. **Compatibility**: Maintains existing API contracts where applicable

## Future Considerations

- **Configurable Range**: Allow runtime adjustment of min/max frequencies
- **Alternative Scales**: Support for other frequency scales (mel, bark) if needed
- **Bidirectional Mapping**: Add frequency→Y conversion for UI feedback