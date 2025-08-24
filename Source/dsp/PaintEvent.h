#pragma once
#include <cstdint>

// Minimal "paint" → synthesis event.
// Expand as your canvas/gesture system needs.
struct PaintEvent
{
    float baseHz;        // fundamental or dominant partial
    float amplitude;     // 0..1
    float pan;           // -1..+1
    uint16_t partials;   // number of harmonics
    uint32_t sampleOffset; // when to schedule inside current block
};