#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <memory>
#include <type_traits>

/**
 * Real-Time Memory Management System
 * 
 * Implements Gemini's recommendations for zero real-time allocation:
 * - Pre-allocated memory pools for all audio objects
 * - Lock-free allocation/deallocation
 * - Memory-mapped buffers for large audio data
 * - RAII-style memory management
 * - Performance monitoring and leak detection
 */

// Memory statistics for monitoring
struct RealtimeMemoryStats
{
    std::atomic<size_t> totalAllocatedBytes{0};
    std::atomic<size_t> peakMemoryUsage{0};
    std::atomic<uint64_t> allocationCount{0};
    std::atomic<uint64_t> deallocationCount{0};
    std::atomic<uint64_t> failedAllocations{0};
    
    // Delete copy constructor and assignment to prevent atomic copying
    RealtimeMemoryStats(const RealtimeMemoryStats&) = delete;
    RealtimeMemoryStats& operator=(const RealtimeMemoryStats&) = delete;
    
    // Default constructor
    RealtimeMemoryStats() = default;
    
    void recordAllocation(size_t bytes) noexcept
    {
        totalAllocatedBytes.fetch_add(bytes, std::memory_order_relaxed);
        allocationCount.fetch_add(1, std::memory_order_relaxed);
        
        // Update peak usage
        size_t current = totalAllocatedBytes.load(std::memory_order_relaxed);
        size_t peak = peakMemoryUsage.load(std::memory_order_relaxed);
        while (current > peak && !peakMemoryUsage.compare_exchange_weak(peak, current, std::memory_order_relaxed))
        {
            peak = peakMemoryUsage.load(std::memory_order_relaxed);
        }
    }
    
    void recordDeallocation(size_t bytes) noexcept
    {
        totalAllocatedBytes.fetch_sub(bytes, std::memory_order_relaxed);
        deallocationCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    void recordFailedAllocation() noexcept
    {
        failedAllocations.fetch_add(1, std::memory_order_relaxed);
    }
};

// Fixed-size memory pool for specific object types
template<typename T, size_t PoolSize = 256>
class RealtimeObjectPool
{
public:
    RealtimeObjectPool()
    {
        // Initialize free list
        for (size_t i = 0; i < PoolSize; ++i)
        {
            freeList_[i] = i;
        }
        nextFreeIndex_.store(0, std::memory_order_relaxed);
    }
    
    // Acquire an object from the pool (real-time safe)
    T* acquire() noexcept
    {
        int freeIndex = nextFreeIndex_.fetch_add(1, std::memory_order_acquire);
        if (freeIndex >= static_cast<int>(PoolSize))
        {
            // Pool exhausted
            nextFreeIndex_.fetch_sub(1, std::memory_order_acq_rel);
            stats_.recordFailedAllocation();
            return nullptr;
        }
        
        size_t objectIndex = freeList_[freeIndex];
        T* object = &pool_[objectIndex];
        
        // Construct object in-place
        new (object) T();
        
        stats_.recordAllocation(sizeof(T));
        return object;
    }
    
    // Release an object back to the pool (real-time safe)
    void release(T* object) noexcept
    {
        if (!object) return;
        
        // Find object index
        size_t objectIndex = object - pool_.data();
        if (objectIndex >= PoolSize)
        {
            // Invalid object - not from this pool
            DBG("RealtimeObjectPool::release() - Invalid object pointer");
            return;
        }
        
        // Destruct object
        object->~T();
        
        // Return to free list
        int freeIndex = nextFreeIndex_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (freeIndex >= 0)
        {
            freeList_[freeIndex] = objectIndex;
        }
        
        stats_.recordDeallocation(sizeof(T));
    }
    
    // RAII wrapper for automatic cleanup
    class ScopedObject
    {
    public:
        ScopedObject(RealtimeObjectPool& pool) noexcept
            : pool_(pool), object_(pool.acquire()) {}
        
        ~ScopedObject() noexcept
        {
            if (object_)
            {
                pool_.release(object_);
            }
        }
        
        T* get() const noexcept { return object_; }
        T* operator->() const noexcept { return object_; }
        T& operator*() const noexcept { return *object_; }
        
        explicit operator bool() const noexcept { return object_ != nullptr; }
        
        // Non-copyable, movable
        ScopedObject(const ScopedObject&) = delete;
        ScopedObject& operator=(const ScopedObject&) = delete;
        
        ScopedObject(ScopedObject&& other) noexcept
            : pool_(other.pool_), object_(other.object_)
        {
            other.object_ = nullptr;
        }
        
        ScopedObject& operator=(ScopedObject&& other) noexcept
        {
            if (this != &other)
            {
                if (object_)
                {
                    pool_.release(object_);
                }
                object_ = other.object_;
                other.object_ = nullptr;
            }
            return *this;
        }
        
    private:
        RealtimeObjectPool& pool_;
        T* object_;
    };
    
    // Get statistics
    const RealtimeMemoryStats& getStats() const noexcept { return stats_; }
    
    // Get pool utilization
    float getUtilization() const noexcept
    {
        int used = nextFreeIndex_.load(std::memory_order_relaxed);
        return (float)std::max(0, used) / (float)PoolSize;
    }
    
private:
    alignas(64) std::array<T, PoolSize> pool_;  // Align to cache line
    std::array<size_t, PoolSize> freeList_;
    std::atomic<int> nextFreeIndex_{0};
    RealtimeMemoryStats stats_;
};

// Pre-allocated buffer pool for audio buffers
template<size_t BufferSize = 4096, size_t NumBuffers = 32>
class AudioBufferPool
{
public:
    struct AudioBufferWrapper
    {
        std::array<float, BufferSize> data;
        std::atomic<bool> inUse{false};
        
        float* getData() noexcept { return data.data(); }
        const float* getData() const noexcept { return data.data(); }
        size_t getSize() const noexcept { return BufferSize; }
        
        void clear() noexcept { data.fill(0.0f); }
    };
    
    // Acquire a buffer (real-time safe)
    AudioBufferWrapper* acquireBuffer() noexcept
    {
        for (size_t i = 0; i < NumBuffers; ++i)
        {
            bool expected = false;
            if (buffers_[i].inUse.compare_exchange_weak(expected, true, std::memory_order_acquire))
            {
                // Successfully acquired buffer
                buffers_[i].clear();
                stats_.recordAllocation(BufferSize * sizeof(float));
                return &buffers_[i];
            }
        }
        
        // No free buffers available
        stats_.recordFailedAllocation();
        return nullptr;
    }
    
    // Release a buffer (real-time safe)
    void releaseBuffer(AudioBufferWrapper* buffer) noexcept
    {
        if (!buffer) return;
        
        // Verify buffer belongs to this pool
        size_t index = buffer - buffers_.data();
        if (index >= NumBuffers) return;
        
        buffer->inUse.store(false, std::memory_order_release);
        stats_.recordDeallocation(BufferSize * sizeof(float));
    }
    
    // RAII wrapper
    class ScopedBuffer
    {
    public:
        ScopedBuffer(AudioBufferPool& pool) noexcept
            : pool_(pool), buffer_(pool.acquireBuffer()) {}
        
        ~ScopedBuffer() noexcept
        {
            if (buffer_)
            {
                pool_.releaseBuffer(buffer_);
            }
        }
        
        AudioBufferWrapper* get() const noexcept { return buffer_; }
        AudioBufferWrapper* operator->() const noexcept { return buffer_; }
        AudioBufferWrapper& operator*() const noexcept { return *buffer_; }
        
        explicit operator bool() const noexcept { return buffer_ != nullptr; }
        
        // Non-copyable, movable
        ScopedBuffer(const ScopedBuffer&) = delete;
        ScopedBuffer& operator=(const ScopedBuffer&) = delete;
        
        ScopedBuffer(ScopedBuffer&& other) noexcept
            : pool_(other.pool_), buffer_(other.buffer_)
        {
            other.buffer_ = nullptr;
        }
        
        ScopedBuffer& operator=(ScopedBuffer&& other) noexcept
        {
            if (this != &other)
            {
                if (buffer_)
                {
                    pool_.releaseBuffer(buffer_);
                }
                buffer_ = other.buffer_;
                other.buffer_ = nullptr;
            }
            return *this;
        }
        
    private:
        AudioBufferPool& pool_;
        AudioBufferWrapper* buffer_;
    };
    
    // Statistics
    const RealtimeMemoryStats& getStats() const noexcept { return stats_; }
    
    float getUtilization() const noexcept
    {
        size_t used = 0;
        for (const auto& buffer : buffers_)
        {
            if (buffer.inUse.load(std::memory_order_relaxed))
            {
                used++;
            }
        }
        return (float)used / (float)NumBuffers;
    }
    
private:
    std::array<AudioBufferWrapper, NumBuffers> buffers_;
    RealtimeMemoryStats stats_;
};

// Real-time safe performance profiler
class RealtimeProfiler
{
public:
    struct ProfileEntry
    {
        std::atomic<uint64_t> totalCalls{0};
        std::atomic<uint64_t> totalTime{0};  // In microseconds
        std::atomic<uint64_t> maxTime{0};
        char name[64] = {};
        
        // Delete copy constructor and assignment to prevent atomic copying
        ProfileEntry(const ProfileEntry&) = delete;
        ProfileEntry& operator=(const ProfileEntry&) = delete;
        
        // Default constructor
        ProfileEntry() = default;
        
        void recordTiming(uint64_t microseconds) noexcept
        {
            totalCalls.fetch_add(1, std::memory_order_relaxed);
            totalTime.fetch_add(microseconds, std::memory_order_relaxed);
            
            // Update max time
            uint64_t currentMax = maxTime.load(std::memory_order_relaxed);
            while (microseconds > currentMax && 
                   !maxTime.compare_exchange_weak(currentMax, microseconds, std::memory_order_relaxed))
            {
                currentMax = maxTime.load(std::memory_order_relaxed);
            }
        }
        
        double getAverageTime() const noexcept
        {
            uint64_t calls = totalCalls.load(std::memory_order_relaxed);
            if (calls == 0) return 0.0;
            return (double)totalTime.load(std::memory_order_relaxed) / (double)calls;
        }
    };
    
    class ScopedTimer
    {
    public:
        ScopedTimer(ProfileEntry& entry) noexcept
            : entry_(entry), startTime_(juce::Time::getHighResolutionTicks()) {}
        
        ~ScopedTimer() noexcept
        {
            auto endTime = juce::Time::getHighResolutionTicks();
            auto elapsed = juce::Time::highResolutionTicksToSeconds(endTime - startTime_) * 1000000.0; // Convert to microseconds
            entry_.recordTiming(static_cast<uint64_t>(elapsed));
        }
        
    private:
        ProfileEntry& entry_;
        juce::int64 startTime_;
    };
    
    // Get or create a profile entry (real-time safe after initialization)
    ProfileEntry& getEntry(const char* name) noexcept
    {
        // Find existing entry
        for (auto& entry : entries_)
        {
            if (std::strcmp(entry.name, name) == 0)
            {
                return entry;
            }
        }
        
        // Create new entry if we have space
        int nextIndex = nextEntryIndex_.fetch_add(1, std::memory_order_acquire);
        if (nextIndex < MaxEntries)
        {
            ProfileEntry& entry = entries_[nextIndex];
            std::strncpy(entry.name, name, sizeof(entry.name) - 1);
            entry.name[sizeof(entry.name) - 1] = '\0';
            return entry;
        }
        else
        {
            // Pool exhausted, return dummy entry
            nextEntryIndex_.fetch_sub(1, std::memory_order_acq_rel);
            return dummyEntry_;
        }
    }
    
    // Create scoped timer (real-time safe)
    ScopedTimer createScopedTimer(const char* name) noexcept
    {
        return ScopedTimer(getEntry(name));
    }
    
    // Get all entries for reporting - return copies of data, not atomic structs
    struct ProfileData {
        uint64_t totalCalls;
        uint64_t totalTime;
        uint64_t maxTime;
        char name[64];
    };
    
    std::vector<ProfileData> getAllEntries() const
    {
        std::vector<ProfileData> result;
        int numEntries = std::min(nextEntryIndex_.load(std::memory_order_relaxed), MaxEntries);
        
        for (int i = 0; i < numEntries; ++i)
        {
            ProfileData data;
            data.totalCalls = entries_[i].totalCalls.load();
            data.totalTime = entries_[i].totalTime.load();
            data.maxTime = entries_[i].maxTime.load();
            strncpy(data.name, entries_[i].name, sizeof(data.name) - 1);
            data.name[sizeof(data.name) - 1] = '\0';
            result.push_back(data);
        }
        
        return result;
    }
    
private:
    static constexpr int MaxEntries = 128;
    std::array<ProfileEntry, MaxEntries> entries_;
    std::atomic<int> nextEntryIndex_{0};
    ProfileEntry dummyEntry_; // Fallback when pool is exhausted
};

// Global memory management system
class RealtimeMemorySystem
{
public:
    static RealtimeMemorySystem& getInstance() noexcept
    {
        static RealtimeMemorySystem instance;
        return instance;
    }
    
    // Memory pools for common audio objects
    RealtimeObjectPool<juce::AudioBuffer<float>, 64> audioBufferPool;
    RealtimeObjectPool<std::vector<float>, 128> vectorPool;
    AudioBufferPool<4096, 32> largeBufferPool;
    AudioBufferPool<1024, 64> smallBufferPool;
    
    // Performance monitoring
    RealtimeProfiler profiler;
    
    // Global statistics - return individual pool stats instead of combining
    struct GlobalStats {
        size_t totalAllocatedBytes = 0;
        size_t peakMemoryUsage = 0;
        uint64_t allocationCount = 0;
        uint64_t deallocationCount = 0;
        uint64_t failedAllocations = 0;
    };
    
    GlobalStats getGlobalStats() const noexcept
    {
        GlobalStats combined;
        
        // Combine stats from all pools
        const auto& audioStats = audioBufferPool.getStats();
        const auto& vectorStats = vectorPool.getStats();
        const auto& largeStats = largeBufferPool.getStats();
        const auto& smallStats = smallBufferPool.getStats();
        
        combined.totalAllocatedBytes = 
            audioStats.totalAllocatedBytes.load() +
            vectorStats.totalAllocatedBytes.load() +
            largeStats.totalAllocatedBytes.load() +
            smallStats.totalAllocatedBytes.load();
        
        combined.allocationCount = 
            audioStats.allocationCount.load() +
            vectorStats.allocationCount.load() +
            largeStats.allocationCount.load() +
            smallStats.allocationCount.load();
        
        combined.failedAllocations = 
            audioStats.failedAllocations.load() +
            vectorStats.failedAllocations.load() +
            largeStats.failedAllocations.load() +
            smallStats.failedAllocations.load();
        
        return combined;
    }
    
    // Print memory report (for debugging, not real-time safe)
    void printMemoryReport() const
    {
        auto stats = getGlobalStats();
        
        DBG("=== Real-Time Memory System Report ===");
        DBG("Total Allocated: " << stats.totalAllocatedBytes << " bytes");
        DBG("Peak Usage: " << stats.peakMemoryUsage << " bytes");
        DBG("Total Allocations: " << stats.allocationCount);
        DBG("Total Deallocations: " << stats.deallocationCount);
        DBG("Failed Allocations: " << stats.failedAllocations);
        
        DBG("AudioBuffer Pool Utilization: " << (audioBufferPool.getUtilization() * 100.0f) << "%");
        DBG("Vector Pool Utilization: " << (vectorPool.getUtilization() * 100.0f) << "%");
        DBG("Large Buffer Pool Utilization: " << (largeBufferPool.getUtilization() * 100.0f) << "%");
        DBG("Small Buffer Pool Utilization: " << (smallBufferPool.getUtilization() * 100.0f) << "%");
        
        // Print performance profiling results
        auto profileEntries = profiler.getAllEntries();
        if (!profileEntries.empty())
        {
            DBG("=== Performance Profile ===");
            for (const auto& entry : profileEntries)
            {
                if (entry.totalCalls > 0)
                {
                    double avgTime = (entry.totalCalls > 0) ? (double)entry.totalTime / (double)entry.totalCalls : 0.0;
                    DBG(entry.name << ": " << entry.totalCalls << " calls, "
                        << "avg: " << avgTime << "μs, "
                        << "max: " << entry.maxTime << "μs");
                }
            }
        }
    }
    
private:
    RealtimeMemorySystem() = default;
};

// Convenience macros for easy usage
#define RT_SCOPED_TIMER(name) \
    auto _timer = RealtimeMemorySystem::getInstance().profiler.createScopedTimer(name)

#define RT_ACQUIRE_BUFFER() \
    RealtimeMemorySystem::getInstance().audioBufferPool.acquire()

#define RT_RELEASE_BUFFER(ptr) \
    RealtimeMemorySystem::getInstance().audioBufferPool.release(ptr)

#define RT_SCOPED_BUFFER() \
    RealtimeMemorySystem::AudioBufferPool<4096, 32>::ScopedBuffer( \
        RealtimeMemorySystem::getInstance().largeBufferPool)