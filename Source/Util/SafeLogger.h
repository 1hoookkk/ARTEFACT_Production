// Source/Util/SafeLogger.h
// RT-safe lock-free audio event logging - SafeLogger replacement for AudioTrace
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>
#include <cstring>

namespace SpectralCanvas {

//==============================================================================
// RT-safe POD event types (same as AudioTrace for compatibility)
enum class AudioEventType : uint8_t
{
    PaintGesture = 0,       // Paint gesture received on audio thread
    EmergencyMode = 1,      // Emergency test tone toggled
    FilterUpdate = 2,       // Filter parameters changed
    AudioBlock = 3,         // processBlock() executed
    SilenceDetected = 4,    // Watchdog detected silence
    WatchdogFallback = 5,   // Watchdog triggered emergency mode
    MaxEvents = 6
};

//==============================================================================
// POD trace event for lock-free communication (same as AudioTrace)
struct alignas(32) AudioTraceEvent
{
    AudioEventType type;
    uint64_t timestamp;     // High-resolution ticks
    float param1;          // Context-dependent parameter
    float param2;          // Context-dependent parameter
    float param3;          // Context-dependent parameter
    
    AudioTraceEvent(AudioEventType t = AudioEventType::AudioBlock, 
                   float p1 = 0.0f, float p2 = 0.0f, float p3 = 0.0f) noexcept
        : type(t), timestamp(juce::Time::getHighResolutionTicks())
        , param1(p1), param2(p2), param3(p3) {}
};

//==============================================================================
// RT-safe SPSC circular buffer for trace events
template<size_t Size>
class AudioTraceBuffer
{
public:
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");
    
    AudioTraceBuffer() noexcept : head(0), tail(0), dropCount(0) {}
    
    // RT-safe push (audio thread producer)
    bool push(const AudioTraceEvent& event) noexcept
    {
        const auto currentHead = head.load(std::memory_order_relaxed);
        const auto nextHead = (currentHead + 1) & (Size - 1);
        
        if (nextHead == tail.load(std::memory_order_acquire))
        {
            dropCount.fetch_add(1, std::memory_order_relaxed);
            return false; // Buffer full
        }
        
        buffer[currentHead] = event;
        head.store(nextHead, std::memory_order_release);
        return true;
    }
    
    // RT-safe pop (test thread consumer)
    bool pop(AudioTraceEvent& event) noexcept
    {
        const auto currentTail = tail.load(std::memory_order_relaxed);
        if (currentTail == head.load(std::memory_order_acquire))
            return false; // Buffer empty
            
        event = buffer[currentTail];
        tail.store((currentTail + 1) & (Size - 1), std::memory_order_release);
        return true;
    }
    
    // Query functions (RT-safe)
    size_t size() const noexcept
    {
        const auto h = head.load(std::memory_order_acquire);
        const auto t = tail.load(std::memory_order_acquire);
        return (h - t) & (Size - 1);
    }
    
    uint32_t getDropCount() const noexcept
    {
        return dropCount.load(std::memory_order_relaxed);
    }
    
    void resetDropCount() noexcept
    {
        dropCount.store(0, std::memory_order_relaxed);
    }

private:
    alignas(64) std::atomic<size_t> head;
    alignas(64) std::atomic<size_t> tail;
    alignas(64) std::atomic<uint32_t> dropCount;
    alignas(64) std::array<AudioTraceEvent, Size> buffer;
};

//==============================================================================
// Global trace buffer for tests (extern declaration)
extern AudioTraceBuffer<4096> g_safeLoggerBuffer;

// Also provide g_traceBuffer symbol for compatibility
extern AudioTraceBuffer<4096> g_traceBuffer;

//==============================================================================
// RT-safe logging functions (same API as AudioTrace)
inline void logAudioEvent(AudioEventType type, 
                          float param1 = 0.0f, 
                          float param2 = 0.0f, 
                          float param3 = 0.0f) noexcept
{
    AudioTraceEvent event(type, param1, param2, param3);
    g_safeLoggerBuffer.push(event);
}

//==============================================================================
// Test consumer functions
inline bool readAudioEvent(AudioTraceEvent& event) noexcept
{
    return g_safeLoggerBuffer.pop(event);
}

inline size_t getAudioEventCount() noexcept
{
    return g_safeLoggerBuffer.size();
}

inline uint32_t getDroppedEventCount() noexcept
{
    return g_safeLoggerBuffer.getDropCount();
}

inline void resetDroppedEventCount() noexcept
{
    g_safeLoggerBuffer.resetDropCount();
}

} // namespace SpectralCanvas