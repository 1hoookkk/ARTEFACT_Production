# EMUFilter Surface Reconnaissance Report

## Class Definition and API Surface

### EMUFilter Class (Source/Core/EMUFilter.h:8-127)
- **Namespace**: Global (no namespace)
- **Class Name**: `EMUFilter`
- **Type**: TPT State Variable Low-Pass filter with zero-delay topology

### Public Methods
- `EMUFilter()` = default constructor (line 11)
- `~EMUFilter()` = default destructor (line 12)
- `void prepare(double sampleRate, int blockSize)` (line 14)
- `void reset()` (line 24)
- `void setParams(float cutoffHzIn, float qIn)` (line 31)
- `void processBlock(juce::AudioBuffer<float>& buffer)` (line 40)
- `float getCutoffHz() const noexcept` (line 105)
- `float getQ() const noexcept` (line 106)

### Parameter Ranges (Source/Core/EMUFilter.h:30-37)
- **Cutoff**: 20.0f to 20000.0f Hz
- **Q/Resonance**: 0.1f to 10.0f

## ParamIDs.h Parameter Identifiers

### Filter Parameters (Source/ParamIDs.h:35-36)
- `ParamIDs::filterCutoff = "filterCutoff"`
- `ParamIDs::filterResonance = "filterResonance"`

## Test Coverage Analysis

### Test Files Found
1. **TestEMUFilter.h** (Source/Tests/TestEMUFilter.h:1-57)
   - Contains `TEST(EMUFilter, AttenuatesSignalAboveCutoff)` 
   - Uses FFT analysis with 4096 samples at 48kHz
   - Tests 1kHz sine through 200Hz cutoff filter
   - Assertion: magnitude < -20dB attenuation (line 56)

2. **TestEMUFilterMinimal.cpp** (Source/Tests/TestEMUFilterMinimal.cpp:1-102)
   - Standalone compilation test with main() function
   - Basic instantiation test (lines 15-18)
   - FFT attenuation test similar to TestEMUFilter.h
   - More realistic -6dB attenuation threshold (line 37)

3. **EMUFilter_RedTests.cpp** (Source/Tests/EMUFilter_RedTests.cpp:1-352)
   - Comprehensive JUCE UnitTest suite
   - **CRITICAL**: Contains API mismatch - calls `filter.processSample()` (line 62) and `filter.setSampleRate()` (line 111)
   - **CONFLICT**: Actual EMUFilter only has `processBlock()` and `prepare()` methods
   - Tests include: DC tracking, frequency response, resonance, sample rate switching, denormal handling

### Test Helpers Exposed (Source/Tests/TestEMUFilter.h:8-15)
- `TestHelpers::toDecibels(float linear)` - converts linear magnitude to dB

## CMake Integration

### CMakeLists.txt Wiring (CMakeLists.txt:52, 114, 147-171)
- **Core**: `Source/Core/EMUFilter.cpp` included in main target (line 52)
- **Test**: `Source/Tests/TestEMUFilterMinimal.cpp` in test target (line 114)
- **Red Tests**: Standalone executable `EMUFilter_RedTests` (lines 147-171)
  - Links with JUCE libraries
  - Registered as CTest target: `add_test(NAME EMUFilter_RedTests COMMAND EMUFilter_RedTests)` (line 171)

## Risk Assessment

### CRITICAL API Mismatch
- **EMUFilter_RedTests.cpp** calls non-existent methods:
  - `filter.processSample(float)` - actual API only has `processBlock(AudioBuffer<float>&)`
  - `filter.setSampleRate(float)` - actual API uses `prepare(double, int)`
  - `filter.setFrequency(float)` - actual API uses `setParams(float, float)`
  - `filter.setResonance(float)` - actual API uses `setParams(float, float)`

### Missing Implementation
- **Source/Core/EMUFilter.cpp** contains only `#include "EMUFilter.h"` (line 1)
- All functionality is header-only (inline implementation)

### Test Inconsistencies
- TestEMUFilter.h expects -20dB attenuation
- TestEMUFilterMinimal.cpp expects -6dB attenuation  
- Different test signal configurations across test files

## File Citations Summary
1. `Source/Core/EMUFilter.h:8` - Class definition
2. `Source/Core/EMUFilter.h:31` - setParams method
3. `Source/ParamIDs.h:35-36` - Parameter identifiers
4. `Source/Tests/TestEMUFilter.h:17` - FFT attenuation test
5. `Source/Tests/EMUFilter_RedTests.cpp:62` - API mismatch processSample call
6. `CMakeLists.txt:147-171` - Test target configuration