# 🔄 SpectralCanvas Pro - Multi-AI Workflow Guide

*The complete guide to seamless AI-to-AI collaboration for SpectralCanvas Pro development*

---

## 🎯 Overview

This workflow system enables seamless handoffs between multiple AI systems in your development pipeline:

**ChatGPT JUCE VST3** → **Gemini** → **Claude Code GPT** → **Claude Code** → **Repeat**

Each AI has specialized roles and templates, with automatic state management and validation.

---

## 🚀 Quick Start

### For Immediate Handoff
1. **Export current state**: `handoff-export.bat`
2. **Copy QUICK CONTEXT** to next AI
3. **Use appropriate template** from `workflow/templates/`
4. **Import on receiving end**: `handoff-import.bat`

### For Safe Checkpointing
1. **Create checkpoint**: `checkpoint.bat "Description of changes"`
2. **Continue development** or handoff

### For Status Updates
1. **Update documentation**: `update-snapshot.bat current-state "Your update"`
2. **Validate system**: `validate-ready.bat`

---

## 🛠️ Tool Reference

### Core Scripts

| Script | Purpose | Usage |
|--------|---------|-------|
| `handoff-export.bat` | Creates complete handoff package | Run before handing off to next AI |
| `handoff-import.bat` | Validates environment and shows context | Run when receiving handoff |
| `checkpoint.bat "msg"` | Creates safe restore point with git tag | Use at major milestones |
| `validate-ready.bat` | Pre-flight check for handoff readiness | Run before any handoff |
| `update-snapshot.bat` | Updates PROJECT_SNAPSHOT.md automatically | Keep documentation current |

### AI Templates

| Template | AI Role | Purpose |
|----------|---------|---------|
| `chatgpt-concepts.md` | Creative Conceptualist | Generate inspiring feature ideas |
| `gemini-architecture.md` | Technical Architect | Review feasibility and create implementation plans |
| `claude-gpt-processing.md` | Requirements Processor | Transform plans into TDD specifications |
| `claude-code-implementation.md` | TDD Executor | Implement using RED→GREEN→REFACTOR |

---

## 🔄 Workflow Stages

### Stage 1: Creative Concepts (ChatGPT JUCE VST3)
**Input**: Project vision + current capabilities
**Output**: Musical feature concepts

```bash
# Before starting:
handoff-import.bat  # Get current context

# Use template:
workflow/templates/chatgpt-concepts.md

# Example concepts:
- Harmonic brush: Emphasizes harmonic frequencies
- Noise brush: Creates inharmonic spectral content  
- Morphing brush: Smooth transitions between timbres
```

**Handoff Format**:
```markdown
## CONCEPTS READY FOR ARCHITECTURE REVIEW
**Source**: ChatGPT Creative Concepts
**Concepts**: 3 unique spectral painting behaviors
**Next**: Technical feasibility analysis
```

### Stage 2: Technical Architecture (Gemini)
**Input**: Creative concepts + technical constraints
**Output**: RT-safe implementation plan

```bash
# Use template:
workflow/templates/gemini-architecture.md

# Focus areas:
- RT-safety analysis (no allocations/locks in audio thread)
- Performance estimates (CPU/memory impact)
- JUCE integration approach
- Risk assessment
```

**Handoff Format**:
```markdown
## ARCHITECTURE APPROVED FOR PROCESSING
**Source**: Gemini Technical Architecture
**Status**: READY
**Complexity**: 3/5
**Files**: SpectralSynthEngine.h/cpp, NewFeature.h/cpp
```

### Stage 3: Requirements Processing (Claude Code GPT)
**Input**: Architecture plan + TDD methodology
**Output**: Precise test specifications

```bash
# Use template:
workflow/templates/claude-gpt-processing.md

# Deliverables:
- Step 0: Files to read for reconnaissance
- Step 1: Exact RED test specifications
- Step 2: Minimal GREEN implementation plan
- Step 3: REFACTOR optimization targets
```

**Handoff Format**:
```markdown
## REQUIREMENTS READY FOR IMPLEMENTATION
**Source**: Claude Code GPT Requirements Processing
**Test Files**: TestNewFeature.cpp
**Implementation Files**: NewFeature.h/cpp, SpectralSynthEngine.h/cpp
**Parameters**: 3 new APVTS parameters
```

### Stage 4: TDD Implementation (Claude Code)
**Input**: Complete test specifications
**Output**: Working, tested feature

```bash
# Use template:
workflow/templates/claude-code-implementation.md

# TDD Process (absolute):
1. Step 0: Reconnaissance (read all specified files)
2. Step 1: RED (write failing test)
3. Step 2: GREEN (minimal implementation)
4. Step 3: REFACTOR (optimize while keeping tests GREEN)

# Verification:
scripts/build-tests.bat
scripts/run-tests.bat
```

**Completion Actions**:
```bash
# Create checkpoint
checkpoint.bat "Feature implementation complete"

# Update documentation
update-snapshot.bat current-state "New feature fully implemented and tested"

# Create handoff for next cycle
handoff-export.bat
```

---

## 🎯 AI Role Definitions

### 🎨 ChatGPT JUCE VST3 - Creative Conceptualist
**Strengths**: Musical inspiration, user experience, creative features
**Focus**: "What would inspire musicians?"
**Constraints**: Must respect RT-audio requirements and paint-based interaction
**Output**: 3-5 feature concepts with clear musical use cases

### 🏗️ Gemini - Technical Architect  
**Strengths**: System design, performance analysis, risk assessment
**Focus**: "How can this be built safely and efficiently?"
**Constraints**: RT-safety, JUCE patterns, existing architecture
**Output**: Detailed implementation plan with risk mitigation

### 📋 Claude Code GPT - Requirements Processor
**Strengths**: TDD methodology, specification precision, test design
**Focus**: "What exactly needs to be tested and implemented?"
**Constraints**: CLAUDE.md constitution, existing test patterns
**Output**: Complete TDD cycle specifications

### ⚡ Claude Code - TDD Executor
**Strengths**: Implementation, debugging, system integration
**Focus**: "Make it work, make it right, make it fast"
**Constraints**: RED→GREEN→REFACTOR cycle, RT-safety absolute
**Output**: Working, tested, integrated feature

---

## 🧪 Quality Gates

### Pre-Handoff Validation
Before passing to next AI:

```bash
validate-ready.bat
```

**Must Pass**:
- ✅ Core files present (PROJECT_SNAPSHOT.md, CLAUDE.md)
- ✅ Git repository healthy
- ✅ Build system operational
- ✅ Test infrastructure working

### Post-Implementation Validation
After feature completion:

```bash
scripts/build-tests.bat  # Must succeed
scripts/run-tests.bat    # All tests must pass
```

**Quality Criteria**:
- ✅ All existing tests still pass
- ✅ New feature has test coverage
- ✅ RT-safety maintained
- ✅ Integration complete

### Checkpoint Validation
Before major milestones:

```bash
checkpoint.bat "Major milestone description"
```

**Checkpoint Creates**:
- 📌 Git tag for easy restoration
- 📄 Metadata file with system status
- 💾 Safe restore point

---

## 🔧 Advanced Usage

### Emergency Recovery
If workflow breaks:

```bash
# Check last good checkpoint
git tag | sort

# Restore to known good state
git checkout checkpoint-YYYY-MM-DD_HH-MM-SS

# Rebuild and validate
scripts/build-tests.bat
scripts/run-tests.bat
```

### Multi-Branch Development
For parallel feature development:

```bash
# Create feature branch from checkpoint
git checkout -b feature-spectral-brushes checkpoint-2025-08-17_14-30-00

# Work on feature
# ... development cycle ...

# Create feature checkpoint
checkpoint.bat "Spectral brushes implementation complete"

# Merge when ready
git checkout main
git merge feature-spectral-brushes
```

### Custom Templates
To create project-specific templates:

```bash
# Copy existing template
copy workflow\templates\claude-code-implementation.md workflow\templates\custom-template.md

# Modify for specific needs
# Update handoff scripts to reference new template
```

---

## 🎛️ Configuration

### Script Customization
All scripts can be customized by editing:
- Test commands in `validate-ready.bat`
- Template references in `handoff-export.bat`
- Checkpoint metadata in `checkpoint.bat`

### Template Customization
Templates in `workflow/templates/` can be modified for:
- Project-specific constraints
- Additional AI roles
- Custom workflow stages

### Integration with External Tools
Scripts can be extended to integrate with:
- CI/CD systems (add webhook calls)
- Project management (update task status)
- Communication tools (send notifications)

---

## 🎯 Success Patterns

### Optimal Handoff Sequence
1. **Validate before handoff**: `validate-ready.bat`
2. **Export clean state**: `handoff-export.bat`
3. **Use role-specific template**
4. **Import and validate**: `handoff-import.bat`
5. **Complete assigned role**
6. **Checkpoint major changes**: `checkpoint.bat "description"`

### Preventing Context Loss
- Always reference PROJECT_SNAPSHOT.md for complete context
- Use QUICK CONTEXT section for immediate understanding
- Include file paths and line numbers in specifications
- Checkpoint before and after major changes

### Quality Assurance
- Run tests before and after every handoff
- Use validate-ready.bat before complex operations
- Create checkpoints at stable states
- Update PROJECT_SNAPSHOT.md regularly

---

## ⚠️ Common Pitfalls

### ❌ Context Drift
**Problem**: AI forgets project context
**Solution**: Always include QUICK CONTEXT from handoff package

### ❌ Test Regression  
**Problem**: New features break existing functionality
**Solution**: Run full test suite before every handoff

### ❌ Architecture Violations
**Problem**: Changes violate RT-safety or CLAUDE.md laws
**Solution**: Use templates that enforce constraints

### ❌ Integration Conflicts
**Problem**: Multiple AIs modify same files
**Solution**: Use checkpoint system and clear file ownership

---

## 📚 Reference Links

- **Project Constitution**: `CLAUDE.md` (TDD laws and methodology)
- **Complete Context**: `PROJECT_SNAPSHOT.md` (full project state)
- **Test Patterns**: `Source/Tests/` (working examples)
- **Build System**: `CMakeLists.txt` + `scripts/`

---

## 🎨 Philosophy

> "Each AI is a specialist craftsperson in a master workshop. The workflow system is our shared language, ensuring that the baton passes cleanly from vision to architecture to implementation to polish. Together, we build something greater than any single intelligence could achieve alone."

**Core Principles**:
- 🎯 **Clear Roles**: Each AI has a defined specialty
- 🔄 **Clean Handoffs**: No context loss between stages  
- 🧪 **Quality Gates**: Validation at every transition
- 📋 **Documentation**: Living record of all decisions
- 🛡️ **Safety First**: RT-audio performance never compromised

---

*🎵 "Draw, and the canvas sings." - SpectralCanvas Pro Team*

**Generated with Multi-AI Workflow System**  
*Optimized for seamless collaboration between human creativity and AI precision*