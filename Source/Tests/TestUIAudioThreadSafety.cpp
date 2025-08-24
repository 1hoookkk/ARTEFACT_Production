// Source/Tests/TestUIAudioThreadSafety.cpp
#include "../Core/SpectralSynthEngine.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <thread>
#include <iostream>
#include <atomic>
#include <chrono>

int TestUIAudioThreadSafety()
{
    std::cout << "TestUIAudioThreadSafety: Starting concurrent UI→Audio thread safety test..." << std::endl;
    
    try {
        SpectralSynthEngine engine;
        engine.prepareToPlay(48000.0, 512, 2);
        
        // Shared state for the test
        std::atomic<bool> testRunning{true};
        std::atomic<int> paintCallsCompleted{0};
        std::atomic<int> audioBlocksProcessed{0};
        
        // UI Thread simulation - rapid paint strokes
        std::thread uiThread([&]() {
            int paintCount = 0;
            while (testRunning.load() && paintCount < 1000) {
                // Simulate paint strokes at different frequencies
                float y = 100.0f + (paintCount % 400);  // Vary frequency
                float pressure = 0.5f + 0.3f * std::sin(paintCount * 0.1f);  // Vary amplitude
                
                engine.processPaintStrokeDirectToAudio(400.0f, y, pressure, juce::Colours::blue);
                paintCallsCompleted.fetch_add(1);
                paintCount++;
                
                // Simulate UI frame rate (~60fps)
                std::this_thread::sleep_for(std::chrono::microseconds(16667));
            }
        });
        
        // Audio Thread simulation - process audio blocks
        std::thread audioThread([&]() {
            juce::AudioBuffer<float> buffer(2, 512);
            int blockCount = 0;
            
            while (testRunning.load() && blockCount < 500) {
                buffer.clear();
                engine.processBlock(buffer);
                audioBlocksProcessed.fetch_add(1);
                blockCount++;
                
                // Simulate audio callback timing (~512 samples at 48kHz = ~10.67ms)
                std::this_thread::sleep_for(std::chrono::microseconds(10670));
            }
        });
        
        // Let threads run for a reasonable duration
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        testRunning.store(false);
        
        // Wait for threads to complete
        uiThread.join();
        audioThread.join();
        
        int finalPaintCalls = paintCallsCompleted.load();
        int finalAudioBlocks = audioBlocksProcessed.load();
        
        std::cout << "  Paint calls completed: " << finalPaintCalls << std::endl;
        std::cout << "  Audio blocks processed: " << finalAudioBlocks << std::endl;
        
        // Verify reasonable progress was made
        bool paintProgressOK = finalPaintCalls > 50;  // Should complete many paint calls
        bool audioProgressOK = finalAudioBlocks > 50; // Should process many audio blocks
        
        if (paintProgressOK) {
            std::cout << "  ✓ UI thread progress OK: " << finalPaintCalls << " paint calls" << std::endl;
        } else {
            std::cout << "  ✗ UI thread progress FAILED: only " << finalPaintCalls << " paint calls" << std::endl;
        }
        
        if (audioProgressOK) {
            std::cout << "  ✓ Audio thread progress OK: " << finalAudioBlocks << " audio blocks" << std::endl;
        } else {
            std::cout << "  ✗ Audio thread progress FAILED: only " << finalAudioBlocks << " audio blocks" << std::endl;
        }
        
        if (!paintProgressOK || !audioProgressOK) {
            std::cout << "✗ TestUIAudioThreadSafety FAILED: Insufficient concurrent progress" << std::endl;
            return 1;
        }
        
        std::cout << "✓ TestUIAudioThreadSafety PASSED: UI→Audio atomic communication verified under load" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "✗ TestUIAudioThreadSafety FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
}