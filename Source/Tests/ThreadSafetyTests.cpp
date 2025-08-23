/**
 * Thread Safety Test Suite for SpectralCanvas Pro
 * 
 * Comprehensive tests for all critical thread-safe components
 * Run with ThreadSanitizer for complete race condition detection
 * 
 * @author SpectralCanvas Team
 * @version 1.0
 */

#include <JuceHeader.h>
#include "Core/PaintQueue.h"
#include "Core/RealtimeMemoryManager.h"
#include <thread>
#include <chrono>
#include <random>
#include <vector>

class ThreadSafetyTests : public juce::UnitTest
{
public:
    ThreadSafetyTests() 
        : UnitTest("Thread Safety Tests", "Audio")
    {
    }
    
    void runTest() override
    {
        beginTest("Paint Queue Thread Safety");
        testPaintQueueConcurrency();
        
        beginTest("Memory Manager Thread Safety");
        testMemoryManagerConcurrency();
        
        beginTest("Memory Ordering Verification");
        testMemoryOrdering();
        
        beginTest("Stress Test - High Load Simulation");
        testHighLoadStability();
    }
    
private:
    /**
     * Test 1: Paint Queue Concurrent Access
     */
    void testPaintQueueConcurrency()
    {
        SpectralPaintQueue queue;
        std::atomic<int> successfulPushes{0};
        std::atomic<int> successfulPops{0};
        std::atomic<bool> stopFlag{false};
        
        const int NUM_PRODUCERS = 4;
        const int NUM_CONSUMERS = 2;
        const int EVENTS_PER_PRODUCER = 10000;
        
        // Launch producer threads (UI threads)
        std::vector<std::thread> producers;
        for (int i = 0; i < NUM_PRODUCERS; ++i)
        {
            producers.emplace_back([&queue, &successfulPushes, i, EVENTS_PER_PRODUCER]()
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                
                for (int j = 0; j < EVENTS_PER_PRODUCER; ++j)
                {
                    PaintEvent event(
                        dist(gen),  // nx (0-1)
                        dist(gen),  // ny (0-1)
                        dist(gen),  // pressure (0-1)
                        (j % 3 == 0) ? kStrokeStart : 
                        (j % 3 == 1) ? kStrokeMove : kStrokeEnd,
                        0xFF0000FF + (i << 8)  // Color with thread ID
                    );
                    
                    if (queue.push(event))
                    {
                        successfulPushes.fetch_add(1, std::memory_order_relaxed);
                    }
                    
                    // Simulate realistic UI timing
                    if (j % 100 == 0)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(10));
                    }
                }
            });
        }
        
        // Launch consumer threads (Audio threads)
        std::vector<std::thread> consumers;
        for (int i = 0; i < NUM_CONSUMERS; ++i)
        {
            consumers.emplace_back([&queue, &successfulPops, &stopFlag]()
            {
                PaintEvent event;
                while (!stopFlag.load(std::memory_order_acquire))
                {
                    if (queue.pop(event))
                    {
                        successfulPops.fetch_add(1, std::memory_order_relaxed);
                        
                        // Verify event integrity
                        expect(event.nx >= 0.0f && event.nx <= 1.0f, "X coordinate in valid range");
                        expect(event.ny >= 0.0f && event.ny <= 1.0f, "Y coordinate in valid range");
                        expect(event.pressure >= 0.0f && event.pressure <= 1.0f, "Pressure in valid range");
                        expect((event.flags & (kStrokeStart | kStrokeMove | kStrokeEnd)) != 0, "Valid flags");
                    }
                    else
                    {
                        // Queue empty, brief wait
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                    }
                }
                
                // Drain remaining events
                while (queue.pop(event))
                {
                    successfulPops.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        // Wait for producers to finish
        for (auto& t : producers)
        {
            t.join();
        }
        
        // Signal consumers to stop
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        stopFlag.store(true, std::memory_order_release);
        
        // Wait for consumers
        for (auto& t : consumers)
        {
            t.join();
        }
        
        // Verify results
        logMessage("Successful pushes: " + juce::String(successfulPushes.load()));
        logMessage("Successful pops: " + juce::String(successfulPops.load()));
        logMessage("Queue size at end: " + juce::String(queue.approxSize()));
        
        expect(successfulPops.load() == successfulPushes.load(),
               "All pushed events should be popped");
    }
    
    /**
     * Test 2: Memory Manager Concurrent Operations
     */
    void testMemoryManagerConcurrency()
    {
        auto& memSystem = RealtimeMemorySystem::getInstance();
        std::atomic<int> successfulAllocations{0};
        std::atomic<int> successfulDeallocations{0};
        std::atomic<bool> stopFlag{false};
        
        const int NUM_THREADS = 8;
        const int OPERATIONS_PER_THREAD = 5000;
        
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            threads.emplace_back([&memSystem, &successfulAllocations, &successfulDeallocations, OPERATIONS_PER_THREAD]()
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> opType(0, 2);
                std::vector<juce::AudioBuffer<float>*> myBuffers;
                std::vector<RealtimeMemorySystem::AudioBufferPool<4096, 32>::ScopedBuffer> scopedBuffers;
                
                for (int j = 0; j < OPERATIONS_PER_THREAD; ++j)
                {
                    int op = opType(gen);
                    
                    switch (op)
                    {
                        case 0:  // Allocate audio buffer
                        {
                            auto* buffer = memSystem.audioBufferPool.acquire();
                            if (buffer)
                            {
                                successfulAllocations.fetch_add(1, std::memory_order_relaxed);
                                myBuffers.push_back(buffer);
                                
                                // Use the buffer briefly
                                std::this_thread::sleep_for(std::chrono::microseconds(10));
                            }
                            break;
                        }
                        case 1:  // Use scoped buffer
                        {
                            RealtimeMemorySystem::AudioBufferPool<4096, 32>::ScopedBuffer scopedBuffer(memSystem.largeBufferPool);
                            if (scopedBuffer)
                            {
                                successfulAllocations.fetch_add(1, std::memory_order_relaxed);
                                // Buffer will be automatically released
                            }
                            break;
                        }
                        case 2:  // Release buffers
                        {
                            if (!myBuffers.empty())
                            {
                                auto* buffer = myBuffers.back();
                                myBuffers.pop_back();
                                memSystem.audioBufferPool.release(buffer);
                                successfulDeallocations.fetch_add(1, std::memory_order_relaxed);
                            }
                            break;
                        }
                    }
                }
                
                // Clean up remaining buffers
                for (auto* buffer : myBuffers)
                {
                    memSystem.audioBufferPool.release(buffer);
                    successfulDeallocations.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        // Wait for all threads
        for (auto& t : threads)
        {
            t.join();
        }
        
        // Verify results
        auto stats = memSystem.getGlobalStats();
        logMessage("Total allocations: " + juce::String(stats.allocationCount));
        logMessage("Failed allocations: " + juce::String(stats.failedAllocations));
        logMessage("Audio buffer pool utilization: " + juce::String(memSystem.audioBufferPool.getUtilization() * 100.0f) + "%");
        
        expect(stats.failedAllocations == 0 || stats.failedAllocations < stats.allocationCount * 0.1f, 
               "Failed allocation rate should be low");
    }
    
    /**
     * Test 3: Memory Ordering Verification
     */
    void testMemoryOrdering()
    {
        struct SharedData
        {
            std::atomic<int> flag{0};
            int data = 0;
        } shared;
        
        std::atomic<bool> success{true};
        const int ITERATIONS = 10000;
        
        // Writer thread
        std::thread writer([&shared, ITERATIONS]()
        {
            for (int i = 0; i < ITERATIONS; ++i)
            {
                shared.data = i;
                std::atomic_thread_fence(std::memory_order_release);
                shared.flag.store(1, std::memory_order_release);
                
                while (shared.flag.load(std::memory_order_acquire) != 0)
                {
                    std::this_thread::yield();
                }
            }
        });
        
        // Reader thread
        std::thread reader([&shared, &success, ITERATIONS]()
        {
            for (int i = 0; i < ITERATIONS; ++i)
            {
                while (shared.flag.load(std::memory_order_acquire) != 1)
                {
                    std::this_thread::yield();
                }
                
                std::atomic_thread_fence(std::memory_order_acquire);
                if (shared.data != i)
                {
                    success.store(false, std::memory_order_relaxed);
                }
                
                shared.flag.store(0, std::memory_order_release);
            }
        });
        
        writer.join();
        reader.join();
        
        expect(success.load(), "Memory ordering should ensure data consistency");
    }
    
    /**
     * Test 4: High Load Stability Test
     */
    void testHighLoadStability()
    {
        SpectralPaintQueue paintQueue;
        auto& memSystem = RealtimeMemorySystem::getInstance();
        
        std::atomic<bool> stopFlag{false};
        std::atomic<uint64_t> totalOperations{0};
        std::atomic<uint64_t> errors{0};
        
        // Simulate high load for shorter duration
        const int SIMULATION_SECONDS = 5;
        
        // Launch worker threads
        std::vector<std::thread> workers;
        for (int i = 0; i < 6; ++i)
        {
            workers.emplace_back([&paintQueue, &memSystem, &stopFlag, &totalOperations, &errors, i]()
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> opType(0, 3);
                std::uniform_real_distribution<float> coord(0.0f, 1.0f);
                
                while (!stopFlag.load(std::memory_order_acquire))
                {
                    int op = opType(gen);
                    
                    try 
                    {
                        switch (op)
                        {
                            case 0:  // Push paint event
                            {
                                PaintEvent event(coord(gen), coord(gen), coord(gen), kStrokeMove, 0xFF000000 + i);
                                paintQueue.push(event);
                                break;
                            }
                            case 1:  // Pop paint event
                            {
                                PaintEvent event;
                                paintQueue.pop(event);
                                break;
                            }
                            case 2:  // Memory allocation test
                            {
                                RealtimeMemorySystem::AudioBufferPool<4096, 32>::ScopedBuffer buffer(memSystem.largeBufferPool);
                                if (buffer)
                                {
                                    // Use buffer briefly
                                    buffer->clear();
                                }
                                break;
                            }
                            case 3:  // Performance timing
                            {
                                RT_SCOPED_TIMER("HighLoadTest");
                                std::this_thread::sleep_for(std::chrono::microseconds(1));
                                break;
                            }
                        }
                        
                        totalOperations.fetch_add(1, std::memory_order_relaxed);
                    }
                    catch (...)
                    {
                        errors.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            });
        }
        
        // Run simulation
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::seconds(SIMULATION_SECONDS));
        stopFlag.store(true, std::memory_order_release);
        
        // Wait for workers
        for (auto& t : workers)
        {
            t.join();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Check results
        uint64_t ops = totalOperations.load();
        double opsPerSecond = (double)ops / ((double)duration.count() / 1000.0);
        
        logMessage("Total operations: " + juce::String(ops));
        logMessage("Operations per second: " + juce::String(opsPerSecond, 0));
        logMessage("Errors: " + juce::String(errors.load()));
        logMessage("Paint queue final size: " + juce::String(paintQueue.approxSize()));
        
        expect(errors.load() == 0, "No exceptions should occur during stress test");
        expect(opsPerSecond > 1000.0, "Should achieve reasonable throughput");
        
        // Print memory report
        memSystem.printMemoryReport();
    }
};

// Register the test suite
static ThreadSafetyTests threadSafetyTests;