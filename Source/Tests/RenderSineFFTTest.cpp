#include <juce_dsp/juce_dsp.h>
#include "../Core/SpectralSynthEngine.h"
#include <cmath>
#include <iostream>

static float db(float m) { return 20.0f * std::log10(std::max(m, 1e-12f)); }

int RenderSineFFTTest()
{
    std::cout << "RenderSineFFTTest: Starting offline render + FFT verification..." << std::endl;
    
    const double sr = 48000.0;
    const int    N  = 1 << 15;  // 32768 samples ~0.68s
    const float  targetHz = 1000.0f;

    try {
        // 1) Prepare engine
        SpectralSynthEngine engine;
        engine.prepareToPlay(sr, 512, 2);

        // 2) Simulate a paint gesture that should produce 1000Hz
        // Using the mapping: frequency = jmap(1.0f - (y / 600.0f), 20.0f, 20000.0f)
        // For 1000Hz: 1000 = jmap(1.0f - (y / 600.0f), 20.0f, 20000.0f)
        // Solve: (1000 - 20) / (20000 - 20) = 1.0f - (y / 600.0f)
        // 0.049 = 1.0f - (y / 600.0f) -> y = 600 * (1.0f - 0.049) = 570.6
        float testY = 570.6f;  // Should map to ~1000Hz
        float testX = 400.0f;  // Center X (irrelevant for frequency)
        float testPressure = 0.8f;  // Good amplitude

        juce::Colour testColor = juce::Colours::blue;
        engine.processPaintStrokeDirectToAudio(testX, testY, testPressure, testColor);
        
        std::cout << "  Paint gesture: y=" << testY << " → target " << targetHz << "Hz, pressure=" << testPressure << std::endl;

        // 3) Offline render
        juce::AudioBuffer<float> buf(2, N);  // Stereo for safety
        buf.clear();
        
        int offset = 0;
        while (offset < N)
        {
            const int block = 512;
            int samplesThisBlock = std::min(block, N - offset);
            
            // Create a view of the buffer for this block
            juce::AudioBuffer<float> blockBuf(buf.getArrayOfWritePointers(), 2, offset, samplesThisBlock);
            engine.processBlock(blockBuf);
            
            offset += samplesThisBlock;
        }
        
        std::cout << "  Rendered " << N << " samples at " << sr << "Hz" << std::endl;

        // 4) Window + FFT (using mono - left channel)
        juce::dsp::FFT fft(15); // 2^15 = 32768
        std::vector<float> fftData(2 * N, 0.0f);
        
        // Apply Hann window and copy to FFT buffer
        for (int i = 0; i < N; ++i) {
            float window = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (N - 1)));
            fftData[i] = buf.getSample(0, i) * window;  // Left channel
        }
        
        fft.performRealOnlyForwardTransform(fftData.data());
        
        std::cout << "  FFT analysis complete" << std::endl;

        // 5) Find peak bin
        const double binHz = sr / (double)N;
        int peakBin = 0; 
        float peakMag = 0.0f;
        
        for (int b = 1; b < N / 2; ++b)
        {
            const float re = fftData[2 * b];
            const float im = fftData[2 * b + 1];
            const float mag = std::sqrt(re * re + im * im);
            if (mag > peakMag) { 
                peakMag = mag; 
                peakBin = b; 
            }
        }

        const double detectedHz = peakBin * binHz;
        const float peakDb = db(peakMag);
        
        std::cout << "  Peak detected: " << detectedHz << "Hz at " << peakDb << "dB" << std::endl;

        // 6) Assertions: frequency near target, peak well above noise floor
        const bool freqOK = std::abs(detectedHz - targetHz) < 10.0; // 10Hz tolerance (generous)
        const bool levelOK = peakDb > -30.0f; // -30dB threshold (generous)

        if (freqOK) {
            std::cout << "  ✓ Frequency test PASSED: " << detectedHz << "Hz (target " << targetHz << "Hz)" << std::endl;
        } else {
            std::cout << "  ✗ Frequency test FAILED: " << detectedHz << "Hz (target " << targetHz << "Hz)" << std::endl;
        }
        
        if (levelOK) {
            std::cout << "  ✓ Level test PASSED: " << peakDb << "dB (threshold -30dB)" << std::endl;
        } else {
            std::cout << "  ✗ Level test FAILED: " << peakDb << "dB (threshold -30dB)" << std::endl;
        }

        if (!freqOK || !levelOK)
        {
            std::cout << "✗ RenderSineFFTTest FAILED: Physics-level audio verification failed" << std::endl;
            return 1;
        }
        
        std::cout << "✓ RenderSineFFTTest PASSED: Physics-level audio generation verified" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "✗ RenderSineFFTTest FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
}

int main()
{
    return RenderSineFFTTest();
}