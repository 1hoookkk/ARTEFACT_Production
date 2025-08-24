#include <gtest/gtest.h>
#include "../../Core/AtomicOscillator.h"

// Test basic initialization
TEST(AtomicOscillatorTest, Initialization) {
    AtomicOscillator osc;
    EXPECT_NO_THROW(osc.setSampleRate(44100.0f));
}

// Test parameter updates
TEST(AtomicOscillatorTest, ParameterUpdates) {
    AtomicOscillator osc;
    osc.setSampleRate(44100.0f);
    
    EXPECT_NO_THROW(osc.setFrequency(440.0f));
    EXPECT_NEAR(osc.getFrequency(), 440.0f, 0.001f);
}

// Test denormal suppression
TEST(AtomicOscillatorTest, DenormalSuppression) {
    AtomicOscillator osc;
    osc.setSampleRate(44100.0f);
    osc.setFrequency(0.0f);  // Should not cause denormals
    
    float output = 0.0f;
    EXPECT_NO_THROW(output = osc.processSample());
    EXPECT_TRUE(std::isfinite(output));
}

// Test phase coherence
TEST(AtomicOscillatorTest, PhaseCoherence) {
    AtomicOscillator osc;
    osc.setSampleRate(44100.0f);
    osc.setFrequency(440.0f);
    
    // Process several samples and ensure output remains stable
    for (int i = 0; i < 1024; ++i) {
        float sample = osc.processSample();
        EXPECT_TRUE(std::isfinite(sample));
        EXPECT_GE(sample, -1.1f);  // Allow some headroom
        EXPECT_LE(sample, 1.1f);
    }
}

// Test frequency stability
TEST(AtomicOscillatorTest, FrequencyStability) {
    AtomicOscillator osc;
    osc.setSampleRate(44100.0f);
    
    // Test various frequency ranges
    std::vector<float> frequencies = {20.0f, 440.0f, 1000.0f, 8000.0f};
    
    for (float freq : frequencies) {
        osc.setFrequency(freq);
        EXPECT_NEAR(osc.getFrequency(), freq, 0.001f);
        
        // Ensure oscillator remains stable at this frequency
        for (int i = 0; i < 100; ++i) {
            float sample = osc.processSample();
            EXPECT_TRUE(std::isfinite(sample));
        }
    }
}

// Test thread-safety basics (compile-time check)
TEST(AtomicOscillatorTest, ThreadSafetyCompilation) {
    AtomicOscillator osc;
    osc.setSampleRate(44100.0f);
    
    // These calls should be atomic/thread-safe
    osc.setFrequency(440.0f);
    float freq = osc.getFrequency();
    EXPECT_NEAR(freq, 440.0f, 0.001f);
    
    // Process sample should be safe to call from audio thread
    float sample = osc.processSample();
    EXPECT_TRUE(std::isfinite(sample));
}