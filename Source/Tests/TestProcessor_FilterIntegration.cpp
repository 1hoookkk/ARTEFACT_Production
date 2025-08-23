// TestProcessor_FilterIntegration.cpp
// RED test: Verifies APVTS→Processor→EMUFilter affects audio (integration test)
// This test will FAIL until GREEN phase wires EMUFilter into processBlock

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../Core/PluginProcessor.h"
#include "../ParamIDs.h"
#include <iostream>
#include <cmath>

// Helper: Generate a sine wave at specified frequency
static void generateSine(juce::AudioBuffer<float>& buffer, float frequencyHz, float sampleRate)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const float phaseIncrement = 2.0f * juce::MathConstants<float>::pi * frequencyHz / sampleRate;
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* data = buffer.getWritePointer(channel);
        float phase = 0.0f;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            data[sample] = std::sin(phase);
            phase += phaseIncrement;
            if (phase > 2.0f * juce::MathConstants<float>::pi)
                phase -= 2.0f * juce::MathConstants<float>::pi;
        }
    }
}

// Helper: Calculate RMS of a buffer
static float calculateRMS(const juce::AudioBuffer<float>& buffer)
{
    float sum = 0.0f;
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* data = buffer.getReadPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            sum += data[sample] * data[sample];
        }
    }
    
    return std::sqrt(sum / static_cast<float>(numChannels * numSamples));
}

// Main test function
int TestProcessor_FilterIntegration()
{
    std::cout << "\n=== TestProcessor_FilterIntegration (RED) ===" << std::endl;
    std::cout << "Testing APVTS → Processor → EMUFilter audio path integration..." << std::endl;
    
    try {
        // ARRANGE: Create processor and prepare it
        ARTEFACTAudioProcessor processor;
        
        const double sampleRate = 48000.0;
        const int blockSize = 4096;
        const int numChannels = 2;
        
        std::cout << "  Preparing processor: SR=" << sampleRate << " Hz, blockSize=" << blockSize << std::endl;
        processor.prepareToPlay(sampleRate, blockSize);
        
        // Create test signal: 1 kHz sine wave
        juce::AudioBuffer<float> testBuffer(numChannels, blockSize);
        const float testFrequency = 1000.0f; // 1 kHz test tone
        generateSine(testBuffer, testFrequency, static_cast<float>(sampleRate));
        
        // Measure input RMS
        const float inputRMS = calculateRMS(testBuffer);
        std::cout << "  Input signal: 1 kHz sine, RMS=" << inputRMS << std::endl;
        
        // ACT: Set filter parameters via APVTS
        const float cutoffFreq = 200.0f; // 200 Hz cutoff
        const float filterQ = 0.707f;    // Butterworth Q
        
        // Set filter parameters through APVTS
        if (auto* cutoffParam = processor.apvts.getParameter(juce::String(ParamIDs::filterCutoff)))
        {
            // Normalize to 0-1 range (assuming 20-20000 Hz range)
            float normalizedValue = (cutoffFreq - 20.0f) / (20000.0f - 20.0f);
            cutoffParam->setValueNotifyingHost(normalizedValue);
            std::cout << "  Set filter cutoff to " << cutoffFreq << " Hz (normalized: " << normalizedValue << ")" << std::endl;
        }
        else
        {
            std::cout << "  WARNING: filterCutoff parameter not found in APVTS" << std::endl;
        }
        
        // Use filterResonance instead of filterQ (based on ParamIDs.h)
        if (auto* resonanceParam = processor.apvts.getParameter(juce::String(ParamIDs::filterResonance)))
        {
            // Normalize Q to 0-1 range (assuming 0.1-10 range for resonance)
            float normalizedQ = (filterQ - 0.1f) / (10.0f - 0.1f);
            resonanceParam->setValueNotifyingHost(normalizedQ);
            std::cout << "  Set filter resonance to " << filterQ << " (normalized: " << normalizedQ << ")" << std::endl;
        }
        else
        {
            std::cout << "  WARNING: filterResonance parameter not found in APVTS" << std::endl;
        }
        
        // Process the audio through the processor
        juce::MidiBuffer midiBuffer;
        processor.processBlock(testBuffer, midiBuffer);
        
        // ASSERT: Measure output RMS and check for attenuation
        const float outputRMS = calculateRMS(testBuffer);
        
        // First check if we got any output at all
        if (outputRMS < 0.001f)
        {
            std::cout << "✗ TestProcessor_FilterIntegration FAILED: " << std::endl;
            std::cout << "  Output is silent (RMS=" << outputRMS << ")" << std::endl;
            std::cout << "  Processor is not producing any audio output" << std::endl;
            std::cout << "  (This is expected in RED phase - filter not wired to audio path)" << std::endl;
            return 1; // Test fails (expected for RED)
        }
        
        const float attenuationDB = 20.0f * std::log10(outputRMS / inputRMS);
        
        std::cout << "  Output RMS=" << outputRMS << std::endl;
        std::cout << "  Attenuation=" << attenuationDB << " dB" << std::endl;
        
        // Expected: ~18-24 dB attenuation at 1 kHz with 200 Hz cutoff
        // (1 kHz is ~2.3 octaves above 200 Hz, so ~14 dB/octave for 2nd order = ~32 dB theoretical)
        // But we'll be conservative and expect at least 18 dB
        const float expectedMinAttenuation = -18.0f;
        
        if (attenuationDB <= expectedMinAttenuation)
        {
            std::cout << "✓ TestProcessor_FilterIntegration PASSED: " 
                      << "Filter properly attenuated 1 kHz signal with 200 Hz cutoff" << std::endl;
            return 0; // Test passes
        }
        else
        {
            std::cout << "✗ TestProcessor_FilterIntegration FAILED: " << std::endl;
            std::cout << "  Expected at least " << expectedMinAttenuation << " dB attenuation" << std::endl;
            std::cout << "  Got " << attenuationDB << " dB" << std::endl;
            std::cout << "  (Filter is not affecting the audio path - this is expected in RED phase)" << std::endl;
            return 1; // Test fails (expected for RED)
        }
        
    } catch (const std::exception& e) {
        std::cout << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

// Standalone test runner
#ifdef STANDALONE_TEST
int main()
{
    // Initialize JUCE subsystems
    juce::ScopedJuceInitialiser_GUI juceInit;
    
    int result = TestProcessor_FilterIntegration();
    
    return result;
}
#endif