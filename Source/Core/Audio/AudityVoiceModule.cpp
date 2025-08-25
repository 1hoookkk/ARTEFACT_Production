/**
 * AudityVoiceModule Implementation
 * RT-safe EMU Audity-style processing with filter, noise, and drift
 */

#include "AudityVoiceModule.h"
#include <cmath>
#include <algorithm>

AudityVoiceModule::AudityVoiceModule()
{
    // Initialize all states
    reset();
}

//==============================================================================
// Audio processing lifecycle

void AudityVoiceModule::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;
    
    // Initialize SSM2040 filter
    filter.setSampleRate(static_cast<float>(sampleRate));
    filter.updateCoefficients(cutoffFreq.load(), resonance.load());
    
    // Initialize SSM2050 envelope
    envelope.setSampleRate(static_cast<float>(sampleRate));
    envelope.setADSR(0.001f, 0.1f, 0.7f, 0.3f); // Default ADSR
    envelope.setOvershoot(1.1f); // 10% overshoot
    
    // Reset all state
    reset();
}

void AudityVoiceModule::reset()
{
    // Reset SSM2040 filter
    filter.reset();
    
    // Reset SSM2050 envelope
    envelope.reset();
    
    // Reset noise generators
    noiseGen.reset();
    
    // Reset modulation state
    driftPhase = 0.0f;
    bpmPhase = 0.0f;
    
    // Reset smoothed parameters
    smoothedCutoff = cutoffFreq.load();
    smoothedResonance = resonance.load();
}

void AudityVoiceModule::process(juce::AudioBuffer<float>& buffer, int numSamples)
{
    jassert(numSamples <= maxSamplesPerBlock);
    
    // Load atomic parameters (RT-safe)
    float targetCutoff = cutoffFreq.load();
    float targetResonance = resonance.load();
    bool syncEnabled = bpmSync.load();
    float driftAmount = drift.load();
    int currentNoiseType = noiseType.load();
    
    // Update modulation
    updateDrift();
    if (syncEnabled)
        updateBpmModulation();
    
    // Process each channel
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Smooth parameter changes to avoid clicks
            smoothedCutoff += (targetCutoff - smoothedCutoff) * 0.01f;
            smoothedResonance += (targetResonance - smoothedResonance) * 0.01f;
            
            // Apply drift modulation
            float modulatedCutoff = smoothedCutoff * (1.0f + currentDrift * driftAmount);
            
            // Apply BPM sync modulation if enabled
            if (syncEnabled)
            {
                float bpmMod = std::sin(bpmPhase) * 0.1f; // ±10% modulation
                modulatedCutoff *= (1.0f + bpmMod);
            }
            
            // Stroke-aware gain staging
            float inputGain = 1.0f + (strokeVelocity - 0.5f) * 0.5f; // ±25%
            float pressureGain = 0.5f + strokePressure * 0.5f; // 50%-100%
            
            // Add noise based on type (subtle amount)
            float noiseLevel = 0.005f * pressureGain; // Scale with pressure
            float noise = 0.0f;
            
            switch (currentNoiseType)
            {
                case 0: noise = noiseGen.generateWhite(); break;
                case 1: noise = noiseGen.generatePink(); break;
                case 2: noise = noiseGen.generateMauve(); break;
            }
            
            // Process input sample
            float inputSample = channelData[sample] * inputGain + noise * noiseLevel;
            
            // Update SSM2040 filter coefficients if needed
            static float lastCutoff = 0.0f;
            static float lastResonance = 0.0f;
            if (std::abs(modulatedCutoff - lastCutoff) > 1.0f || 
                std::abs(smoothedResonance - lastResonance) > 0.01f)
            {
                filter.updateCoefficients(modulatedCutoff, smoothedResonance);
                lastCutoff = modulatedCutoff;
                lastResonance = smoothedResonance;
            }
            
            // Process through SSM2040-inspired filter
            float output = filter.process(inputSample);
            
            // Apply SSM2050 envelope (amplitude modulation)
            float envelopeLevel = envelope.process();
            output *= envelopeLevel;
            
            // Write back to buffer
            channelData[sample] = output;
        }
    }
}

//==============================================================================
// Parameter control (RT-safe)

void AudityVoiceModule::setCutoffFrequency(float frequency)
{
    cutoffFreq.store(juce::jlimit(20.0f, 20000.0f, frequency));
}

void AudityVoiceModule::setResonance(float res)
{
    resonance.store(juce::jlimit(0.0f, 1.0f, res));
}

void AudityVoiceModule::setBpmSync(bool enabled)
{
    bpmSync.store(enabled);
}

void AudityVoiceModule::setDrift(float driftAmount)
{
    drift.store(juce::jlimit(0.0f, 0.05f, driftAmount));
}

void AudityVoiceModule::setNoiseType(int type)
{
    noiseType.store(juce::jlimit(0, 2, type));
}

//==============================================================================
// Host synchronization

void AudityVoiceModule::updatePlaybackInfo(const juce::AudioPlayHead::CurrentPositionInfo& positionInfo)
{
    hostBpm = positionInfo.bpm > 0 ? positionInfo.bpm : 120.0;
    hostPpqPosition = positionInfo.ppqPosition;
    isPlaying = positionInfo.isPlaying;
}

void AudityVoiceModule::setStrokeVelocity(float velocity)
{
    strokeVelocity = juce::jlimit(0.0f, 1.0f, velocity);
}

void AudityVoiceModule::setStrokePressure(float pressure)
{
    strokePressure = juce::jlimit(0.0f, 1.0f, pressure);
}

//==============================================================================
// Envelope control

void AudityVoiceModule::triggerEnvelope()
{
    envelope.noteOn();
}

void AudityVoiceModule::releaseEnvelope()
{
    envelope.noteOff();
}

//==============================================================================
// Helper methods

void AudityVoiceModule::updateDrift()
{
    // Slow-moving sine wave for organic drift
    driftPhase += driftSpeed * juce::MathConstants<float>::twoPi / static_cast<float>(currentSampleRate);
    if (driftPhase > juce::MathConstants<float>::twoPi)
        driftPhase -= juce::MathConstants<float>::twoPi;
    
    // Smooth sine wave with 1% random dither
    static std::mt19937 driftRng{std::random_device{}()};
    static std::uniform_real_distribution<float> dither{-0.01f, 0.01f};
    
    currentDrift = std::sin(driftPhase) + dither(driftRng);
}

void AudityVoiceModule::updateBpmModulation()
{
    if (!isPlaying || hostBpm <= 0)
        return;
    
    // Sync to quarter notes (1 beat)
    double beatsPerSecond = hostBpm / 60.0;
    double phaseIncrement = beatsPerSecond * juce::MathConstants<double>::twoPi / currentSampleRate;
    
    bpmPhase = static_cast<float>(std::fmod(hostPpqPosition * juce::MathConstants<double>::twoPi, juce::MathConstants<double>::twoPi));
}