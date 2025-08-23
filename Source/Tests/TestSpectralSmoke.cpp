#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <iostream>

// Simple smoke test for TDD guard bootstrap
// Tests basic JUCE functionality without complex engine dependencies
int TestSpectralSmoke() {
    std::cout << "SpectralSmoke: Starting basic JUCE test..." << std::endl;
    
    try {
        // Test 1: JUCE initialization
        std::cout << "✓ JUCE headers included successfully" << std::endl;
        
        // Test 2: AudioBuffer creation
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        std::cout << "✓ AudioBuffer created successfully" << std::endl;
        
        // Test 3: Basic buffer operations
        buffer.setSample(0, 0, 0.5f);
        float sample = buffer.getSample(0, 0);
        
        if (std::abs(sample - 0.5f) < 0.001f) {
            std::cout << "✓ AudioBuffer operations working" << std::endl;
        } else {
            std::cout << "✗ AudioBuffer operations failed" << std::endl;
            return 1;
        }
        
        // Test 4: All tests passed - GREEN state achieved
        std::cout << "✓ All basic JUCE tests passed successfully" << std::endl;
        return 0; // GREEN - tests pass
        
    } catch (const std::exception& e) {
        std::cout << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}