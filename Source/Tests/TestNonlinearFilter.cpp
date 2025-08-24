#include <iostream>
#include "../Core/EMUFilter.h"
#include <juce_dsp/juce_dsp.h>

// Helper to convert linear magnitude to decibels
static float toDecibels(float linear)
{
    return 20.0f * std::log10(std::max(linear, 1e-9f));
}

// Harmonic generation test - should FAIL initially because EMUFilter doesn't have nonlinear capability yet
int TestNonlinearFilter()
{
    std::cout << "Testing EMUFilter nonlinear harmonic generation..." << std::endl;
    
    try {
        const double sampleRate = 48000.0;
        const int numSamples = 8192; // Power of 2 for FFT
        const float fundamentalFreq = 1000.0f;
        const float filterCutoff = 5000.0f; // Well above fundamental
        const float amp = juce::Decibels::decibelsToGain(-12.0f);
        
        // Create test signal - 1kHz sine wave
        juce::AudioBuffer<float> inputBuffer(1, numSamples);
        float* input = inputBuffer.getWritePointer(0);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float phase = (fundamentalFreq * juce::MathConstants<float>::twoPi * (float)i) / (float)sampleRate;
            input[i] = amp * std::sin(phase);
        }
        
        // Test helper function to run filter with specific drive setting
        auto runFilter = [&](float driveDb) -> juce::AudioBuffer<float>
        {
            EMUFilter filter;
            filter.prepare(sampleRate, numSamples);
            filter.setParams(filterCutoff, 0.707f); // This will FAIL - no drive parameter yet
            
            // Warmup filter (let transients settle)
            for (int i = 0; i < 1024; ++i)
            {
                filter.processBlock(juce::AudioBuffer<float>(1, 1));
            }
            
            juce::AudioBuffer<float> output(1, numSamples);
            output.copyFrom(0, 0, inputBuffer, 0, 0, numSamples);
            filter.processBlock(output);
            
            return output;
        };
        
        // Get linear and nonlinear responses
        auto linearOutput = runFilter(0.0f);      // Linear (no drive)
        auto nonlinearOutput = runFilter(12.0f);  // +12dB drive
        
        // FFT analysis helper
        auto analyzeSpectrum = [&](const juce::AudioBuffer<float>& buffer) -> std::function<float(float)>
        {
            juce::dsp::FFT fft(juce::ilog2(numSamples));
            juce::HeapBlock<float> fftData(numSamples * 2);
            
            // Apply Hann window and copy to FFT buffer
            const float* signal = buffer.getReadPointer(0);
            for (int i = 0; i < numSamples; ++i)
            {
                float window = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * (float)i / (numSamples - 1)));
                fftData[i] = signal[i] * window;
            }
            
            // Clear imaginary part
            for (int i = numSamples; i < numSamples * 2; ++i)
            {
                fftData[i] = 0.0f;
            }
            
            // Perform FFT
            fft.performRealOnlyForwardTransform(fftData.getData());
            
            // Return magnitude function
            return [=](float frequency) -> float
            {
                int bin = static_cast<int>(std::round(frequency * numSamples / sampleRate));
                bin = juce::jlimit(0, numSamples / 2 - 1, bin);
                
                float real = fftData[bin * 2];
                float imag = fftData[bin * 2 + 1];
                float magnitude = std::sqrt(real * real + imag * imag) / (numSamples / 2);
                
                return toDecibels(magnitude);
            };
        };
        
        auto linearSpectrum = analyzeSpectrum(linearOutput);
        auto nonlinearSpectrum = analyzeSpectrum(nonlinearOutput);
        
        // Test harmonic generation
        const float fundamental_1kHz = nonlinearSpectrum(1000.0f);
        const float harmonic_3kHz_linear = linearSpectrum(3000.0f);
        const float harmonic_3kHz_nonlinear = nonlinearSpectrum(3000.0f);
        
        std::cout << "  - Fundamental (1kHz): " << fundamental_1kHz << " dB" << std::endl;
        std::cout << "  - 3rd Harmonic Linear: " << harmonic_3kHz_linear << " dB" << std::endl;
        std::cout << "  - 3rd Harmonic Nonlinear: " << harmonic_3kHz_nonlinear << " dB" << std::endl;
        
        // Test criteria (these should FAIL initially)
        bool test1 = harmonic_3kHz_linear < -90.0f;  // Linear should have minimal harmonics
        bool test2 = harmonic_3kHz_nonlinear > -60.0f;  // Nonlinear should generate visible harmonics
        bool test3 = (harmonic_3kHz_nonlinear - harmonic_3kHz_linear) > 20.0f;  // Significant difference
        
        std::cout << "  - Linear harmonic suppression: " << (test1 ? "PASS" : "FAIL") << std::endl;
        std::cout << "  - Nonlinear harmonic generation: " << (test2 ? "PASS" : "FAIL") << std::endl;
        std::cout << "  - Harmonic enhancement: " << (test3 ? "PASS" : "FAIL") << std::endl;
        
        if (test1 && test2 && test3)
        {
            std::cout << "✓ EMUFilter nonlinear test PASSED - harmonic generation working" << std::endl;
            return 0;
        }
        else
        {
            std::cout << "✗ EMUFilter nonlinear test FAILED - awaiting nonlinear implementation" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "✗ EMUFilter nonlinear test FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
}