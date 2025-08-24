// Source/Dev/AudioTrace.h
// RT-safe lock-free audio event tracing for SpectralCanvas Pro restoration
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>
#include <cstring>

namespace SpectralCanvas {

//==============================================================================
// RT-safe POD event types for audio thread tracing
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
// POD trace event for lock-free communication
struct alignas(32) AudioTraceEvent
{
    AudioEventType type;
    uint64_t timestamp;     // High-resolution ticks
    float param1 = 0.0f;    // Event-specific data
    float param2 = 0.0f;
    float param3 = 0.0f;
    uint32_t frameCounter = 0;
    uint32_t sequenceId = 0;
    
    // POD constructor
    AudioTraceEvent(AudioEventType t = AudioEventType::AudioBlock, 
                   float p1 = 0.0f, float p2 = 0.0f, float p3 = 0.0f) noexcept
        : type(t), timestamp(juce::Time::getHighResolutionTicks()), 
          param1(p1), param2(p2), param3(p3) {}
};

//==============================================================================
// Lock-free SPSC ring buffer for RT-safe event logging
// Single Producer (audio thread) Single Consumer (UI thread)
template<size_t Size = 4096>
class AudioTraceBuffer
{
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");

public:
    AudioTraceBuffer() noexcept : head(0), tail(0), dropCount(0) {}
    
    // RT-safe push (audio thread ONLY) - never blocks
    bool push(const AudioTraceEvent& event) noexcept
    {
        const auto current_tail = tail.load(std::memory_order_relaxed);
        const auto next_tail = increment(current_tail);
        
        // Check for space - if full, drop oldest event
        if (next_tail != head.load(std::memory_order_acquire))
        {
            buffer[current_tail] = event;
            tail.store(next_tail, std::memory_order_release);
            return true;
        }
        
        // Buffer full - drop event and increment counter
        dropCount.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    
    // Thread-safe pop (consumer thread ONLY)
    bool pop(AudioTraceEvent& event) noexcept
    {
        const auto current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire))
            return false; // Buffer empty
            
        event = buffer[current_head];
        head.store(increment(current_head), std::memory_order_release);
        return true;
    }
    
    // Statistics (non-RT thread)
    size_t size() const noexcept
    {
        const auto h = head.load(std::memory_order_acquire);
        const auto t = tail.load(std::memory_order_acquire);
        return (t >= h) ? (t - h) : (Size - h + t);
    }
    
    uint32_t getDropCount() const noexcept
    {
        return dropCount.load(std::memory_order_acquire);
    }
    
    void resetDropCount() noexcept
    {
        dropCount.store(0, std::memory_order_release);
    }
    
private:
    alignas(64) std::array<AudioTraceEvent, Size> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    std::atomic<uint32_t> dropCount;  // Count of dropped events
    
    constexpr size_t increment(size_t idx) const noexcept 
    { 
        return (idx + 1) & (Size - 1); 
    }
};

//==============================================================================
// Global trace instance for SpectralCanvas Pro
extern AudioTraceBuffer<4096> g_traceBuffer;

// RT-safe trace logging (audio thread ONLY)
inline void logAudioEvent(AudioEventType type, 
                         float p1 = 0.0f, float p2 = 0.0f, float p3 = 0.0f) noexcept
{
    AudioTraceEvent event(type, p1, p2, p3);
    g_traceBuffer.push(event);
}

// Specialized loggers for common events
inline void logPaintGesture(float x, float y, float pressure) noexcept
{
    logAudioEvent(AudioEventType::PaintGesture, x, y, pressure);
}

inline void logEmergencyMode(bool enabled, float amplitude = 0.0f) noexcept
{
    logAudioEvent(AudioEventType::EmergencyMode, enabled ? 1.0f : 0.0f, amplitude, 0.0f);
}

inline void logSilenceDetected(float rms, int consecutiveBlocks) noexcept
{
    logAudioEvent(AudioEventType::SilenceDetected, rms, static_cast<float>(consecutiveBlocks), 0.0f);
}

inline void logWatchdogFallback(float threshold, int triggerCount) noexcept
{
    logAudioEvent(AudioEventType::WatchdogFallback, threshold, static_cast<float>(triggerCount), 0.0f);
}

// Phase 2: Retro fast path logging
inline void logPaintStroke(float frequency, float amplitude, float hue, int oscillatorCount) noexcept
{
    logAudioEvent(AudioEventType::PaintGesture, frequency, amplitude, hue);
    logAudioEvent(AudioEventType::AudioBlock, static_cast<float>(oscillatorCount), 0.0f, 0.0f);
}

// Consumer-side event reading (non-RT thread ONLY)
inline bool readAudioEvent(AudioTraceEvent& event) noexcept
{
    return g_traceBuffer.pop(event);
}

// Get trace statistics (non-RT thread)
inline size_t getTraceBufferSize() noexcept
{
    return g_traceBuffer.size();
}

inline uint32_t getTraceDropCount() noexcept
{
    return g_traceBuffer.getDropCount();
}

inline void resetTraceDropCount() noexcept
{
    g_traceBuffer.resetDropCount();
}

} // namespace SpectralCanvas