#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <cstdint>

/**
 * @file HudMetrics.h
 * @brief RT-safe telemetry system for HUD performance counters
 * 
 * Implements lock-free single-producer, single-consumer queue for passing
 * performance metrics from audio thread to GUI thread without allocations
 * or blocking operations.
 * 
 * CLAUDE.md Core Practice 4: HUD Counters
 * - Track evPushed / evPopped / maxQDepth / lastBlockRMS
 * - Update via 30-60Hz timer, never on the audio thread
 * - Use SPSC queue for RT-safe data transfer
 */

namespace SpectralCanvas {

/**
 * @brief Plain Old Data structure for RT-safe telemetry metrics
 * 
 * Contains only POD types to ensure RT-safe copying without
 * constructors, destructors, or virtual function calls.
 */
struct HudMetrics
{
    // Audio level metrics
    float peakL = 0.0f;        // Peak level left channel (0.0-1.0)
    float peakR = 0.0f;        // Peak level right channel (0.0-1.0)
    float rmsL = 0.0f;         // RMS level left channel (0.0-1.0)
    float rmsR = 0.0f;         // RMS level right channel (0.0-1.0)
    
    // Spectral synthesis metrics
    int activeBands = 0;       // Number of active spectral bands
    int topN = 0;              // Top N partials being tracked
    int block = 0;             // Current processing block counter
    
    // Paint-to-audio metrics
    float cps = 0.0f;          // Canvas pixels per second paint rate
    float gamma = 1.0f;        // Gamma correction factor
    
    // System metrics
    double sr = 44100.0;       // Sample rate
    uint32_t serial = 0;       // Serial number for ordering/freshness
    
    // Queue metrics (evPushed / evPopped / maxQDepth per CLAUDE.md)
    uint32_t evPushed = 0;     // Events pushed to paint queue
    uint32_t evPopped = 0;     // Events popped from paint queue
    uint32_t maxQDepth = 0;    // Maximum queue depth observed
    float lastBlockRMS = 0.0f; // RMS of last processed audio block
    
    /**
     * @brief Default constructor - zero-initialize all metrics
     */
    HudMetrics() noexcept = default;
    
    /**
     * @brief Copy constructor - trivial copy for RT-safety
     */
    HudMetrics(const HudMetrics&) noexcept = default;
    
    /**
     * @brief Assignment operator - trivial assignment for RT-safety
     */
    HudMetrics& operator=(const HudMetrics&) noexcept = default;
    
    /**
     * @brief Check if metrics appear valid (basic sanity check)
     */
    bool isValid() const noexcept
    {
        return sr > 0.0 && 
               peakL >= 0.0f && peakL <= 10.0f &&  // Allow some headroom
               peakR >= 0.0f && peakR <= 10.0f &&
               rmsL >= 0.0f && rmsL <= 10.0f &&
               rmsR >= 0.0f && rmsR <= 10.0f &&
               activeBands >= 0 && topN >= 0;
    }
};

/**
 * @brief RT-safe single-producer, single-consumer queue for HUD metrics
 * 
 * Uses juce::AbstractFifo for lock-free operation. The audio thread pushes
 * metrics (producer) and the GUI timer thread pops them (consumer).
 * 
 * Design principles:
 * - No heap allocations in push/pop operations
 * - No locks or blocking operations
 * - Fixed-size buffer allocated at construction
 * - Single producer (audio thread) and single consumer (GUI thread)
 */
class HudQueue
{
public:
    /**
     * @brief Construct HUD queue with specified capacity
     * @param capacity Number of metrics entries to buffer (default 128)
     * 
     * Capacity should be power of 2 for optimal performance, but
     * juce::AbstractFifo handles any size efficiently.
     */
    explicit HudQueue(int capacity = 128) 
        : fifo(capacity)
        , buffer(static_cast<size_t>(capacity))
    {
        jassert(capacity > 0);
        jassert(capacity <= 4096); // Reasonable upper limit
    }
    
    /**
     * @brief Destructor - trivial cleanup
     */
    ~HudQueue() noexcept = default;
    
    /**
     * @brief Push metrics to queue (called from audio thread)
     * @param metrics The metrics to push
     * @return true if successful, false if queue is full
     * 
     * RT-safe: No allocations, no locks, no blocking operations.
     * If queue is full, silently drops the metrics to maintain RT-safety.
     */
    bool push(const HudMetrics& metrics) noexcept
    {
        // Get write space - this is lock-free and RT-safe
        int start1, size1, start2, size2;
        fifo.prepareToWrite(1, start1, size1, start2, size2);
        
        // Check if we have space for at least 1 item
        if (size1 == 0) {
            return false; // Queue is full
        }
        
        // Copy metrics to buffer - simple POD copy, RT-safe
        buffer[static_cast<size_t>(start1)] = metrics;
        
        // Commit the write - this publishes the data to consumer
        fifo.finishedWrite(1);
        
        return true;
    }
    
    /**
     * @brief Pop metrics from queue (called from GUI thread)
     * @param metrics Reference to store the popped metrics
     * @return true if successful, false if queue is empty
     * 
     * RT-safe: No allocations, no locks, no blocking operations.
     * Returns false if no metrics are available.
     */
    bool pop(HudMetrics& metrics) noexcept
    {
        // Get read space - this is lock-free and RT-safe
        int start1, size1, start2, size2;
        fifo.prepareToRead(1, start1, size1, start2, size2);
        
        // Check if we have at least 1 item to read
        if (size1 == 0) {
            return false; // Queue is empty
        }
        
        // Copy metrics from buffer - simple POD copy, RT-safe
        metrics = buffer[static_cast<size_t>(start1)];
        
        // Commit the read - this frees the slot for producer
        fifo.finishedRead(1);
        
        return true;
    }
    
    /**
     * @brief Get approximate number of items in queue
     * @return Number of metrics entries available to read
     * 
     * This is safe to call from any thread and gives an approximate
     * count due to concurrent access patterns.
     */
    int getNumReady() const noexcept
    {
        return fifo.getNumReady();
    }
    
    /**
     * @brief Get total capacity of the queue
     * @return Maximum number of metrics entries that can be buffered
     */
    int getTotalSize() const noexcept
    {
        return fifo.getTotalSize();
    }
    
    /**
     * @brief Check if queue is approximately empty
     * @return true if no items are available to read
     */
    bool isEmpty() const noexcept
    {
        return fifo.getNumReady() == 0;
    }
    
    /**
     * @brief Check if queue is approximately full
     * @return true if no space available for writing
     */
    bool isFull() const noexcept
    {
        return fifo.getFreeSpace() == 0;
    }
    
    /**
     * @brief Reset the queue to empty state
     * 
     * WARNING: This should only be called when no concurrent access
     * is happening (e.g., during initialization or shutdown).
     */
    void reset() noexcept
    {
        fifo.reset();
    }
    
    // Prevent copying to avoid accidental expensive operations
    HudQueue(const HudQueue&) = delete;
    HudQueue& operator=(const HudQueue&) = delete;
    
    // Allow moving for container storage if needed
    HudQueue(HudQueue&&) noexcept = default;
    HudQueue& operator=(HudQueue&&) noexcept = default;

private:
    juce::AbstractFifo fifo;           // Lock-free FIFO management
    std::vector<HudMetrics> buffer;    // Fixed-size buffer for metrics
};

} // namespace SpectralCanvas