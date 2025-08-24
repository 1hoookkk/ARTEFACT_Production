#include <iostream>
#include "../Core/EMUFilter.h"
#include <juce_dsp/juce_dsp.h>

// Helper to convert linear magnitude to decibels
static float toDecibels(float linear)
{
    return 20.0f * std::log10(std::max(linear, 1e-9f));
}

// Helper to build test sine wave
static juce::AudioBuffer<float> buildTestSine(double sampleRate, int numSamples, float frequency)
{
    juce::AudioBuffer<float> buffer(1, numSamples);
    float phase = 0.0f;
    const float phaseIncrement = (frequency * juce::MathConstants<float>::twoPi) / (float)sampleRate;
    
    for (int i = 0; i < numSamples; ++i)
    {
        buffer.setSample(0, i, std::sin(phase));
        phase += phaseIncrement;
    }
    
    return buffer;
}

// Helper to calculate RMS
static float calculateRMS(const juce::AudioBuffer<float>& buffer)
{
    float sum = 0.0f;
    const int numSamples = buffer.getNumSamples();
    const float* samples = buffer.getReadPointer(0);
    
    for (int i = 0; i < numSamples; ++i)
    {
        sum += samples[i] * samples[i];
    }
    
    return std::sqrt(sum / numSamples);
}

// Basic compilation and instantiation test
static bool testBasicInstantiation()
{
    try {
        // Basic instantiation test
        EMUFilter filter;
        filter.prepare(48000.0, 512);
        filter.setParams(1000.0f, 0.707f);
        
        // Test with a small buffer
        juce::AudioBuffer<float> buffer(1, 10);
        buffer.clear();
        filter.processBlock(buffer);
        
        // Should have expected parameter values
        if (filter.getCutoffHz() != 1000.0f) {
            std::cout << "FAIL: Expected cutoff 1000Hz, got " << filter.getCutoffHz() << std::endl;
            return false;
        }
        if (filter.getQ() != 0.707f) {
            std::cout << "FAIL: Expected Q 0.707, got " << filter.getQ() << std::endl;
            return false;
        }
        
        std::cout << "PASS: Basic instantiation and parameter setting" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: Exception in basic instantiation: " << e.what() << std::endl;
        return false;
    }
}

// FFT-based attenuation test using the original logic
static bool testFFTAttenuation()
{
    try {
        const double sampleRate = 48000.0;
        const int numSamples = 1 << 12; // 4096 samples
        const float sineFreq = 1000.0f;
        const float filterCutoff = 200.0f;
        const float expectedAttenuationDb = -6.0f; // Realistic for 2-pole filter at this frequency ratio

        // Reset filter for new test
        EMUFilter testFilter;
        testFilter.prepare(sampleRate, numSamples);
        testFilter.setParams(filterCutoff, 0.707f);

        // Create a 1kHz sine wave test signal
        juce::AudioBuffer<float> testBuffer(1, numSamples);
        float phase = 0.0f;
        const float phaseIncrement = (sineFreq * juce::MathConstants<float>::twoPi) / (float)sampleRate;
        for (int i = 0; i < numSamples; ++i)
        {
            testBuffer.setSample(0, i, std::sin(phase));
            phase += phaseIncrement;
        }

        // Process the signal through the filter
        testFilter.processBlock(testBuffer);

        // Use FFT to verify the signal was attenuated
        juce::dsp::FFT fft(12); // 2^12 = 4096
        juce::HeapBlock<float> fftData(2 * numSamples);
        
        // Copy buffer data to FFT buffer and clear the imaginary part
        for (int i = 0; i < numSamples; ++i)
        {
            fftData[i] = testBuffer.getSample(0, i);
        }
        for (int i = numSamples; i < 2 * numSamples; ++i)
        {
            fftData[i] = 0.0f;
        }
        
        fft.performRealOnlyForwardTransform(fftData.getData());

        // Find the magnitude at the sine wave's frequency bin
        const int targetBin = static_cast<int>(sineFreq * numSamples / sampleRate);
        const float real = fftData[targetBin * 2];
        const float imag = fftData[targetBin * 2 + 1];
        const float magnitude = std::sqrt(real * real + imag * imag) / (float)(numSamples / 2);
        
        const float magnitudeDb = toDecibels(magnitude);
        
        // Assert that attenuation is sufficient
        if (magnitudeDb >= expectedAttenuationDb) {
            std::cout << "FAIL: Expected attenuation < " << expectedAttenuationDb << " dB, got " << magnitudeDb << " dB" << std::endl;
            return false;
        }
        
        std::cout << "PASS: FFT attenuation test (got " << magnitudeDb << " dB attenuation)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: Exception in FFT attenuation test: " << e.what() << std::endl;
        return false;
    }
}

// RED Test: Precise tolerance-based EMUFilter response test
static bool testDeterministicAttenuation()
{
    try {
        // Test configuration - expect this to FAIL initially (RED phase)
        const double sampleRate = 48000.0;
        const int numSamples = 4096;
        const float testFreq = 1000.0f;     // Test frequency  
        const float cutoffFreq = 200.0f;    // Filter cutoff
        const float resonance = 0.707f;     // Butterworth Q
        const float expectedAttenuationDb = 18.0f;  // Expected ≥18 dB attenuation at 1kHz with 200Hz cutoff
        
        // Build 1 kHz test sine wave at 48 kHz sample rate
        auto testBuffer = buildTestSine(sampleRate, numSamples, testFreq);
        
        // Measure RMS before filtering (should be ~0.707 for unit sine)
        float rmsBefore = calculateRMS(testBuffer);
        
        // Configure and apply EMUFilter with 200 Hz cutoff, Q=0.707
        EMUFilter filter;
        filter.prepare(sampleRate, numSamples);
        filter.setParams(cutoffFreq, resonance);
        
        // No allocations on audio thread within test harness - processBlock is RT-safe
        filter.processBlock(testBuffer);
        
        // Measure RMS after filtering
        float rmsAfter = calculateRMS(testBuffer);
        
        // Calculate actual attenuation in dB
        float attenuationRatio = rmsAfter / rmsBefore;
        float actualAttenuationDb = -toDecibels(attenuationRatio);  // Negative because we expect attenuation
        
        // This test is designed to FAIL initially (RED phase)
        // A 2-pole low-pass filter at 200 Hz should attenuate 1 kHz by ≥18 dB
        if (actualAttenuationDb < expectedAttenuationDb) {
            std::cout << "FAIL (EXPECTED): Attenuation " << actualAttenuationDb << " dB < expected " << expectedAttenuationDb << " dB (RED phase)" << std::endl;
            return false;  // Expected failure for RED phase
        }
        
        std::cout << "PASS: Deterministic attenuation test (got " << actualAttenuationDb << " dB)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: Exception in deterministic attenuation test: " << e.what() << std::endl;
        return false;
    }
}

// Additional RED Test: RMS-based precise attenuation measurement
static bool testPreciseToleranceAttenuation()
{
    try {
        // This is a deterministic test designed to FAIL in RED phase
        // to verify filter behavior before implementation
        const double sampleRate = 48000.0;
        const int numSamples = 4096;
        const float testFreq = 1000.0f;     // Test frequency: 1 kHz
        const float cutoffFreq = 200.0f;    // Filter cutoff: 200 Hz
        const float resonance = 0.707f;     // Butterworth Q (critical damping)
        
        // Expected attenuation for 2-pole low-pass at 5x cutoff frequency
        // Theoretical: 20*log10(1/sqrt(1+(f/fc)^(2n))) = 20*log10(1/sqrt(1+25^4)) ≈ -28 dB
        // Practical expectation for TPT implementation: ≥18 dB
        const float expectedMinAttenuationDb = 18.0f;
        
        // Build deterministic test sine wave using buildTestSine helper
        auto testBuffer = buildTestSine(sampleRate, numSamples, testFreq);
        
        // Measure RMS before filtering (reference level)
        float rmsBefore = calculateRMS(testBuffer);
        
        // Initialize and configure EMUFilter with correct API
        EMUFilter filter;
        filter.prepare(sampleRate, numSamples);  // Prepare with sample rate and block size
        filter.setParams(cutoffFreq, resonance); // Set cutoff=200Hz, Q=0.707
        
        // Apply filter in-place (no allocations on audio thread)
        filter.processBlock(testBuffer);
        
        // Measure RMS after filtering
        float rmsAfter = calculateRMS(testBuffer);
        
        // Calculate attenuation ratio and convert to dB
        float attenuationRatio = rmsAfter / rmsBefore;
        float actualAttenuationDb = -toDecibels(attenuationRatio);
        
        // This assertion SHOULD FAIL initially (RED phase) 
        // The current EMUFilter may not provide sufficient attenuation at 1kHz
        if (actualAttenuationDb < expectedMinAttenuationDb) {
            std::cout << "FAIL (EXPECTED): Precise tolerance attenuation " << actualAttenuationDb << " dB < expected " << expectedMinAttenuationDb << " dB (RED phase)" << std::endl;
            return false;  // Expected failure for RED phase
        }
        
        std::cout << "PASS: Precise tolerance attenuation test (got " << actualAttenuationDb << " dB)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: Exception in precise tolerance attenuation test: " << e.what() << std::endl;
        return false;
    }
}

// Main test runner function called by central test system
int TestEMUFilterMinimal()
{
    std::cout << "========================================" << std::endl;
    std::cout << "EMUFilter Minimal Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Run basic instantiation test
    std::cout << "\nTest 1: Basic Instantiation" << std::endl;
    if (testBasicInstantiation()) {
        passed++;
    } else {
        failed++;
    }
    
    // Run FFT attenuation test
    std::cout << "\nTest 2: FFT Attenuation" << std::endl;
    if (testFFTAttenuation()) {
        passed++;
    } else {
        failed++;
    }
    
    // Run deterministic attenuation test (RED - expect failure)
    std::cout << "\nTest 3: Deterministic Attenuation (RED test)" << std::endl;
    if (testDeterministicAttenuation()) {
        passed++;
    } else {
        failed++;
    }
    
    // Run precise tolerance attenuation test (RED - expect failure)
    std::cout << "\nTest 4: Precise Tolerance Attenuation (RED test)" << std::endl;
    if (testPreciseToleranceAttenuation()) {
        passed++;
    } else {
        failed++;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Summary:" << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    std::cout << "========================================" << std::endl;
    
    return failed;  // Return number of failures (0 = all passed)
}