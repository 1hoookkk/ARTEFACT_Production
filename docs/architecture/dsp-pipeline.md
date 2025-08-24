# DSP Pipeline — Paint → Audio Flow

This page summarizes the end-to-end flow from UI gestures to audio output, referencing the concrete implementation files in this repository.

## Overview

- UI captures gestures and publishes them via a lock-free SPSC queue.
- Audio thread consumes gestures, updates targets atomically, and renders audio.
- Post-processing applies analog-style filtering and tube stage coloration.

## Components

- Producer (UI Thread): `Source/Core/PaintQueue.h`, `Source/GUI/*Canvas*`
- Consumer (Audio Thread): `Source/Core/SpectralSynthEngine.(h|cpp)`
- Spectral/Masking: `Source/Core/SpectralMask.*`, `Source/Core/SampleMaskingEngine.*`
- Analog Chain: `Source/Core/EMUFilter.*`, `Source/Core/TubeStage.*`
- Utilities: `Source/Core/ColorToSpectralMapper.*`, `Source/Core/OptimizedOscillatorPool.h`

## Data Flow

1. UI creates `PaintGesture` events and pushes to `PaintQueue`.
2. Audio thread pops gestures, updates engine targets atomically.
3. `SpectralSynthEngine::processBlock` synthesizes audio using preallocated buffers.
4. Optional post-processing: EMU filter → Tube stage → Width/Tape.

## Real-Time Rules

- No locks/allocations/logging on the audio thread.
- Preallocate in `prepareToPlay()` and reuse in `processBlock()`.
- Access parameters via APVTS atomics.

See also: `docs/architecture/rt-safety.md`.

