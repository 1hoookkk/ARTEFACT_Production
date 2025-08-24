# RT-Safety Requirements

Hard constraints for real-time audio processing in SpectralCanvas Pro.

## Non-Negotiables

- Zero locks on audio thread (`processBlock()` chain).
- Zero heap allocations on audio thread; preallocate all buffers.
- Zero logging/printing on audio thread; use lock-free trace buffers if needed.
- SPSC queues only for UI→Audio: `Source/Core/PaintQueue.h`.
- Parameter reads use APVTS atomics; snapshot once per block.

## Patterns

- Preallocate in `prepareToPlay()`, reuse in `processBlock()`.
- Use `std::atomic` with `memory_order_relaxed` for RT parameters.
- Double-buffer visual/spectral data shared with the UI.

## Example

```cpp
// UI Thread (Producer)
paintQueue.forcePush(gesture);  // Lock-free push

// Audio Thread (Consumer)
PaintGesture g{};
while (paintQueue.pop(g)) {
    engine.setTargets(g); // atomic stores only
}
engine.processBlock(buffer, midi);
```

## Checks

- Run RT-focused tests via `ctest -R "RT|Paint|Audio|processBlock|ThreadSafety"`.
- Grep for hazards before merges (`new`, `malloc`, `std::lock_guard`, `DBG`, `printf` in audio path).

