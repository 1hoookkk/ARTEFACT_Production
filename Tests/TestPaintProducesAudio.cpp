// Tests/TestPaintProducesAudio.cpp
// CRITICAL TEST: Verify paint gestures produce audible sound output
// Target: RMS > 1e-4 after paint gesture simulation

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include "../Source/Core/PluginProcessor.h"
#include "../Source/CanvasComponent.h"
#include "../Source/Core/PaintQueue.h"
#include <iostream>
#include <cassert>

// Define plugin constants needed for standalone test
#define JucePlugin_Name "SpectralCanvas Pro"
#define JucePlugin_VersionString "1.0.0"

int main()
{
    std::cout << "\n=== TestPaintProducesAudio ===\n";
    std::cout << "Testing paint gesture → audio generation pipeline\n\n";

    try {
        // Initialize JUCE
        juce::MessageManager::getInstance();
        
        // 1. Create plugin processor (our main audio engine)
        std::cout << "1. Creating ARTEFACTAudioProcessor...\n";
        ARTEFACTAudioProcessor processor;
        
        // 2. Initialize audio system
        std::cout << "2. Preparing audio processing...\n";
        const double sampleRate = 44100.0;
        const int samplesPerBlock = 512;
        
        processor.prepareToPlay(sampleRate, samplesPerBlock);
        
        // 3. Create test audio buffer
        std::cout << "3. Creating test buffer...\n";
        juce::AudioBuffer<float> testBuffer(2, samplesPerBlock);
        juce::MidiBuffer midiBuffer;
        
        // 4. Test emergency mode first (should produce guaranteed audio)
        std::cout << "4. Testing emergency test tone mode...\n";
        processor.enableEmergencyTestMode(true);
        processor.setTestToneAmplitude(0.3f); // Clear audible level
        
        // Process one block with emergency test tone
        testBuffer.clear();
        processor.processBlock(testBuffer, midiBuffer);
        
        // Check RMS of emergency test tone
        float emergencyRMS = testBuffer.getRMSLevel(0, 0, samplesPerBlock);
        std::cout << "   Emergency test tone RMS: " << emergencyRMS << "\n";
        
        if (emergencyRMS > 1e-4f) {
            std::cout << "   ✓ Emergency test tone PASS (RMS > 1e-4)\n";
        } else {
            std::cout << "   ✗ Emergency test tone FAIL (RMS too low)\n";
            return 1;
        }
        
        // Disable emergency mode for paint testing
        processor.enableEmergencyTestMode(false);
        
        // 5. Test paint gesture pipeline
        std::cout << "5. Testing paint gesture pipeline...\n";
        
        // Simulate paint gesture: middle of canvas, high pressure
        auto& paintQueue = processor.getPaintQueue();
        
        PaintGesture testGesture;
        testGesture.x = 400.0f;        // Middle of 800px canvas
        testGesture.y = 200.0f;        // Upper portion (high frequency)
        testGesture.pressure = 0.8f;   // Strong pressure
        testGesture.ticks = juce::Time::getHighResolutionTicks();
        
        // Push paint gesture into queue
        paintQueue.forcePush(testGesture);
        std::cout << "   Paint gesture pushed to queue\n";
        
        // 6. Process audio block with paint gesture
        std::cout << "6. Processing audio block with paint gesture...\n";
        testBuffer.clear();
        processor.processBlock(testBuffer, midiBuffer);
        
        // Calculate RMS to detect audio generation
        float paintRMS = 0.0f;
        for (int ch = 0; ch < testBuffer.getNumChannels(); ++ch) {
            float channelRMS = testBuffer.getRMSLevel(ch, 0, samplesPerBlock);
            paintRMS = juce::jmax(paintRMS, channelRMS);
        }
        
        std::cout << "   Paint audio RMS: " << paintRMS << "\n";
        
        // 7. Validate results
        std::cout << "7. Validating results...\n";
        
        if (paintRMS > 1e-4f) {
            std::cout << "   ✓ PASS: Paint gesture produced audio (RMS = " << paintRMS << ")\n";
            std::cout << "\n=== TestPaintProducesAudio SUCCESSFUL ===\n";
            return 0;
        } else {
            std::cout << "   ✗ FAIL: Paint gesture produced no audio (RMS = " << paintRMS << ")\n";
            std::cout << "\n=== Debugging Information ===\n";
            
            // Debug: Check if gesture reached audio thread
            std::cout << "Paint queue empty after processing: " << paintQueue.empty() << "\n";
            std::cout << "Processor sample rate: " << processor.getSampleRate() << "\n";
            
            std::cout << "\n=== TestPaintProducesAudio FAILED ===\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "Unknown exception caught\n";
        return 1;
    }
}