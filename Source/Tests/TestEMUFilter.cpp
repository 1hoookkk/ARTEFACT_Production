#include "TestEMUFilter.h"
#include <iostream>

// Implement the test function referenced in RunAllTests.cpp
int TestEMUFilter()
{
    std::cout << "Running EMUFilter attenuation test..." << std::endl;
    
    try {
        // Execute the test from the header
        // This will fail initially because EMUFilter is passthrough
        
        // 1. Arrange: Set up the test conditions
        const double sampleRate = 48000.0;
        const int numSamples = 1 << 12; // 4096 samples
        const float sineFreq = 1000.0f;
        const float filterCutoff = 200.0f;
        const float expectedAttenuationDb = -20.0f; // Expect at least 20dB of attenuation

        // Create the filter
        EMUFilter filter;
        filter.prepare(sampleRate, numSamples);
        filter.setParams(filterCutoff, 0.707f); // Cutoff, Resonance

        // Create a 1kHz sine wave test signal
        juce::AudioBuffer<float> buffer(1, numSamples);
        float phase = 0.0f;
        const float phaseIncrement = (sineFreq * juce::MathConstants<float>::twoPi) / (float)sampleRate;
        for (int i = 0; i < numSamples; ++i)
        {
            buffer.setSample(0, i, std::sin(phase));
            phase += phaseIncrement;
        }

        // 2. Act: Process the signal through the filter
        filter.processBlock(buffer);

        // 3. Assert: Use FFT to verify the signal was attenuated
        juce::dsp::FFT fft(12); // 2^12 = 4096
        juce::HeapBlock<float> fftData(2 * numSamples);
        
        // Copy buffer data to FFT buffer and clear the imaginary part
        for (int i = 0; i < numSamples; ++i)
        {
            fftData[i] = buffer.getSample(0, i);
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
        
        const float magnitudeDb = TestHelpers::toDecibels(magnitude);
        
        std::cout << "  - Input frequency: " << sineFreq << " Hz" << std::endl;
        std::cout << "  - Filter cutoff: " << filterCutoff << " Hz" << std::endl;
        std::cout << "  - Output magnitude: " << magnitudeDb << " dB" << std::endl;
        std::cout << "  - Expected attenuation: < " << expectedAttenuationDb << " dB" << std::endl;

        // Test should fail initially (passthrough = no attenuation)
        if (magnitudeDb < expectedAttenuationDb)
        {
            std::cout << "✓ EMUFilter test PASSED - sufficient attenuation achieved" << std::endl;
            return 0;
        }
        else
        {
            std::cout << "✗ EMUFilter test FAILED - insufficient attenuation (expected)" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "✗ EMUFilter test FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
}