// Paint Performance Benchmark Test for Living Canvas
// Measures gesture throughput, latency, and validates real-time performance requirements
//
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>
#include <juce_dsp/juce_dsp.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#define JucePlugin_Name "SpectralCanvas Pro"

// Forward declarations  
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

// Include paint queue for performance testing
#include "../Source/Core/PaintQueue.h"

struct TestPaintPerformance : juce::UnitTest
{
    TestPaintPerformance() : juce::UnitTest ("Living Canvas — Performance", "Performance") {}

    void runTest() override
    {
        beginTest ("Gesture Dispatch Latency");
        testGestureDispatchLatency();
        
        beginTest ("Maximum Throughput");  
        testMaximumThroughput();
        
        beginTest ("Sustained Load Testing");
        testSustainedLoad();
        
        beginTest ("Memory Usage Stability");
        testMemoryUsageStability();
    }

private:
    void testGestureDispatchLatency()
    {
        // Test individual gesture dispatch times
        SpscRing<PaintGesture, 128> queue;
        
        const int numSamples = 10000;
        std::vector<double> latencies;
        latencies.reserve(numSamples);
        
        PaintGesture gesture;
        gesture.x = 100.0f;
        gesture.y = 200.0f;
        gesture.pressure = 0.5f;
        
        // Warm up
        for (int i = 0; i < 100; ++i)
        {
            gesture.ticks = juce::Time::getHighResolutionTicks();
            queue.forcePush(gesture);
        }
        
        // Measure individual dispatch latencies
        for (int i = 0; i < numSamples; ++i)
        {
            auto start = std::chrono::high_resolution_clock::now();
            
            gesture.ticks = juce::Time::getHighResolutionTicks();
            queue.forcePush(gesture);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            latencies.push_back(static_cast<double>(duration.count()) / 1000.0); // Convert to µs
        }
        
        // Calculate statistics
        double sum = 0.0;
        double maxLatency = 0.0;
        double minLatency = latencies[0];
        
        for (double latency : latencies)
        {
            sum += latency;
            maxLatency = std::max(maxLatency, latency);
            minLatency = std::min(minLatency, latency);
        }
        
        double avgLatency = sum / numSamples;
        
        // Calculate percentiles
        std::sort(latencies.begin(), latencies.end());
        double p95Latency = latencies[static_cast<size_t>(numSamples * 0.95)];
        double p99Latency = latencies[static_cast<size_t>(numSamples * 0.99)];
        
        std::cout << "Gesture Dispatch Latency Results:" << std::endl;
        std::cout << "  Average: " << avgLatency << " µs" << std::endl;
        std::cout << "  Min: " << minLatency << " µs" << std::endl;
        std::cout << "  Max: " << maxLatency << " µs" << std::endl;
        std::cout << "  95th percentile: " << p95Latency << " µs" << std::endl;
        std::cout << "  99th percentile: " << p99Latency << " µs" << std::endl;
        
        // Validate RT requirements
        expect (avgLatency < 10.0, "Average dispatch latency must be < 10µs");
        expect (p95Latency < 20.0, "95th percentile dispatch latency must be < 20µs"); 
        expect (p99Latency < 50.0, "99th percentile dispatch latency must be < 50µs");
    }
    
    void testMaximumThroughput()
    {
        // Test maximum gesture throughput under ideal conditions
        SpscRing<PaintGesture, 128> queue;
        
        const int testDurationMs = 1000;
        const auto testDuration = std::chrono::milliseconds(testDurationMs);
        
        std::atomic<int> gesturesProcessed{0};
        std::atomic<bool> testRunning{true};
        
        PaintGesture gesture;
        gesture.x = 50.0f;
        gesture.y = 75.0f;
        gesture.pressure = 0.8f;
        
        // Producer thread - maximum rate gesture generation
        std::thread producer([&queue, &gesture, &gesturesProcessed, &testRunning]() {
            while (testRunning)
            {
                gesture.ticks = juce::Time::getHighResolutionTicks();
                queue.forcePush(gesture);
                gesturesProcessed++;
            }
        });
        
        // Consumer thread - maximum rate consumption
        std::thread consumer([&queue, &testRunning]() {
            PaintGesture retrieved;
            while (testRunning)
            {
                queue.pop(retrieved);
                // Minimal processing to simulate audio thread work
            }
        });
        
        // Let test run for specified duration
        std::this_thread::sleep_for(testDuration);
        testRunning = false;
        
        producer.join();
        consumer.join();
        
        int totalGestures = gesturesProcessed;
        double gesturesPerSecond = (static_cast<double>(totalGestures) / testDurationMs) * 1000.0;
        
        std::cout << "Maximum Throughput Results:" << std::endl;
        std::cout << "  Total gestures in " << testDurationMs << "ms: " << totalGestures << std::endl;
        std::cout << "  Throughput: " << gesturesPerSecond << " gestures/second" << std::endl;
        
        // Validate minimum throughput requirements
        expect (gesturesPerSecond > 10000, "Must support at least 10,000 gestures/second");
        expect (totalGestures > 0, "Test must have processed some gestures");
    }
    
    void testSustainedLoad()
    {
        // Test performance under sustained realistic load
        SpscRing<PaintGesture, 128> queue;
        
        const int testDurationMs = 5000; // 5 second test
        const double targetGestureRate = 1000.0; // 1000 gestures/second (realistic)
        const auto gestureInterval = std::chrono::microseconds(static_cast<int>(1000000.0 / targetGestureRate));
        
        std::atomic<int> gesturesProduced{0};
        std::atomic<int> gesturesConsumed{0};
        std::atomic<bool> testRunning{true};
        
        PaintGesture gesture;
        gesture.pressure = 0.6f;
        
        // Producer thread - sustained realistic rate
        std::thread producer([&]() {
            auto nextGestureTime = std::chrono::high_resolution_clock::now();
            
            while (testRunning)
            {
                auto now = std::chrono::high_resolution_clock::now();
                if (now >= nextGestureTime)
                {
                    gesture.x = static_cast<float>(gesturesProduced % 800);
                    gesture.y = static_cast<float>((gesturesProduced / 10) % 600);
                    gesture.ticks = juce::Time::getHighResolutionTicks();
                    
                    queue.forcePush(gesture);
                    gesturesProduced++;
                    
                    nextGestureTime += gestureInterval;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            }
        });
        
        // Consumer thread - sustained processing
        std::thread consumer([&]() {
            PaintGesture retrieved;
            
            while (testRunning)
            {
                if (queue.pop(retrieved))
                {
                    gesturesConsumed++;
                    
                    // Simulate minimal audio processing work
                    volatile double dummy = retrieved.x * retrieved.y * retrieved.pressure;
                    (void)dummy;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            }
        });
        
        // Run sustained test
        std::this_thread::sleep_for(std::chrono::milliseconds(testDurationMs));
        testRunning = false;
        
        producer.join();
        consumer.join();
        
        int produced = gesturesProduced;
        int consumed = gesturesConsumed;
        double efficiency = (static_cast<double>(consumed) / produced) * 100.0;
        
        std::cout << "Sustained Load Results:" << std::endl;
        std::cout << "  Duration: " << testDurationMs << "ms" << std::endl;
        std::cout << "  Gestures produced: " << produced << std::endl;
        std::cout << "  Gestures consumed: " << consumed << std::endl;
        std::cout << "  Processing efficiency: " << efficiency << "%" << std::endl;
        
        // Validate sustained performance
        expect (produced > (testDurationMs * targetGestureRate / 1000 * 0.8), "Producer should maintain target rate within 20%");
        expect (consumed > (produced * 0.95), "Consumer should process at least 95% of gestures");
        expect (efficiency > 95.0, "Processing efficiency should be > 95%");
    }
    
    void testMemoryUsageStability()
    {
        // Test that memory usage remains stable under load
        SpscRing<PaintGesture, 128> queue;
        
        PaintGesture gesture;
        gesture.x = 300.0f;
        gesture.y = 400.0f;
        gesture.pressure = 0.7f;
        
        // Fill and empty queue multiple times to test for memory leaks
        const int cycles = 1000;
        
        for (int cycle = 0; cycle < cycles; ++cycle)
        {
            // Fill queue
            for (int i = 0; i < 64; ++i) // Half capacity
            {
                gesture.ticks = juce::Time::getHighResolutionTicks();
                queue.forcePush(gesture);
            }
            
            // Empty queue
            PaintGesture retrieved;
            for (int i = 0; i < 64; ++i)
            {
                queue.pop(retrieved);
            }
        }
        
        // Test queue still functions correctly
        expect (queue.push(gesture), "Queue should still accept gestures after stress test");
        
        PaintGesture finalGesture;
        expect (queue.pop(finalGesture), "Queue should still provide gestures after stress test");
        expect (finalGesture.x == gesture.x, "Data integrity preserved after stress test");
        
        std::cout << "Memory stability test completed: " << cycles << " fill/empty cycles" << std::endl;
    }
};

static TestPaintPerformance performanceTest;

// Main function for the test runner
int main()
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;
    juce::UnitTestRunner runner;
    runner.setPassesAreLogged(true);
    
    std::cout << "Running Living Canvas Performance Benchmarks..." << std::endl;
    runner.runAllTests();
    
    // Check results
    int failures = 0;
    int passes = 0;
    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        if (const auto* result = runner.getResult(i))
        {
            failures += result->failures;
            passes += result->passes;
            std::cout << "Test: " << result->unitTestName << " - " 
                      << result->passes << " passed, " 
                      << result->failures << " failed" << std::endl;
        }
    }
    
    std::cout << "\n=== PERFORMANCE BENCHMARK COMPLETE ===" << std::endl;
    std::cout << "Total passes: " << passes << std::endl;
    std::cout << "Total failures: " << failures << std::endl;
    std::cout << "Result: " << (failures == 0 ? "SUCCESS" : "FAILED") << std::endl;
    
    return failures > 0 ? 1 : 0;
}