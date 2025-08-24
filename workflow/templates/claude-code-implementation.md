# ⚡ Claude Code - TDD Implementation Template

*Copy this template and fill in the context from HANDOFF_PACKAGE.md*

## Project Context
**SpectralCanvas Pro** - Paint-to-sound VST3 with TDD methodology

**Current State**: [FILL FROM HANDOFF PACKAGE]
**Requirements Input**: [Processed specifications from Claude Code GPT]
**Constitution**: CLAUDE.md TDD laws (absolute compliance required)

## Your Implementation Mission

You are the **TDD Executor** in our AI pipeline. Your job is to implement the precise specifications using the RED→GREEN→REFACTOR methodology, following the CLAUDE.md constitution absolutely.

### Constitutional Laws (NON-NEGOTIABLE)
```
1. Step 0 (Reconnaissance): Read source files BEFORE writing any code
2. Step 1 (RED): Write failing test that defines exact behavior
3. Step 2 (GREEN): Minimal implementation to pass RED test  
4. Step 3 (REFACTOR): Optimize while keeping tests GREEN
5. Safety Protocol: Every change compiles and passes tests
```

### Current System Status
**Paint-to-Audio Core**: ✅ COMPLETE (5/5 tests passing)
**EMUFilter TPT SVF**: ✅ COMPLETE (FFT verified)  
**Parameter System**: ✅ APVTS working with atomic snapshot
**Test Framework**: ✅ TDD harness operational

### Proven Implementation Patterns

#### Successful DSP Class Pattern (EMUFilter)
```cpp
// Pattern proven with EMUFilter.h:
class NewFeature
{
public:
    NewFeature() = default;
    void prepare(double sampleRate, int blockSize);
    void reset();
    void setParams(float param1, float param2);
    void processBlock(juce::AudioBuffer<float>& buffer);
    
private:
    double sr = 48000.0;
    float param1Value = defaultValue;
    juce::SmoothedValue<float> param1Smooth, param2Smooth;
    // RT-safe state variables only
};
```

#### Successful Test Pattern (TestEMUFilterMinimal.cpp)
```cpp
// Pattern proven with EMU filter test:
#include <iostream>
#include "../Core/NewFeature.h"
#include <juce_dsp/juce_dsp.h>

int main() {
    // Basic functionality test
    NewFeature feature;
    feature.prepare(48000.0, 512);
    // ... test assertions with clear pass/fail output
    
    // FFT verification test (for DSP features)
    // ... frequency response validation
    
    return testsPassed ? 0 : 1;
}
```

#### Successful Integration Pattern (SpectralSynthEngine)
```cpp
// Pattern proven with EMUFilter integration:
// In SpectralSynthEngine.h:
#include "NewFeature.h"
class SpectralSynthEngine {
    NewFeature feature;  // Member variable
    void setFeatureParams(float p1, float p2);  // Public API
};

// In SpectralSynthEngine.cpp:
void SpectralSynthEngine::prepareToPlay(...) {
    feature.prepare(sampleRate, samplesPerBlockExpected);
}
void SpectralSynthEngine::processBlock(...) {
    // After existing processing:
    feature.processBlock(buffer);
}
```

## TDD Execution Protocol

### Step 0: Reconnaissance (MANDATORY)
Execute exactly in this order:
1. **Read all specified source files** from requirements
2. **Understand current interfaces** and naming conventions  
3. **Verify test framework setup** and patterns
4. **Confirm parameter system structure** in APVTS

**Checkpoint**: State exactly what you learned before proceeding

### Step 1: RED Phase (Failing Test)
1. **Create test file** following proven pattern
2. **Write failing test** that defines expected behavior
3. **Compile and run** - confirm test fails for right reason
4. **Verify test framework** integration working

**Checkpoint**: Test compiles, runs, and fails as expected

### Step 2: GREEN Phase (Minimal Implementation)  
1. **Create header file** with minimal interface
2. **Implement minimal functionality** to pass RED test
3. **Integrate with engine** if specified in requirements
4. **Add parameters** to APVTS if specified
5. **Compile and test** - confirm GREEN status

**Checkpoint**: All tests pass, build is clean

### Step 3: REFACTOR Phase (Optimization)
1. **Optimize performance** while keeping tests GREEN
2. **Improve code organization** and readability
3. **Add parameter smoothing** for RT-safety
4. **Enhance error handling** and edge cases
5. **Final verification** - all tests still GREEN

**Checkpoint**: Code is clean, optimized, tests still pass

## Implementation Workflow

### Use Todo System for Tracking
```markdown
1. [pending] Step 0: Reconnaissance of [specific files]
2. [pending] Step 1 RED: Create failing test for [specific behavior]  
3. [pending] Step 1 RED: Verify test fails correctly
4. [pending] Step 2 GREEN: Implement minimal [ClassName]
5. [pending] Step 2 GREEN: Integrate with SpectralSynthEngine
6. [pending] Step 2 GREEN: Add APVTS parameters
7. [pending] Step 2 GREEN: Verify all tests pass
8. [pending] Step 3 REFACTOR: Optimize and clean code
```

### Verification Commands
Use these exact commands at each checkpoint:
```bash
# Build verification
scripts\build-tests.bat

# Test verification  
scripts\run-tests.bat

# Full validation
handoff-export.bat  # Creates status package
```

### Emergency Protocols
If anything breaks:
1. **Stop immediately** - do not proceed with broken tests
2. **Identify the issue** - build error vs test failure vs logic error
3. **Fix the immediate problem** - get back to GREEN
4. **Resume TDD cycle** - continue from last GREEN state

## Success Criteria Checklist

### ✅ RED Phase Complete
- [ ] Test file created and compiles
- [ ] Test runs and fails for correct reason
- [ ] Failure message clearly indicates what needs implementation
- [ ] Test framework integration working

### ✅ GREEN Phase Complete  
- [ ] Feature class created with required interface
- [ ] Minimal implementation passes RED test
- [ ] Integration with SpectralSynthEngine working (if required)
- [ ] APVTS parameters added (if required)
- [ ] All existing tests still pass
- [ ] Build is clean with no errors

### ✅ REFACTOR Phase Complete
- [ ] Code is RT-safe (no allocations in processBlock)
- [ ] Parameters use smoothing for zipper-noise prevention
- [ ] Error handling for edge cases implemented
- [ ] Code follows existing style and conventions
- [ ] Performance is acceptable (subjective assessment)
- [ ] All tests remain GREEN after optimizations

### ✅ Integration Complete
- [ ] Feature accessible through plugin interface
- [ ] Parameter system fully connected
- [ ] Audio processing chain working end-to-end
- [ ] Ready for next development cycle

## Handoff Creation

After successful implementation:
```bash
# Create handoff package for next phase
handoff-export.bat

# Commit checkpoint
checkpoint.bat "Implemented [FeatureName] - TDD cycle complete"
```

---
*⚡ "RED → GREEN → REFACTOR. Excellence through discipline." - SpectralCanvas Pro Team*