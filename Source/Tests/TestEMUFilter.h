#pragma once
#include "tdd-guard.h"

// This test requires some advanced helpers that we define here for now.
#include <juce_dsp/juce_dsp.h>
#include "Core/EMUFilter.h" // This file does not exist yet. This will make the build fail.

namespace TestHelpers
{
    // Helper to convert linear magnitude to decibels
    static float toDecibels(float linear)
    {
        return 20.0f * std::log10(std::max(linear, 1e-9f));
    }
}

TEST(EMUFilter, AttenuatesSignalAboveCutoff)
{
    // 1. Arrange: Set up the test conditions
    const double sampleRate = 48000.0;
    const int numSamples = 1 << 12; // 4096 samples
    const float sineFreq = 1000.0f;
    const float filterCutoff = 200.0f;
    const float expectedAttenuationDb = -20.0f; // Expect at least 20dB of attenuation

    // Create the filter (this class doesn't exist yet, so this will fail)
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
    buffer.copyFrom(0, 0, fftData.getData(), numSamples);
    fft.performRealOnlyForwardTransform(fftData.getData());

    // Find the magnitude at the sine wave's frequency bin
    const int targetBin = static_cast<int>(sineFreq * numSamples / sampleRate);
    const float real = fftData[targetBin * 2];
    const float imag = fftData[targetBin * 2 + 1];
    const float magnitude = std::sqrt(real * real + imag * imag) / (float)(numSamples / 2);

    ASSERT_LT(TestHelpers::toDecibels(magnitude), expectedAttenuationDb);
}