#include "Determinism.h"

namespace SpectralCanvas {
namespace Determinism {

static std::atomic<bool> g_enabled{ false };
static std::atomic<uint32_t> g_seed{ 0x4D595DF4u }; // default stable seed

bool IsEnabled() { return g_enabled.load(std::memory_order_relaxed); }
void SetEnabled(bool enable) noexcept { g_enabled.store(enable, std::memory_order_release); }

uint32_t GetSeed() noexcept { return g_seed.load(std::memory_order_relaxed); }
void SetSeed(uint32_t seed) noexcept { g_seed.store(seed ? seed : 0x4D595DF4u, std::memory_order_release); }

} // namespace Determinism
} // namespace SpectralCanvas

