# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview
SpectralCanvas Pro is an advanced JUCE-based audio plugin for spectral synthesis and canvas-based sound design. Available as VST3 and Standalone applications, it provides visual manipulation of audio spectra through an interactive canvas interface.

### Repository Structure
- `Source/Core/` → Audio processing engine, DSP components, RT-safe systems
- `Source/GUI/` → User interface, editor components, visual feedback
- `Source/Spectral/` → STFT processing, spectral analysis components
- `Source/Tests/` → Unit tests integrated with main build
- `Tests/` → Standalone integration tests and validation
- `docs/` → Documentation, design specs, session notes
- `External/JUCE/` → JUCE framework (managed externally)
- `build/` → Generated build artifacts and binaries

### Core Architecture
- **Paint→Audio Pipeline**: Lock-free SPSC queue (`PaintQueue.h`) connects UI thread gestures to audio thread synthesis
- **RT-Safe Audio Processing**: `SpectralSynthEngine` performs synthesis without locks/allocations on audio thread
- **Parameter Management**: APVTS with atomic parameter snapshots for RT-safety
- **Emergency Systems**: Test tone injection and debugging infrastructure for development
- **Filter Chain**: EMU-style analog filter emulation with tube stage processing

## Commands

### Build System
```bash
# Standard build with tests (recommended)
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo --target ALL_BUILD

# Release build for distribution
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Debug build for development
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build --config Debug
```

### Testing
```bash
# Run all tests (use RelWithDebInfo for best debugging + performance)
ctest --test-dir build -C RelWithDebInfo --output-on-failure

# Run specific test
ctest --test-dir build -C RelWithDebInfo -R "TestPaintProducesAudio" --output-on-failure

# List available tests
ctest --test-dir build -N -C RelWithDebInfo

# Run RT-safety focused tests
ctest --test-dir build -C RelWithDebInfo -R "RT|Paint|Audio" --output-on-failure
```

### Development Workflow
```bash
# Emergency test mode toggle (in PluginEditor)
Press 'T' key → toggles emergency test tone for audio debugging

# TDD continuous testing
tdd-guard.bat

# Quick build validation
tools/test/phase7-quick-validation.bat
```

## File Boundaries

### Editable Files
- `Source/**` → All source code and headers
- `Tests/**` → Test files and validation scripts
- `docs/**` → Documentation and specifications
- `CMakeLists.txt` → Build configuration

### Protected Files
- `CLAUDE.md` → This workflow document (edit via unified diff only)
- `.claude/*.json` → Agent configuration and settings
- `.git/**` → Version control metadata

### Read-Only Files
- `External/JUCE/**` → JUCE framework (managed externally)
- `build/**` → Generated build artifacts
- `**/obj/**` → Compiler intermediates

## Workflow

### RECON → SPEC → PATCH → VERIFY Cycle
1. **RECON**: Delegate to reader-agent for file analysis and evidence gathering
2. **SPEC**: Main agent confirms targets and requirements with user
3. **PATCH**: Delegate to maker-agent for unified diffs and build commands
4. **VERIFY**: User applies changes and validates with build/test commands

### Agent Delegation
- **reader-agent** (haiku) → Fast recon, file searches, evidence gathering
- **maker-agent** (opus/sonnet) → Code generation, patches, build commands

### Conflict Resolution Priority
1. **User directives** (highest priority)
2. **CLAUDE.md** workflow rules
3. **settings.local.json** configuration
4. **Default behavior** (lowest priority)

## Permissions Strategy

### Default Mode
Plan Mode is default per `settings.local.json` configuration.

### Auto-Accept Commands
- `cmake -B build*` → Build configuration
- `cmake --build build*` → Compilation
- `ctest --test-dir build*` → Test execution
- `tdd-guard.bat` → Continuous testing

### Notification Wrappers (Auto-Accept)
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build-and-notify.ps1` → SpectralCanvas Pro build with notifications
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/test-and-notify.ps1` → RT-safety test execution with notifications
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/run-and-notify.ps1` → Generic task wrapper with logging and notifications
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/notify.ps1` → Desktop toast notifications (BurntToast/SnoreToast/console)

### Notification Integration
Environment variables for webhook integration:
- `SPECTRALCANVAS_SLACK_WEBHOOK` → Slack webhook URL for build/test notifications
- Logs stored in `./logs/` directory with timestamped files
- RT-Safety context automatically detected for audio thread operations
- Desktop notifications support BurntToast → SnoreToast → console fallback

### Require User Approval
- File edits (Source/, Tests/, docs/)
- Git operations (commit, push, merge)
- External dependency changes
- Configuration modifications

## Safety Rails

### Code Changes
- **Unified diffs only** for all patches
- **No direct file writes** without explicit approval
- **Minimal scoped changes** targeting specific issues
- **Build validation required** before considering changes complete

### Version Control
- **No commits without user approval**
- **Branch protection** for main/master branches
- **Rollback support** via git stash/branch mechanisms
- **Clean working directory** verification before major changes

## Best-of-N Pattern

For ambiguous requirements or implementation choices, present structured options:

### Option A: Conservative/Safe
- Minimal changes with maximum compatibility
- Established patterns and proven solutions
- Lower risk, incremental improvements

### Option B: Aggressive/Optimized
- More comprehensive changes for better performance
- Modern patterns and advanced techniques
- Higher impact but increased complexity

## Critical RT-Safety Rules

SpectralCanvas Pro is a real-time audio plugin with strict performance requirements:

### Audio Thread Constraints
- **Zero locks** in `processBlock()` chain - use atomic operations only
- **Zero allocations** in audio processing - preallocate in `prepareToPlay()`
- **Zero logging/printing** on audio thread - use lock-free trace buffers instead
- **SPSC queues only** - Single Producer (UI) Single Consumer (Audio) pattern
- **Atomic parameter access** - `std::memory_order_relaxed` for performance params

### Paint→Audio Pipeline Safety
```cpp
// UI Thread (Producer)
paintQueue.forcePush(gesture);  // Lock-free push

// Audio Thread (Consumer) 
while (paintQueue.pop(gesture)) {
    engine->setTargets(freq, amp);  // Atomic stores only
}
engine->processBlock(buffer);  // RT-safe synthesis
```

### Parameter Management
- **APVTS snapshots** taken once per block (RT-safe)
- **Atomic variables** for real-time modulation
- **Emergency fallbacks** for debugging (test tone injection)

## AI Collaboration Framework

### Integration Protocol v2.0 - Big-Prompt Playbook

SpectralCanvas Pro uses a simplified, evidence-based AI collaboration system focused on **big prompts with small patches**. This eliminates micro-management while ensuring quality and RT-safety.

### Core Principles
- **Truth = git + ctest** - No narrative without evidence
- **SPEC ↔ PATCH separation** - Planning and patching are different roles
- **Checksum gate** - Every patch directive carries SHA256 verification
- **RT-audio safety** - No locks/allocs/logging on audio thread; APVTS atomic

### Phase Model (Keep It Simple)
1. **RECON (Reader)**: Confirm file reality + test discovery; produce FROZEN evidence with checksum
2. **SPEC (SME/Advisor)**: One clear outcome, minimal surface area, constraints first  
3. **PATCH (Maker)**: Diff-only. No narration. Print build/test commands
4. **VERIFY (You)**: Build, run ctest, decide pass/fail and next step

### Sub-Agent Registry

| Agent | Purpose | When to Use | Output |
|-------|---------|-------------|---------|
| **reader-agent** | Evidence gathering, FROZEN blocks with SHA256 | Always - front-loads truth | File bytes + grep hits + checksum |
| **maker-agent** | Surgical diffs with checksum verification | Always - enforces small patches | ==DIFF== + ==BUILD== + ==TEST== |
| **analyzer-agent** | Architecture analysis, integration mapping | Complex features (DSP, threading) | YAML analysis with file:line evidence |
| **validator-agent** | Performance and regression validation | After patches requiring validation | Pass/fail metrics, performance deltas |
| **rt-linter** | RT-safety violation detection | Every audio path change | Lint report with line-by-line analysis |
| **dsp-oracle** | DSP math sanity checking | New DSP features, frequency mapping | Correctness report + golden test spec |
| **gui-robot** | UI interaction test generation | Canvas features, parameter changes | Test code for UI-to-audio validation |
| **preset-curator** | State management validation | Preset/state changes | Round-trip integrity tests |

### Big Prompt Templates

#### Universal AI Orchestrator (Copy-Paste Ready)
```
PLAN MODE ON. We will ship a RED→GREEN slice for <FEATURE> using big-prompt orchestration and small patches.

CONTEXT
- Truth = repo + ctest. Follow my TL;DR checksum and file boundaries strictly.
- RT-safety: no locks/allocs/logging on audio thread; APVTS atomic.

TASKS
READER → Return FROZEN block with SHA256, FILES, ASSERTIONS, DoD.
SPEC (you) → Propose the tiniest change that flips RED→GREEN; list exact files (≤3).
MAKER (after approval) → Echo SHA256, then unified diffs + exact build/test commands.

OUTPUT NOW
- RECON summary → SPEC → PATCH PLAN → Stop and await approval.
```

#### TL;DR Checksum Format
```
## TL;DR Checksum
TASK: <one-sentence outcome>
FILES (locked): <ordered list>
ENTITIES: <symbols/functions/ParamIDs>
CONSTRAINTS: RT-audio safety + scope limits
ASSERTIONS (test must prove): <bullet list>
DoD: <bullet list ending in exit code 0>
SHA256: <Reader-provided hex over FILES+ASSERTIONS+DoD>
```

### Quality Gates

#### Checksum Gate (Mandatory)
- Reader produces SHA256 over FILES+ASSERTIONS+DoD
- Architect copies exact hash into TL;DR
- Maker echoes hash before generating diffs
- Mismatch = immediate abort

#### RT-Safety Gate (Audio Changes)
- rt-linter scans every diff touching audio paths  
- Zero tolerance for locks/allocs/logging in processBlock
- Atomic parameter access patterns validated
- Performance regression monitoring

### Workflow Patterns

#### Simple Changes (≤3 files)
1. Use reader-agent for evidence
2. Create TL;DR with checksum  
3. Use maker-agent for surgical patch
4. Apply rt-linter if audio-related

#### Complex Features (>3 files, multiple systems)  
1. Use analyzer-agent for architecture preflight
2. Create comprehensive TL;DR with checksum
3. Use specialized agents (dsp-oracle, gui-robot) as needed
4. Full validation chain before integration

### Success Metrics
- **Evidence-Based**: All changes backed by reader-agent file evidence
- **Minimal Patches**: Surgical diffs targeting specific outcomes
- **Quality Gates**: RT-safety and performance preserved
- **Checksum Integrity**: Zero spec-drift between analysis and implementation

## Session Notes & Memory

### Session Tracking
Create session notes in `/docs/session-notes/YYYY-MM-DD.md` format:
```
# Session Notes - 2025-08-20

## Goals
- [Primary objectives for this session]

## Decisions Made
- [Key architectural or implementation decisions]

## Blockers Encountered
- [Issues that prevented progress]

## Next Steps
- [Action items for future sessions]
```

### Active Work Tracking
Link to `/docs/tasks/todo.md` for current sprint items and ongoing development tasks.
- Simple Loop v5.0 is mandatory: RECON → SPEC → PATCH (diff-only) → VERIFY (ctest)
- # RULE: Always start in Plan Mode (read-only). Present SPEC + PATCH; no Bash until I say "VERIFY"
- # RULE: Test-first gatekeeper. A slice is DONE only when `ctest` reports 100% pass from a clean build.