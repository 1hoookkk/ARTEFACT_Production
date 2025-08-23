// FrequencyLUT Performance and Correctness Test
// Validates MIDI-to-frequency lookup table optimization for RT-safety:
// - Initialization creates correct frequency tables
// - RT-safe lookups match HarmonicQuantizer results
// - Performance improvement vs expensive log2/pow operations
// - Quantization caching works correctly
//
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <iostream>
#include <chrono>

#define JucePlugin_Name "SpectralCanvas Pro"

// Include the LUT and original quantizer for comparison
#include "../Source/Core/FrequencyLUT.h"
#include "../Source/Core/HarmonicQuantizer.h"

struct TestFrequencyLUT : juce::UnitTest
{
    TestFrequencyLUT() : juce::UnitTest ("FrequencyLUT — Performance & Correctness", "LUT") {}

    void runTest() override
    {
        beginTest ("FrequencyLUT Initialization");
        {
            // Initialize the LUT
            scp::FrequencyLUT::initialize();
            expect(scp::FrequencyLUT::isInitialized(), "LUT should be initialized");
            
            logMessage("FrequencyLUT initialized successfully");
        }

        beginTest ("MIDI-to-Frequency Accuracy");
        {
            // Test key MIDI notes
            const struct { int midi; float expectedFreq; const char* note; } testCases[] = {
                { 0,   8.1758f,   "C-1"  },
                { 12,  16.3516f,  "C0"   },
                { 21,  27.5f,     "A0"   },
                { 60,  261.626f,  "C4"   },
                { 69,  440.0f,    "A4"   },
                { 72,  523.251f,  "C5"   },
                { 127, 12543.85f, "G9"   }
            };

            for (const auto& test : testCases)
            {
                float lutFreq = scp::FrequencyLUT::midiToFreq(test.midi);
                float expectedFreq = test.expectedFreq;
                float tolerance = expectedFreq * 0.001f; // 0.1% tolerance
                
                expectWithinAbsoluteError(lutFreq, expectedFreq, tolerance, 
                    juce::String("MIDI ") + juce::String(test.midi) + " (" + test.note + ")");
            }
            
            logMessage("MIDI-to-frequency accuracy validated");
        }

        beginTest ("Fractional MIDI Support");
        {
            // Test fractional MIDI notes
            float freq69_0 = scp::FrequencyLUT::midiToFreq(69.0);   // A4 = 440 Hz
            float freq69_5 = scp::FrequencyLUT::midiToFreq(69.5);   // Halfway to A#4
            float freq70_0 = scp::FrequencyLUT::midiToFreq(70.0);   // A#4
            
            expect(freq69_0 == 440.0f, "MIDI 69.0 should be exactly 440 Hz");
            expect(freq69_5 > freq69_0 && freq69_5 < freq70_0, "MIDI 69.5 should be between A4 and A#4");
            
            // Check interpolation is approximately correct
            float expectedMidpoint = (freq69_0 + freq70_0) * 0.5f;
            float tolerance = 5.0f; // 5 Hz tolerance for linear interpolation approximation
            expectWithinAbsoluteError(freq69_5, expectedMidpoint, tolerance, "Linear interpolation");
            
            logMessage("Fractional MIDI support validated");
        }

        beginTest ("Frequency-to-MIDI Conversion");
        {
            // Test roundtrip conversion
            const double testFrequencies[] = { 27.5, 110.0, 220.0, 440.0, 880.0, 1760.0, 3520.0 };
            
            for (double freq : testFrequencies)
            {
                double midiFloat = scp::FrequencyLUT::freqToMidi(freq);
                float reconstructedFreq = scp::FrequencyLUT::midiToFreq(midiFloat);
                
                float tolerance = static_cast<float>(freq * 0.01); // 1% tolerance
                expectWithinAbsoluteError(reconstructedFreq, static_cast<float>(freq), tolerance,
                    juce::String("Roundtrip for ") + juce::String(freq) + " Hz");
            }
            
            logMessage("Frequency-to-MIDI conversion validated");
        }

        beginTest ("Quantization Correctness vs HarmonicQuantizer");
        {
            // Compare LUT quantization with original HarmonicQuantizer
            const double testFrequencies[] = { 261.626, 293.665, 329.628, 349.228, 391.995, 440.0, 493.883 }; // C4 major scale
            const double sigmaCents = 50.0;
            
            for (double freq : testFrequencies)
            {
                // Original HarmonicQuantizer result
                double originalWeight = scp::computeSnapWeightCmaj(freq, sigmaCents);
                double originalWeightCents = 0.0;
                double originalSnapped = scp::computeSnappedFrequencyCmaj(freq, sigmaCents, originalWeightCents);
                
                // FrequencyLUT result
                auto lutResult = scp::FrequencyLUT::quantizeFrequency(freq, sigmaCents);
                
                // Compare snap weights (should be very close)
                float weightTolerance = 0.05f; // 5% tolerance
                expectWithinAbsoluteError(lutResult.snapWeight, static_cast<float>(originalWeight), weightTolerance,
                    juce::String("Snap weight for ") + juce::String(freq) + " Hz");
                
                // Compare snapped frequencies (should be very close)
                float freqTolerance = static_cast<float>(freq * 0.01); // 1% tolerance
                expectWithinAbsoluteError(lutResult.snappedFrequency, static_cast<float>(originalSnapped), freqTolerance,
                    juce::String("Snapped frequency for ") + juce::String(freq) + " Hz");
            }
            
            logMessage("Quantization correctness validated against HarmonicQuantizer");
        }

        beginTest ("Performance Comparison");
        {
            const int numIterations = 100000;
            const double testFreq = 440.0 * 1.5; // Slightly off A4 to test quantization
            const double sigmaCents = 25.0;
            
            // Measure original HarmonicQuantizer performance
            auto startTime = std::chrono::high_resolution_clock::now();
            
            double totalWeight = 0.0;
            for (int i = 0; i < numIterations; ++i)
            {
                double weight = scp::computeSnapWeightCmaj(testFreq, sigmaCents);
                totalWeight += weight;
            }
            
            auto originalTime = std::chrono::high_resolution_clock::now();
            auto originalDuration = std::chrono::duration_cast<std::chrono::microseconds>(originalTime - startTime);
            
            // Measure FrequencyLUT performance  
            startTime = std::chrono::high_resolution_clock::now();
            
            float totalWeightLUT = 0.0f;
            for (int i = 0; i < numIterations; ++i)
            {
                auto result = scp::FrequencyLUT::quantizeFrequency(testFreq, sigmaCents);
                totalWeightLUT += result.snapWeight;
            }
            
            auto lutTime = std::chrono::high_resolution_clock::now();
            auto lutDuration = std::chrono::duration_cast<std::chrono::microseconds>(lutTime - startTime);
            
            // Verify results are similar
            expect(std::abs(totalWeight - double(totalWeightLUT)) < 0.1, "Performance test results should be similar");
            
            // Log performance results
            double speedup = double(originalDuration.count()) / double(lutDuration.count());
            logMessage(juce::String("Original HarmonicQuantizer: ") + juce::String(originalDuration.count()) + " μs");
            logMessage(juce::String("FrequencyLUT: ") + juce::String(lutDuration.count()) + " μs");
            logMessage(juce::String("Speedup: ") + juce::String(speedup, 2) + "x");
            
            // Expect at least some performance improvement (caching should help)
            expect(lutDuration.count() <= originalDuration.count(), "LUT should not be significantly slower");
        }

        beginTest ("Quantization Caching");
        {
            const double testFreq = 440.0;
            const double sigmaCents = 30.0;
            
            // First call (cache miss)
            auto result1 = scp::FrequencyLUT::quantizeFrequency(testFreq, sigmaCents);
            
            // Second call (cache hit - should be identical)
            auto result2 = scp::FrequencyLUT::quantizeFrequency(testFreq, sigmaCents);
            
            expect(result1.snappedFrequency == result2.snappedFrequency, "Cached frequency should be identical");
            expect(result1.snapWeight == result2.snapWeight, "Cached weight should be identical");
            expect(result1.nearestMidi == result2.nearestMidi, "Cached MIDI should be identical");
            expect(result1.centsOffset == result2.centsOffset, "Cached cents should be identical");
            
            logMessage("Quantization caching validated");
        }

        beginTest ("RT-Safety Edge Cases");
        {
            // Test edge cases that could cause RT-safety issues
            
            // Zero and negative frequencies
            auto result = scp::FrequencyLUT::quantizeFrequency(0.0, 25.0);
            expect(result.snappedFrequency == 0.0f, "Zero frequency should return zero");
            
            result = scp::FrequencyLUT::quantizeFrequency(-100.0, 25.0);
            expect(result.snappedFrequency == 0.0f, "Negative frequency should return zero");
            
            // Very large frequencies (should clamp)
            result = scp::FrequencyLUT::quantizeFrequency(50000.0, 25.0);
            expect(result.snappedFrequency > 0.0f, "Large frequency should return valid result");
            
            // Very small sigma (should not divide by zero)
            result = scp::FrequencyLUT::quantizeFrequency(440.0, 1e-15);
            expect(result.snappedFrequency > 0.0f, "Tiny sigma should not cause division by zero");
            
            // Test without initialization (should not crash)
            // Note: Can't easily test this without resetting static state
            
            logMessage("RT-Safety edge cases validated");
        }
    }
};

static TestFrequencyLUT testFrequencyLUT;