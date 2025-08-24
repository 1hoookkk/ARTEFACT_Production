#pragma once

#include <atomic>
#include <cstdint>

namespace SpectralCanvas {
namespace Determinism {

// Global deterministic render switch and seed (RT-safe atomics)
bool IsEnabled();
void SetEnabled(bool enable) noexcept;

uint32_t GetSeed() noexcept;
void SetSeed(uint32_t seed) noexcept;

// Lightweight LCG for deterministic noise when needed
struct Lcg32 {
    uint32_t state;
    explicit Lcg32(uint32_t s) : state(s ? s : 0xA341316Cu) {}
    inline uint32_t nextU32() noexcept {
        // xorshift* variant for speed and reproducibility
        uint32_t x = state;
        x ^= x << 13; x ^= x >> 17; x ^= x << 5;
        state = x ? x : 0xA341316Cu;
        return state;
    }
    inline float nextFloat01() noexcept {
        return (nextU32() >> 8) * (1.0f / 16777216.0f); // 24-bit mantissa to [0,1)
    }
};

} // namespace Determinism
} // namespace SpectralCanvas

