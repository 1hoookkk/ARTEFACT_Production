#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>

// Single-producer / single-consumer lock-free ring buffer.
// Power-of-two capacity for cheap masking.
template <typename T, size_t CapacityPow2>
class SpscRing
{
    static_assert((CapacityPow2 & (CapacityPow2 - 1)) == 0, "Capacity must be power of two");
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable for lock-free SPSC");

public:
    SpscRing() : head(0), tail(0) {}

    // Producer thread (UI/Message thread)
    bool push(const T& v)
    {
        const auto h = head.load(std::memory_order_relaxed);
        const auto t = tail.load(std::memory_order_acquire);
        if (((h + 1) & mask()) == (t & mask()))
            return false; // full
        buffer[h & mask()] = v;
        head.store(h + 1, std::memory_order_release);
        return true;
    }

    // Consumer thread (audio thread)
    bool pop(T& out)
    {
        const auto t = tail.load(std::memory_order_relaxed);
        const auto h = head.load(std::memory_order_acquire);
        if ((t & mask()) == (h & mask()))
            return false; // empty
        out = buffer[t & mask()];
        tail.store(t + 1, std::memory_order_release);
        return true;
    }

    size_t size() const
    {
        const auto h = head.load(std::memory_order_acquire);
        const auto t = tail.load(std::memory_order_acquire);
        return (h - t) & ((size_t{1} << kBits) - 1);
    }

    size_t freeSpace() const { return capacity() - size() - 1; }
    static constexpr size_t capacity() { return CapacityPow2; }

private:
    static constexpr size_t kBits = [] {
        size_t n = 0, c = CapacityPow2;
        while (c >>= 1) ++n;
        return n;
    }();

    static constexpr size_t mask() { return CapacityPow2 - 1; }

    alignas(64) std::atomic<size_t> head;
    alignas(64) std::atomic<size_t> tail;
    alignas(64) T buffer[CapacityPow2];
};