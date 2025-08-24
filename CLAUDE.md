CLAUDE.md

This file provides authoritative guidance to Claude Code when working with the SpectralCanvas Pro repository. All user prompts must operate within these boundaries.

Project Overview

SpectralCanvas Pro is a JUCE-based audio plugin and standalone application for spectral synthesis and canvas-driven sound design.

Phase 1 (MVP): Core Paint→Audio pipeline, real-time synthesis

Phase 2 (Current): UI upgrade per ui-phase2-scoped-spec.md ("Minimal but Musical")

Phase 3+: Advanced 3D features (future roadmap, not current scope)

Repository Structure

Source/Core/ → DSP engine, RT-safe systems

Source/GUI/ → Editor + Phase 2 UI components

Source/Spectral/ → STFT, HPSS, spectral analysis

Source/Tests/ → Unit tests integrated with main build

Tests/ → Standalone integration tests

docs/ → Specs, design notes (ui-phase2-scoped-spec.md)

External/JUCE/ → JUCE framework (external)

build/ → Build artifacts (read-only)

Core Architecture

Paint→Audio Pipeline: Lock-free SPSC queue from UI to audio thread

RT-Safe Audio: No locks/allocations on audio thread

Parameter Management: APVTS with atomic snapshots

Emergency Systems: Test tone injection

Analog Filter Chain: Tube + EMU-style filtering

Development Guidelines

Language & Style:

C++17, JUCE conventions

RAII and smart pointers; no raw new/delete

Thread-safe: no allocations/locks on audio thread

Doxygen-style headers for public classes/functions

Testing:

All DSP changes require unit + RT-safety tests (Source/Tests/)

UI behavior validated with integration tests (Tests/)

Docs:

Major features require update to /docs/

UI changes must sync with ui-phase2-scoped-spec.md

Commands
Build
# Build with tests
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo --target ALL_BUILD

Test
# Run all tests
ctest --test-dir build -C RelWithDebInfo --output-on-failure
# Run RT-safety subset
ctest --test-dir build -C RelWithDebInfo -R "RT|Paint|Audio" --output-on-failure

Dev Workflow
# Emergency test tone toggle (Editor: press 'T')
# Continuous TDD
tdd-guard.bat
# Quick build validation
tools/test/phase7-quick-validation.bat

File Boundaries

Editable: Source/**, Tests/**, docs/**, CMakeLists.txt

Protected: CLAUDE.md, .claude/*.json, .git/**

Read-Only: External/JUCE/**, build/**, **/obj/**

Workflow: RECON → SPEC → PATCH → VERIFY

RECON: Reader-agent gathers evidence

SPEC: Main agent confirms requirements

PATCH: Maker-agent applies unified diffs

VERIFY: User builds/tests to confirm

Agent Delegation

reader-agent (haiku): file reads, searches, evidence gathering

maker-agent (opus/sonnet): code generation, patches, build commands

Always parallelize when safe (multi-agent strategy)

Permissions & Safety Modes

Default: Plan Mode ON (Shift+Tab twice) for analysis before edits

Auto-Accept: Allowed for Edit(*) and Grep(*) during Phase 2 UI iteration

Forbidden: --dangerously-skip-permissions (never use)

Conflict Resolution

User directives (highest priority)

CLAUDE.md rules

Default safe practices

Phase 2 UI Rules (Scoped Spec)

When editing Source/GUI/**, changes must follow docs/ui-phase2-scoped-spec.md:

Canvas: Flat 2D spectrogram, additive glow strokes, cyan/orange HPSS system

Overlays: Note grid, piano roll, scale highlighting, ghost snapping

Feedback: Overtone guides, chord recognition

Export Pod: Real-time waveform preview + drag-to-DAW export

Notifications & Hooks

Mobile Notifications (Pushover):
powershell.exe -ExecutionPolicy Bypass -File .claude/hooks/pushover-notifier.ps1 "Your message"

Setup Pushover:
1. Install Pushover app on phone
2. Get credentials from https://pushover.net/
3. Set environment variables:
   - PUSHOVER_TOKEN = your app token
   - PUSHOVER_USER = your user key
   
OR create config file: C:\Users\%USERNAME%\.claude\pushover.json
{
  "token": "your_app_token", 
  "user": "your_user_key"
}

Auto-notify after:
- Successful builds (cmake --build complete)
- Test runs (ctest complete) 
- Git commits/pushes
- Long refactoring sessions (>10 files changed)

Usage Examples:
powershell.exe -ExecutionPolicy Bypass -File .claude/hooks/pushover-notifier.ps1 "✅ Build completed successfully"
powershell.exe -ExecutionPolicy Bypass -File .claude/hooks/pushover-notifier.ps1 "🧪 All tests passing"
powershell.exe -ExecutionPolicy Bypass -File .claude/hooks/pushover-notifier.ps1 "🚀 Phase 2 UI updates pushed"

No deviation from spec without explicit user approval.