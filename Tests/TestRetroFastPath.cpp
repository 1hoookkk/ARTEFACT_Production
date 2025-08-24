//==============================================================================
// Tests/TestRetroFastPath.cpp
// Phase 2: Retro Fast Path Performance & Multi-Oscillator Validation
//==============================================================================

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include "../Source/Core/SpectralSynthEngine.h"
#include "../Source/Dev/AudioTrace.h"
#include <chrono>
#include <iostream>

//==============================================================================
class RetroFastPathTester 
{
public:
    void runTests()
    {
        std::cout << "=== TestRetroFastPath ===" << std::endl;
        std::cout << "Validating retro fast path performance & multi-oscillator functionality" << std::endl << std::endl;
        
        // Initialize engine
        SpectralSynthEngine engine;
        engine.prepareToPlay(48000.0, 512, 2);
        engine.enableRetroFastPath(true);
        
        // Test 1: Latency validation
        testPaintToAudioLatency(engine);
        
        // Test 2: Multi-oscillator functionality  
        testMultiOscillatorBank(engine);
        
        // Test 3: Color-to-spectral mapping
        testColorSpectralMapping(engine);
        
        // Test 4: Performance under load
        testPerformanceUnderLoad(engine);
        
        std::cout << std::endl << "=== TestRetroFastPath SUCCESSFUL ===" << std::endl;
    }

private:
    void testPaintToAudioLatency(SpectralSynthEngine& engine)
    {
        std::cout << "1. Testing paint-to-audio latency (<5ms requirement)..." << std::endl;
        
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate paint gesture
        engine.processPaintStrokeDirectToAudio(400.0f, 300.0f, 0.8f, juce::Colours::red);
        
        // Process audio block
        engine.processBlock(buffer);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        float latencyMs = latency.count() / 1000.0f;
        float bufferRms = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
        
        std::cout << "   Paint-to-audio latency: " << latencyMs << " ms" << std::endl;
        std::cout << "   Audio generated (RMS): " << bufferRms << std::endl;
        
        if (latencyMs > 5.0f) {
            std::cout << "   ⚠ WARNING: Latency exceeds 5ms requirement!" << std::endl;
        } else {
            std::cout << "   ✓ Latency requirement met" << std::endl;
        }
        
        if (bufferRms < 1e-4f) {
            throw std::runtime_error("No audio generated from paint gesture");
        }
        
        std::cout << "   ✓ Paint gesture produced audio" << std::endl;
    }
    
    void testMultiOscillatorBank(SpectralSynthEngine& engine)
    {
        std::cout << "2. Testing multi-oscillator bank functionality..." << std::endl;
        
        // Clear any existing oscillators
        engine.clearRetroOscillators();
        
        // Trigger multiple oscillators with different frequencies
        engine.triggerRetroOscillator(440.0f, 0.5f, 0.995f);  // A4
        engine.triggerRetroOscillator(880.0f, 0.3f, 0.992f);  // A5 harmonic
        engine.triggerRetroOscillator(1320.0f, 0.2f, 0.990f); // A6 harmonic
        
        int activeCount = engine.getActiveRetroOscillatorCount();
        std::cout << "   Active oscillators: " << activeCount << std::endl;
        
        if (activeCount != 3) {
            throw std::runtime_error("Expected 3 active oscillators, got " + std::to_string(activeCount));
        }
        
        // Process several blocks to test decay
        juce::AudioBuffer<float> buffer(2, 512);
        float initialRms = 0.0f;
        float finalRms = 0.0f;
        
        for (int block = 0; block < 10; ++block) {
            buffer.clear();
            engine.processBlock(buffer);
            
            float blockRms = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
            if (block == 0) initialRms = blockRms;
            if (block == 9) finalRms = blockRms;
        }
        
        std::cout << "   Initial RMS: " << initialRms << std::endl;
        std::cout << "   Final RMS: " << finalRms << std::endl;
        
        if (finalRms >= initialRms) {
            throw std::runtime_error("Expected amplitude decay over time");
        }
        
        std::cout << "   ✓ Multi-oscillator synthesis working" << std::endl;
        std::cout << "   ✓ Amplitude decay functioning" << std::endl;
    }
    
    void testColorSpectralMapping(SpectralSynthEngine& engine)
    {
        std::cout << "3. Testing color-to-spectral mapping..." << std::endl;
        
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Test different colors producing different spectral content
        struct ColorTest {
            juce::Colour color;
            const char* name;
        };
        
        ColorTest colors[] = {
            {juce::Colours::red, "red"},
            {juce::Colours::blue, "blue"}, 
            {juce::Colours::green, "green"},
            {juce::Colour::fromHSV(0.0f, 1.0f, 1.0f, 1.0f), "pure_red"},
            {juce::Colour::fromHSV(0.5f, 1.0f, 1.0f, 1.0f), "pure_cyan"}
        };
        
        for (const auto& test : colors) {
            engine.clearRetroOscillators();
            buffer.clear();
            
            engine.processPaintStrokeDirectToAudio(400.0f, 300.0f, 0.7f, test.color);
            engine.processBlock(buffer);
            
            int oscillators = engine.getActiveRetroOscillatorCount();
            float rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
            
            std::cout << "   " << test.name << ": " << oscillators 
                      << " oscillators, RMS=" << rms << std::endl;
            
            if (rms < 1e-5f) {
                throw std::runtime_error("No audio generated for color " + std::string(test.name));
            }
        }
        
        std::cout << "   ✓ Color-to-spectral mapping functional" << std::endl;
    }
    
    void testPerformanceUnderLoad(SpectralSynthEngine& engine)
    {
        std::cout << "4. Testing performance under load..." << std::endl;
        
        juce::AudioBuffer<float> buffer(2, 512);
        const int numGestures = 20;
        const int blocksPerTest = 100;
        
        // Fill oscillator bank to near capacity
        engine.clearRetroOscillators();
        for (int i = 0; i < numGestures; ++i) {
            float x = 100.0f + (i * 30.0f);
            float y = 200.0f + (i * 15.0f);
            float pressure = 0.5f + (i * 0.02f);
            juce::Colour color = juce::Colour::fromHSV(i / float(numGestures), 0.8f, 0.9f, 1.0f);
            
            engine.processPaintStrokeDirectToAudio(x, y, pressure, color);
        }
        
        int totalOscillators = engine.getActiveRetroOscillatorCount();
        std::cout << "   Created " << totalOscillators << " oscillators" << std::endl;
        
        // Measure processing time under load
        auto start = std::chrono::high_resolution_clock::now();
        
        float totalRms = 0.0f;
        for (int block = 0; block < blocksPerTest; ++block) {
            buffer.clear();
            engine.processBlock(buffer);
            totalRms += buffer.getRMSLevel(0, 0, buffer.getNumSamples());
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        float avgBlockTimeMs = (totalTime.count() / float(blocksPerTest)) / 1000.0f;
        float avgRms = totalRms / blocksPerTest;
        
        std::cout << "   Average block processing time: " << avgBlockTimeMs << " ms" << std::endl;
        std::cout << "   Average RMS level: " << avgRms << std::endl;
        std::cout << "   Final active oscillators: " << engine.getActiveRetroOscillatorCount() << std::endl;
        
        // Performance should be reasonable for real-time audio (< 10ms for 512 samples at 48kHz)
        if (avgBlockTimeMs > 10.0f) {
            std::cout << "   ⚠ WARNING: Processing time may be too high for real-time" << std::endl;
        } else {
            std::cout << "   ✓ Performance suitable for real-time processing" << std::endl;
        }
        
        if (avgRms < 1e-5f) {
            throw std::runtime_error("Audio output too low under load");
        }
        
        std::cout << "   ✓ Audio generation stable under load" << std::endl;
    }
};

//==============================================================================
int main()
{
    try {
        // Initialize JUCE
        juce::initialiseJuce_GUI();
        
        RetroFastPathTester tester;
        tester.runTests();
        
        juce::shutdownJuce_GUI();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "TEST FAILED: " << e.what() << std::endl;
        juce::shutdownJuce_GUI();
        return 1;
    }
}