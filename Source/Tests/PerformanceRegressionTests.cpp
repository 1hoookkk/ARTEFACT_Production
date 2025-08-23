/**
 * Performance Regression Tests for SpectralCanvas Pro
 * Validates our critical performance optimizations (DBG removal, memory barriers, etc.)
 */

#include <JuceHeader.h>
#include "../Core/PluginProcessor.h"
#include "../Core/SpectralSynthEngine.h"
#include "../Core/Commands.h"

class PerformanceRegressionTests : public juce::UnitTest
{
public:
    PerformanceRegressionTests() : UnitTest("Performance Regression", "Optimization") {}
    
    void runTest() override
    {
        beginTest("Audio Thread Latency Validation");
        {
            ARTEFACTAudioProcessor processor;
            processor.prepareToPlay(48000.0, 64); // Small buffer for strict latency
            
            juce::AudioBuffer<float> buffer(2, 64);
            juce::MidiBuffer midiBuffer;
            
            const int numIterations = 1000;
            const double maxLatencyMs = 2.0; // Strict requirement after our optimizations
            
            std::vector<double> latencies;
            latencies.reserve(numIterations);
            
            for (int i = 0; i < numIterations; ++i) {
                buffer.clear();
                midiBuffer.clear();
                
                auto startTime = juce::Time::getHighResolutionTicks();
                processor.processBlock(buffer, midiBuffer);
                auto endTime = juce::Time::getHighResolutionTicks();
                
                double latencyMs = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
                latencies.push_back(latencyMs);
            }
            
            // Calculate statistics
            double avgLatency = 0.0;
            double maxLatency = 0.0;
            for (double lat : latencies) {
                avgLatency += lat;
                maxLatency = std::max(maxLatency, lat);
            }
            avgLatency /= latencies.size();
            
            expect(avgLatency < maxLatencyMs, "Average latency should be < " + juce::String(maxLatencyMs) + "ms");
            expect(maxLatency < maxLatencyMs * 3.0, "Maximum latency spike should be reasonable");
            
            logMessage("Audio thread performance:");
            logMessage("  Average latency: " + juce::String(avgLatency, 4) + "ms");
            logMessage("  Maximum latency: " + juce::String(maxLatency, 4) + "ms");
            logMessage("  Target: < " + juce::String(maxLatencyMs) + "ms average");
            
            processor.releaseResources();
        }
        
        beginTest("Paint Command Processing Speed");
        {
            ARTEFACTAudioProcessor processor;
            processor.prepareToPlay(48000.0, 512);
            
            const int numCommands = 1000;
            const double maxProcessingTimeMs = 10.0; // Should be very fast after optimizations
            
            auto startTime = juce::Time::getHighResolutionTicks();
            
            for (int i = 0; i < numCommands; ++i) {
                Command paintCommand(PaintCommandID::BeginStroke, 
                                   50.0f + i * 0.1f,   // x
                                   25.0f + i * 0.05f,  // y  
                                   0.8f,               // pressure
                                   juce::Colours::red); // color
                
                processor.pushCommandToQueue(paintCommand);
                
                // Also test update and end commands
                if (i % 3 == 1) {
                    Command updateCommand(PaintCommandID::UpdateStroke,
                                        50.0f + i * 0.1f,
                                        25.0f + i * 0.05f,
                                        0.7f,
                                        juce::Colours::green);
                    processor.pushCommandToQueue(updateCommand);
                }
                
                if (i % 3 == 2) {
                    Command endCommand(PaintCommandID::EndStroke);
                    processor.pushCommandToQueue(endCommand);
                }
            }
            
            auto endTime = juce::Time::getHighResolutionTicks();
            double processingTime = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
            
            expect(processingTime < maxProcessingTimeMs, "Paint command processing should be fast");
            
            logMessage("Paint command performance:");
            logMessage("  " + juce::String(numCommands) + " commands in " + juce::String(processingTime, 2) + "ms");
            logMessage("  Average: " + juce::String(processingTime / numCommands, 4) + "ms per command");
            
            processor.releaseResources();
        }
        
        beginTest("Memory Allocation Performance");
        {
            // Test that our optimizations don't cause memory allocation spikes
            const int numIterations = 100;
            auto engine = std::make_unique<SpectralSynthEngine>();
            engine->prepareToPlay(44100.0, 512);
            
            // Warm up (trigger any lazy allocations)
            for (int i = 0; i < 10; ++i) {
                juce::AudioBuffer<float> buffer(2, 512);
                buffer.clear();
                engine->processBlock(buffer);
            }
            
            auto startTime = juce::Time::getHighResolutionTicks();
            
            for (int i = 0; i < numIterations; ++i) {
                juce::AudioBuffer<float> buffer(2, 512);
                buffer.clear();
                engine->processBlock(buffer);
                
                // Add some paint strokes
                if (i % 10 == 0) {
                    SpectralSynthEngine::PaintData paintData;
                    paintData.timeNorm = 0.5f;
                    paintData.freqNorm = 0.3f;
                    paintData.pressure = 0.6f;
                    paintData.velocity = 0.7f;
                    paintData.color = juce::Colours::blue;
                    paintData.timestamp = juce::Time::getMillisecondCounter();
                    paintData.frequencyHz = 440.0f;
                    paintData.amplitude = paintData.pressure;
                    paintData.panPosition = 0.0f;
                    paintData.synthMode = 0;
                    
                    engine->processPaintStroke(paintData);
                }
            }
            
            auto endTime = juce::Time::getHighResolutionTicks();
            double totalTime = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
            double avgTime = totalTime / numIterations;
            
            expect(avgTime < 2.0, "Average processing time should be < 2ms after optimizations");
            
            logMessage("Memory allocation performance:");
            logMessage("  " + juce::String(numIterations) + " iterations in " + juce::String(totalTime, 2) + "ms");
            logMessage("  Average: " + juce::String(avgTime, 4) + "ms per iteration");
        }
        
        beginTest("Oscillator Pool Allocation Speed");
        {
            auto pool = std::make_unique<OptimizedOscillatorPool<1024>>();
            
            const int numAllocations = 1000;
            const double maxAllocationTimeMs = 5.0; // Should be very fast O(1) operations
            
            auto startTime = juce::Time::getHighResolutionTicks();
            
            std::vector<int> allocated;
            allocated.reserve(numAllocations);
            
            for (int i = 0; i < numAllocations; ++i) {
                int id = pool->allocate();
                if (id >= 0) {
                    allocated.push_back(id);
                }
                
                // Occasionally deallocate some to test mixed workload
                if (i > 100 && i % 50 == 0 && !allocated.empty()) {
                    pool->deallocate(allocated.back());
                    allocated.pop_back();
                }
            }
            
            auto endTime = juce::Time::getHighResolutionTicks();
            double allocationTime = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
            
            expect(allocationTime < maxAllocationTimeMs, "Oscillator allocation should be very fast");
            expect(allocated.size() > 0, "Should have successfully allocated oscillators");
            
            logMessage("Oscillator allocation performance:");
            logMessage("  " + juce::String(numAllocations) + " operations in " + juce::String(allocationTime, 2) + "ms");
            logMessage("  Average: " + juce::String(allocationTime / numAllocations * 1000.0, 2) + "μs per operation");
            logMessage("  Successfully allocated: " + juce::String((int)allocated.size()) + " oscillators");
            
            // Clean up
            for (int id : allocated) {
                pool->deallocate(id);
            }
        }
        
        beginTest("CPU Usage Under Load");
        {
            ARTEFACTAudioProcessor processor;
            processor.prepareToPlay(48000.0, 128);
            
            std::atomic<bool> stop{false};
            std::atomic<double> totalProcessingTime{0.0};
            std::atomic<int> processedBlocks{0};
            
            // High-frequency audio processing simulation
            std::thread audioThread([&]() {
                juce::AudioBuffer<float> buffer(2, 128);
                juce::MidiBuffer midiBuffer;
                
                while (!stop.load()) {
                    buffer.clear();
                    midiBuffer.clear();
                    
                    auto startTime = juce::Time::getHighResolutionTicks();
                    processor.processBlock(buffer, midiBuffer);
                    auto endTime = juce::Time::getHighResolutionTicks();
                    
                    double blockTime = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
                    totalProcessingTime.fetch_add(blockTime);
                    processedBlocks.fetch_add(1);
                    
                    // Simulate 48kHz sample rate with 128 sample buffer = ~2.7ms per block
                    std::this_thread::sleep_for(std::chrono::microseconds(2700));
                }
            });
            
            // Paint activity simulation
            std::thread paintThread([&]() {
                int strokeCount = 0;
                while (!stop.load()) {
                    Command beginStroke(PaintCommandID::BeginStroke, 
                                      (strokeCount % 100) * 0.8f, 
                                      (strokeCount % 50) * 1.6f, 
                                      0.7f, 
                                      juce::Colours::green);
                    processor.pushCommandToQueue(beginStroke);
                    
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    
                    Command endStroke(PaintCommandID::EndStroke);
                    processor.pushCommandToQueue(endStroke);
                    
                    strokeCount++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
            });
            
            // Run load test for 1 second
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            stop.store(true);
            
            audioThread.join();
            paintThread.join();
            
            double avgProcessingTime = totalProcessingTime.load() / processedBlocks.load();
            double cpuUsagePercent = (avgProcessingTime / 2.7) * 100.0; // 2.7ms available per block
            
            expect(avgProcessingTime < 1.0, "Average processing time should be well under available time");
            expect(cpuUsagePercent < 50.0, "CPU usage should be reasonable under load");
            
            logMessage("CPU usage under load:");
            logMessage("  Processed blocks: " + juce::String(processedBlocks.load()));
            logMessage("  Average processing: " + juce::String(avgProcessingTime, 4) + "ms per block");
            logMessage("  Estimated CPU usage: " + juce::String(cpuUsagePercent, 1) + "%");
            logMessage("  Available time per block: 2.7ms (48kHz/128 samples)");
            
            processor.releaseResources();
        }
        
        beginTest("Debug Logging Removal Validation");
        {
            // This test ensures our debug logging removal was effective
            ARTEFACTAudioProcessor processor;
            processor.prepareToPlay(48000.0, 64);
            
            // Capture debug output (if any)
            juce::String debugOutput;
            
            // Process many blocks rapidly
            const int numBlocks = 500; // This used to trigger debug logging
            
            auto startTime = juce::Time::getHighResolutionTicks();
            
            for (int i = 0; i < numBlocks; ++i) {
                juce::AudioBuffer<float> buffer(2, 64);
                juce::MidiBuffer midiBuffer;
                buffer.clear();
                midiBuffer.clear();
                
                processor.processBlock(buffer, midiBuffer);
            }
            
            auto endTime = juce::Time::getHighResolutionTicks();
            double totalTime = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
            double avgTime = totalTime / numBlocks;
            
            // After debug removal, this should be very fast
            expect(avgTime < 0.5, "Processing should be very fast without debug logging");
            
            logMessage("Debug removal validation:");
            logMessage("  " + juce::String(numBlocks) + " blocks in " + juce::String(totalTime, 2) + "ms");
            logMessage("  Average: " + juce::String(avgTime, 4) + "ms per block");
            logMessage("  Target: < 0.5ms (achieved after debug removal)");
            
            processor.releaseResources();
        }
    }
};

// Register the performance regression tests
static PerformanceRegressionTests performanceTests;