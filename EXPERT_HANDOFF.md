# SpectralCanvas Pro - Expert LLM Handoff (v2)

## Overview
SpectralCanvas Pro is a JUCE-based audio plugin for canvas-driven spectral synthesis.  
**Goal:** Shape the user interaction + UX flow to deliver a compelling MVP.

---

## Current State
✅ CMake builds/tests configured  
✅ RT-safe paint→audio pipeline operational  
✅ VST3 + Standalone working  

🚧 In progress: frequency mapping, visual feedback, APVTS integration  
❓ Needs direction: UX model, visual language, feature prioritization  

---

## Core Architecture (working)
Canvas Gesture → PaintQueue (lock-free) → SpectralSynthEngine → Audio Output  

---

## Decision Checklist (Expert Input Needed)

### UX / Workflow
- Should painting produce **immediate audio** or a **paint-then-play** workflow?
- How should **pressure → amplitude** and **color → frequency** mappings behave?
- What's the ideal **canvas resolution/aspect ratio**?

### Features / Scope
- Define **MVP vs. advanced features** (presets, export, real-time vs offline).
- Which parameters should be **exposed in DAW automation**?
- What's the **minimum viable visual feedback system** (spectrogram, abstract, waveform)?

### Strategic
- Who is the **primary target user** (sound designers, experimental musicians, educators)?  
- What's the **core "magic moment"** we're designing around?  
- How should we position vs. existing spectral tools (Photosounder, Metasynth, etc.)?  

---

## Development Context
```bash
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo --target ALL_BUILD
ctest --test-dir build -C RelWithDebInfo --output-on-failure
```

### Emergency Debug
Press 'T' in plugin editor → toggles test tone (audio debugging)

### Key Files
- `Source/Core/PluginProcessor.h` - Audio processing core
- `Source/GUI/PluginEditor.h` - UI architecture  
- `Source/Core/SpectralSynthEngine.h` - Synthesis engine
- `CLAUDE.md` - Development workflow

---

## Bug Backlog

### 🔴 Blocking Issues
- [ ] **AudioTrace linker errors** - `g_traceBuffer` undefined symbol in test suite
- [ ] **Test executables incomplete** - Some tests fail to build/link properly
- [ ] **RT-safety validation gaps** - Need comprehensive lock-free verification

### 🟡 Quality Issues  
- [ ] **Performance regression tests missing** - No automated performance monitoring
- [ ] **Parameter automation incomplete** - APVTS integration partially implemented
- [ ] **Visual feedback lag** - Canvas updates not synchronized with audio processing

### 🟢 Enhancement Opportunities
- [ ] **Preset system architecture** - Save/load canvas states + audio parameters
- [ ] **Cross-platform UI consistency** - Windows/Mac/Linux visual parity
- [ ] **Advanced spectral features** - Harmonic/formant analysis tools
- [ ] **Export capabilities** - Audio rendering, canvas state formats

---

## Task List for LLMs

### Immediate Actions
1. **Fix AudioTrace dependencies** in test suite (linker errors)
2. **Complete test suite builds** - ensure all 9 tests compile and run
3. **Implement basic frequency mapping** - canvas X/Y → spectral coordinates

### Strategic Decisions Needed
1. **Define core UX paradigm** - paint-while-hear vs. paint-then-play
2. **Specify MVP feature set** - minimum viable product scope
3. **Design visual feedback system** - what users see while painting

### Next Development Sprint
1. **Parameter binding completion** - APVTS → synthesis engine integration
2. **Visual polish pass** - UI/UX implementation based on expert decisions
3. **Performance optimization** - RT-safety validation and latency reduction

---

**Ready for expert consultation on UX design, feature prioritization, and strategic product direction.**