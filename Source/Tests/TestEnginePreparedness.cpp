/**
 * TestEnginePreparedness.cpp
 * 
 * Unit test to verify all engines have proper isPrepared safety guards
 * preventing crashes when processBlock is called before prepareToPlay.
 * 
 * This test prevents regressions of the segfault issue that was fixed
 * by adding comprehensive safety guards to all audio engines.
 */

#include <JuceHeader.h>
#include "../Core/PaintEngineMinimal.h"
#include "../Core/ForgeProcessor.h"
#include "../Core/SampleMaskingEngine.h"
#include "../Core/AudioRecorder.h"
#include "../Core/TapeSpeed.h"
#include "../Core/StereoWidth.h"
#include "../Core/SpectralSynthEngine.h"

class TestEnginePreparedness
{
public:
    static int runTest()
    {
        std::cout << "=== Testing Engine Preparedness Guards ===" << std::endl;
        
        int failures = 0;
        
        failures += testPaintEnginePreparedness();
        failures += testForgeProcessorPreparedness();
        failures += testSampleMaskingEnginePreparedness();
        failures += testAudioRecorderPreparedness();
        failures += testTapeSpeedPreparedness();
        failures += testStereoWidthPreparedness();
        failures += testSpectralSynthEnginePreparedness();
        
        if (failures == 0)
        {
            std::cout << "✅ All engine preparedness tests passed!" << std::endl;
        }
        else
        {
            std::cout << "❌ " << failures << " engine preparedness tests failed!" << std::endl;
        }
        
        return failures;
    }
    
private:
    static int testPaintEnginePreparedness()
    {
        std::cout << "Testing PaintEngine preparedness..." << std::endl;
        
        PaintEngine engine;
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Test 1: Should not be prepared initially
        if (engine.prepared())
        {
            std::cout << "❌ PaintEngine should not be prepared initially" << std::endl;
            return 1;
        }
        
        // Test 2: processBlock should be safe when unprepared (no crash)
        engine.processBlock(buffer);  // Should not crash
        
        // Test 3: Should be prepared after prepareToPlay
        engine.prepareToPlay(44100.0, 512);
        if (!engine.prepared())
        {
            std::cout << "❌ PaintEngine should be prepared after prepareToPlay" << std::endl;
            return 1;
        }
        
        // Test 4: processBlock should work when prepared
        engine.processBlock(buffer);  // Should work fine
        
        // Test 5: Should not be prepared after releaseResources
        engine.releaseResources();
        if (engine.prepared())
        {
            std::cout << "❌ PaintEngine should not be prepared after releaseResources" << std::endl;
            return 1;
        }
        
        std::cout << "✅ PaintEngine preparedness tests passed" << std::endl;
        return 0;
    }
    
    static int testForgeProcessorPreparedness()
    {
        std::cout << "Testing ForgeProcessor preparedness..." << std::endl;
        
        ForgeProcessor processor;
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        
        // Test 1: Should not be prepared initially
        if (processor.prepared())
        {
            std::cout << "❌ ForgeProcessor should not be prepared initially" << std::endl;
            return 1;
        }
        
        // Test 2: processBlock should be safe when unprepared (no crash)
        processor.processBlock(buffer, midiBuffer);  // Should not crash
        
        // Test 3: Should be prepared after prepareToPlay
        processor.prepareToPlay(44100.0, 512);
        if (!processor.prepared())
        {
            std::cout << "❌ ForgeProcessor should be prepared after prepareToPlay" << std::endl;
            return 1;
        }
        
        // Test 4: processBlock should work when prepared
        processor.processBlock(buffer, midiBuffer);  // Should work fine
        
        std::cout << "✅ ForgeProcessor preparedness tests passed" << std::endl;
        return 0;
    }
    
    static int testSampleMaskingEnginePreparedness()
    {
        std::cout << "Testing SampleMaskingEngine preparedness..." << std::endl;
        
        SampleMaskingEngine engine;
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Test 1: Should not be prepared initially
        if (engine.prepared())
        {
            std::cout << "❌ SampleMaskingEngine should not be prepared initially" << std::endl;
            return 1;
        }
        
        // Test 2: processBlock should be safe when unprepared (no crash)
        engine.processBlock(buffer);  // Should not crash
        
        // Test 3: Should be prepared after prepareToPlay
        engine.prepareToPlay(44100.0, 512, 2);
        if (!engine.prepared())
        {
            std::cout << "❌ SampleMaskingEngine should be prepared after prepareToPlay" << std::endl;
            return 1;
        }
        
        // Test 4: processBlock should work when prepared
        engine.processBlock(buffer);  // Should work fine
        
        std::cout << "✅ SampleMaskingEngine preparedness tests passed" << std::endl;
        return 0;
    }
    
    static int testAudioRecorderPreparedness()
    {
        std::cout << "Testing AudioRecorder preparedness..." << std::endl;
        
        AudioRecorder recorder;
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Test 1: Should not be prepared initially
        if (recorder.prepared())
        {
            std::cout << "❌ AudioRecorder should not be prepared initially" << std::endl;
            return 1;
        }
        
        // Test 2: processBlock should be safe when unprepared (no crash)
        recorder.processBlock(buffer);  // Should not crash
        
        // Test 3: Should be prepared after prepareToPlay
        recorder.prepareToPlay(44100.0, 512, 2);
        if (!recorder.prepared())
        {
            std::cout << "❌ AudioRecorder should be prepared after prepareToPlay" << std::endl;
            return 1;
        }
        
        // Test 4: processBlock should work when prepared
        recorder.processBlock(buffer);  // Should work fine
        
        std::cout << "✅ AudioRecorder preparedness tests passed" << std::endl;
        return 0;
    }
    
    static int testTapeSpeedPreparedness()
    {
        std::cout << "Testing TapeSpeed preparedness..." << std::endl;
        
        TapeSpeed processor;
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Test 1: Should not be prepared initially
        if (processor.prepared())
        {
            std::cout << "❌ TapeSpeed should not be prepared initially" << std::endl;
            return 1;
        }
        
        // Test 2: processBlock should be safe when unprepared (no crash)
        processor.processBlock(buffer);  // Should not crash
        
        // Test 3: Should be prepared after prepareToPlay
        processor.prepareToPlay(44100.0, 512);
        if (!processor.prepared())
        {
            std::cout << "❌ TapeSpeed should be prepared after prepareToPlay" << std::endl;
            return 1;
        }
        
        // Test 4: processBlock should work when prepared
        processor.processBlock(buffer);  // Should work fine
        
        std::cout << "✅ TapeSpeed preparedness tests passed" << std::endl;
        return 0;
    }
    
    static int testStereoWidthPreparedness()
    {
        std::cout << "Testing StereoWidth preparedness..." << std::endl;
        
        StereoWidth processor;
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Test 1: Should not be prepared initially
        if (processor.prepared())
        {
            std::cout << "❌ StereoWidth should not be prepared initially" << std::endl;
            return 1;
        }
        
        // Test 2: processBlock should be safe when unprepared (no crash)
        processor.processBlock(buffer);  // Should not crash
        
        // Test 3: Should be prepared after prepareToPlay
        processor.prepareToPlay(44100.0, 512);
        if (!processor.prepared())
        {
            std::cout << "❌ StereoWidth should be prepared after prepareToPlay" << std::endl;
            return 1;
        }
        
        // Test 4: processBlock should work when prepared
        processor.processBlock(buffer);  // Should work fine
        
        std::cout << "✅ StereoWidth preparedness tests passed" << std::endl;
        return 0;
    }
    
    static int testSpectralSynthEnginePreparedness()
    {
        std::cout << "Testing SpectralSynthEngine preparedness..." << std::endl;
        
        SpectralSynthEngine engine;
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Test 1: processPaintStrokeDirectToAudio should be safe when unprepared (no crash)
        engine.processPaintStrokeDirectToAudio(0.5f, 0.5f, 1.0f, juce::Colours::white);  // Should not crash
        
        // Test 2: Should work after prepareToPlay
        engine.prepareToPlay(44100.0, 512, 2);
        
        // Test 3: processPaintStrokeDirectToAudio should work when prepared
        engine.processPaintStrokeDirectToAudio(0.5f, 0.5f, 1.0f, juce::Colours::white);  // Should work fine
        
        // Test 4: processBlock should work when prepared
        engine.processBlock(buffer);  // Should work fine
        
        std::cout << "✅ SpectralSynthEngine preparedness tests passed" << std::endl;
        return 0;
    }
};

// Test entry point
int main()
{
    return TestEnginePreparedness::runTest();
}