# SpectralCanvas Pro Brownfield Recovery Blueprint

## 🚨 **Critical Situation Assessment**

**Date**: 2025-08-21  
**Status**: CRITICAL - RT-safety infrastructure missing after 2025-08-16 minimalization purge  
**Impact**: ~173 critical files archived, leaving only ~49 active files  

### **Immediate Risks**
- ❌ **RT-Safety Compromised**: PaintQueue, RealtimeMemoryManager, safety assertions missing
- ⚠️ **Professional Appearance Lost**: EMU LookAndFeel components archived
- 🎯 **Core Pipeline Broken**: UI→Audio paint-to-synthesis pathway interrupted
- 🔧 **Testing Infrastructure**: ThreadSafetyTests and validation framework missing

## 📊 **Archive Inventory Analysis**

### **Critical Files for Immediate Restoration**

| Priority | Archive Source | Target Destination | Purpose |
|----------|---------------|-------------------|---------|
| 🔴 P1 | `_archive/ghost_code_cleanup_20250816_204913/Source/rt/PaintQueue.h` | `Source/Core/PaintQueue.h` | Lock-free SPSC UI→Audio queue |
| 🔴 P1 | `_archive/.../Source/Core/RealtimeMemoryManager.*` | `Source/Core/RealtimeMemoryManager.*` | RT-safe memory allocation |
| 🔴 P1 | `_archive/.../Source/Core/RealtimeSafeAssertions.h` | `Source/Core/RealtimeSafeAssertions.h` | Audio-thread safe debugging |
| 🔴 P1 | `_archive/.../Source/Core/ThreadSafetyTests.cpp` | `Source/Tests/ThreadSafetyTests.cpp` | RT-safety validation |
| 🟡 P2 | `_archive/.../Source/UI/EMUDesktopLookAndFeel.*` | `Source/GUI/Skin/EMUDesktopLookAndFeel.*` | Professional UI appearance |

### **Sub-Agent Assignment Strategy**

| Agent | Responsibility | Files |
|-------|---------------|--------|
| 🔴 **rt-audio-guardian** | RT-safety validation, PaintQueue analysis | PaintQueue.h, RealtimeMemoryManager, ThreadSafetyTests |
| 🟠 **build-stability-monitor** | Archive inventory, build system validation | CMake integration, header dependencies |
| 🟢 **juce-integration-specialist** | UI→Audio pipeline rewiring, APVTS safety | Pipeline reconnection, parameter handling |
| 🔵 **dsp-frequency-oracle** | Audio processing integrity | LookAndFeel audio impact validation |

## 🏗️ **Phase-Based Recovery Plan**

### **Phase 1: Critical Infrastructure** (This Session)
**Goal**: Restore RT-safe paint-to-audio pipeline + minimal professional UI

**Sub-Agent Workflow**:
1. **build-stability-monitor** → Verify archive inventory and dependencies
2. **rt-audio-guardian** → Validate restored RT-safety components
3. **juce-integration-specialist** → Rewire UI→Audio pipeline safely
4. **dsp-frequency-oracle** → Ensure audio processing integrity maintained

**Success Criteria**:
- ✅ PaintQueue functional with zero RT violations
- ✅ Professional LookAndFeel selectable without crashes
- ✅ ThreadSafetyTests passing in RelWithDebInfo build
- ✅ pluginval strict validation passes

### **Phase 2: Professional Features** (Next Session)
**Target Files**:
- EMUFilter + EMUModulation DSP components
- Asset pipeline and sprite management systems
- InspectorPanels UI framework
- RoomManager architecture

### **Phase 3: Advanced Systems** (Future)
**Target Features**:
- Hardware controller integration
- AI assistant collaboration features
- Performance optimizations (SIMD, vectorization)
- Advanced spectral processing capabilities

## 🛡️ **Safety Protocols & Validation Gates**

### **Archive Restore Protocol**
1. **Never delete existing files** - only copy/overlay from archive
2. **Show unified diffs** for every file modification
3. **Validate existence** in archive before attempting restore
4. **Check dependencies** before copying headers
5. **Build validation** after each critical restoration

### **RT-Safety Validation Requirements** (rt-audio-guardian enforced)
- ✅ Zero heap allocations in processBlock() chain
- ✅ Zero locks/mutexes in audio thread
- ✅ Zero logging/cout/DBG in audio processing  
- ✅ SPSC queue pattern for UI→Audio communication
- ✅ Atomic parameter access with proper memory ordering

### **Editor Constructor Safety** (juce-integration-specialist enforced)
- ✅ No heavy I/O operations in constructor
- ✅ No timer starts or thread creation in constructor
- ✅ Defer initialization to editorShown() or Timer callback
- ✅ LookAndFeel application in editorShown(), removal in destructor

### **Build System Integrity** (build-stability-monitor enforced)
- ✅ CMake configuration succeeds (RelWithDebInfo + BUILD_TESTS=ON)
- ✅ Full compilation without warnings
- ✅ No header pollution or circular dependencies
- ✅ Cross-platform compatibility maintained

## 🚦 **Quality Gates & Checkpoints**

### **Gate 1: Archive Validation**
**Responsible Agent**: build-stability-monitor
- Archive file accessibility confirmed
- Dependency analysis complete
- Merge conflict prediction complete

### **Gate 2: RT-Safety Restoration**
**Responsible Agent**: rt-audio-guardian  
- PaintQueue.h restored and validated
- RealtimeMemoryManager integrated safely
- ThreadSafetyTests passing
- Zero RT violations detected

### **Gate 3: Pipeline Integration**
**Responsible Agent**: juce-integration-specialist
- UI→Audio pipeline reconnected
- APVTS parameter handling validated
- Component lifecycle safety verified
- No constructor side effects

### **Gate 4: System Validation**
**Responsible Agents**: All agents coordinated
- Full build system validation
- pluginval strict passes
- RT-safety tests all green
- Professional appearance functional

## 🔄 **Rollback & Risk Mitigation**

### **Rollback Triggers**
- RT-safety violations detected by rt-audio-guardian
- Build system breaks identified by build-stability-monitor  
- UI component crashes detected by juce-integration-specialist
- Audio processing degradation flagged by dsp-frequency-oracle

### **Risk Mitigation Strategies**
- **API Drift**: Create minimal shim headers if needed
- **Include Conflicts**: Use namespace aliases or adapter patterns
- **Compilation Failures**: Incremental restoration with git checkpoints
- **RT-Safety Violations**: Immediate revert if violations detected

### **Emergency Procedures**
1. **Immediate Revert**: Git restore to last known good state
2. **Isolated Testing**: Test restored components in isolation
3. **Gradual Integration**: Restore components one at a time
4. **Agent Consultation**: Use rt-audio-guardian for safety validation

## 📈 **Success Metrics & Monitoring**

### **Technical Metrics**
- **RT-Safety Score**: 100% compliance (zero violations)
- **Build Health**: Clean compilation across all configurations
- **Test Coverage**: All RT-safety tests passing
- **Performance**: <5ms paint-to-audio latency maintained

### **Agent Effectiveness Metrics**
- **rt-audio-guardian**: Zero false positives, all RT violations caught
- **build-stability-monitor**: Build prediction accuracy >95%
- **juce-integration-specialist**: Component lifecycle issues <1%
- **dsp-frequency-oracle**: Audio processing integrity maintained

## 📝 **Implementation Commands & Agent Usage**

### **Phase 1 Execution Sequence**
```bash
# 1. Archive inventory with build-stability-monitor
*agent build-stability-monitor
*task analyze-archive-inventory

# 2. RT-safety component restoration with rt-audio-guardian
*agent rt-audio-guardian  
*task validate-paintqueue-restoration

# 3. UI pipeline rewiring with juce-integration-specialist
*agent juce-integration-specialist
*task audit-ui-audio-pipeline

# 4. System validation with coordinated agents
*workflow rt-safety-audit
```

### **Validation Commands**
```bash
# Build and test validation
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo
ctest --test-dir build -C RelWithDebInfo --output-on-failure

# RT-safety validation
*agent rt-audio-guardian
*task comprehensive-rt-audit

# pluginval validation
pluginval --strictness-level 10 build/SpectralCanvas_artefacts/RelWithDebInfo/VST3/SpectralCanvas.vst3
```

## 📋 **Session Documentation Requirements**

### **Session Notes Template** (`docs/session-notes/2025-08-21-brownfield-init.md`)
- Goals and success criteria
- Agent assignments and responsibilities  
- Commands executed and results
- Issues encountered and resolutions
- Next steps and handoff notes

### **CLAUDE.md Updates Required**
- Archive Restore Protocol addition
- RT-Safety Rules reinforcement  
- Editor Constructor Safety guidelines
- Sub-agent integration workflows

---

**This blueprint provides the strategic framework for systematic restoration of SpectralCanvas Pro's critical infrastructure using specialized sub-agent expertise to ensure quality, safety, and professional standards throughout the recovery process.**