/**
 * AudityVoiceModule - EMU Audity 2000 Behavioral Recreation
 * 
 * RT-safe behavioral model of the legendary EMU Audity 2000 (c.1979-1980)
 * Based on comprehensive historical research and SSM chip specifications.
 * 
 * Authentic SSM2040-inspired features:
 * - 4-pole lowpass filter with bilinear pre-warping
 * - ADSR envelope with attack overshoot (>1.0, decays to sustain)
 * - White/Pink/Mauve noise (1/f³ spectral slope)  
 * - VCA with linear/exponential response and soft saturation
 * - Post-VCA "punch path" resonant filter for transient emphasis
 * - Exponential cutoff control over 10,000:1 range
 * - Modulation matrix with LFO and envelope routing
 * 
 * Abstract preset collection: "Distant Thunder", "Crystal Cascade", "Velvet Shadows", "Chrome Whispers"
 */

#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <array>
#include <random>
#include <memory>
#include "../LFO.h"

class AudityVoiceModule
{
public:
    AudityVoiceModule();
    ~AudityVoiceModule() = default;

    //==============================================================================
    // Audio processing lifecycle
    void prepare(double sampleRate, int maxBlockSize);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, int numSamples);

    //==============================================================================
    // Parameter control (RT-safe)
    void setCutoffFrequency(float frequency);
    void setResonance(float resonance);
    void setBpmSync(bool enabled);
    void setDrift(float driftAmount);
    void setNoiseType(int noiseType); // 0=white, 1=pink, 2=mauve
    void setPunchPath(bool enabled, float mix = 0.3f, float drive = 2.0f);
    
    //==============================================================================
    // Modulation matrix control
    void setMovementDepth(float depth);        // Movement knob (0.0-1.0)
    void setLFOModulation(float toCutoff, float toResonance, float toVCA);
    void setEnvelopeModulation(float toCutoff, float toResonance);
    void setMovementLFO(LFO* lfo);            // Shared LFO from processor

    //==============================================================================
    // Host synchronization
    void updatePlaybackInfo(const juce::AudioPlayHead::CurrentPositionInfo& positionInfo);
    void setStrokeVelocity(float velocity); // From paint engine
    void setStrokePressure(float pressure);  // From paint engine
    
    //==============================================================================
    // Envelope control
    void triggerEnvelope(); // Called when paint stroke starts
    void releaseEnvelope(); // Called when paint stroke ends

private:
    //==============================================================================
    // Audio state
    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;
    
    //==============================================================================
    // Filter parameters (atomic for RT-safety)
    std::atomic<float> cutoffFreq{1200.0f};
    std::atomic<float> resonance{0.65f};
    std::atomic<bool> bpmSync{false};
    std::atomic<float> drift{0.02f};
    std::atomic<int> noiseType{1}; // Default to pink
    
    //==============================================================================
    // Real-time state (RT-thread only)
    float smoothedCutoff = 1200.0f;
    float smoothedResonance = 0.65f;
    float currentDrift = 0.0f;
    float strokeVelocity = 0.7f;
    float strokePressure = 0.5f;
    
    //==============================================================================
    // BPM synchronization
    double hostBpm = 120.0;
    double hostPpqPosition = 0.0;
    bool isPlaying = false;
    float bpmPhase = 0.0f;
    
    //==============================================================================
    // SSM2040-inspired 4-pole Lowpass Filter with bilinear pre-warping
    struct SSM2040Filter
    {
        // Two cascaded biquad stages for 4-pole response
        struct BiquadStage
        {
            float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
            float a1 = 0.0f, a2 = 0.0f;
            float x1 = 0.0f, x2 = 0.0f;
            float y1 = 0.0f, y2 = 0.0f;
            
            void reset()
            {
                x1 = x2 = y1 = y2 = 0.0f;
            }
            
            float process(float input)
            {
                float output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
                
                x2 = x1; x1 = input;
                y2 = y1; y1 = output;
                
                return output;
            }
        };
        
        std::array<BiquadStage, 2> stages; // 4-pole = 2 cascaded biquads
        float sampleRate = 44100.0f;
        float currentCutoff = 1200.0f;
        float currentResonance = 0.65f;
        
        // SSM2040 characteristics
        float gainCompensation = 1.0f; // Auto-gain for high resonance
        float saturationDrive = 1.0f;   // Pre/post filter saturation
        
        void setSampleRate(float sr)
        {
            sampleRate = sr;
        }
        
        void reset()
        {
            for (auto& stage : stages)
                stage.reset();
        }
        
        // Bilinear pre-warping as per your specification
        float prewarpCutoff(float cutoffHz)
        {
            const float nyquist = sampleRate * 0.5f;
            cutoffHz = juce::jlimit(20.0f, nyquist * 0.9f, cutoffHz);
            
            // Bilinear transform pre-warping
            const float omega = juce::MathConstants<float>::twoPi * cutoffHz / sampleRate;
            return (2.0f * sampleRate) * std::tan(omega * 0.5f);
        }
        
        void updateCoefficients(float cutoffHz, float resonance)
        {
            currentCutoff = cutoffHz;
            currentResonance = juce::jlimit(0.0f, 0.99f, resonance);
            
            // Pre-warp the cutoff frequency
            float warpedCutoff = prewarpCutoff(cutoffHz);
            float omega = juce::MathConstants<float>::twoPi * warpedCutoff / sampleRate;
            float cosOmega = std::cos(omega);
            float sinOmega = std::sin(omega);
            
            // SSM2040-style Q mapping (0.2 to 10+ range)
            float Q = 0.2f + currentResonance * 9.8f;
            float alpha = sinOmega / (2.0f * Q);
            
            // Auto-gain compensation when resonance > 0.8
            if (currentResonance > 0.8f)
            {
                float excess = (currentResonance - 0.8f) / 0.2f; // 0-1 range
                gainCompensation = 1.0f - excess * 0.3f; // Up to 30% reduction
            }
            else
            {
                gainCompensation = 1.0f;
            }
            
            // Lowpass biquad coefficients
            float norm = 1.0f / (1.0f + alpha);
            float b0 = (1.0f - cosOmega) * 0.5f * norm;
            float b1 = (1.0f - cosOmega) * norm;
            float b2 = (1.0f - cosOmega) * 0.5f * norm;
            float a1 = -2.0f * cosOmega * norm;
            float a2 = (1.0f - alpha) * norm;
            
            // Apply to both stages for 4-pole response
            for (auto& stage : stages)
            {
                stage.b0 = b0;
                stage.b1 = b1;
                stage.b2 = b2;
                stage.a1 = a1;
                stage.a2 = a2;
            }
        }
        
        float process(float input)
        {
            // Pre-filter soft saturation (SSM2040 characteristic)
            float saturated = std::tanh(input * saturationDrive);
            
            // Process through cascaded stages
            float output = saturated;
            for (auto& stage : stages)
            {
                output = stage.process(output);
            }
            
            // Apply gain compensation and post-filter soft saturation
            output *= gainCompensation;
            return std::tanh(output * 0.7f); // Gentle post-saturation
        }
    };
    
    SSM2040Filter filter;
    
    //==============================================================================
    // SSM2050-inspired ADSR Envelope with Overshoot
    struct SSM2050Envelope
    {
        enum Stage
        {
            Idle,
            Delay,
            Attack,
            Decay,
            Sustain,
            Release
        };
        
        Stage currentStage = Idle;
        float sampleRate = 44100.0f;
        float currentValue = 0.0f;
        float targetValue = 0.0f;
        float stageIncrement = 0.0f;
        int samplesRemaining = 0;
        
        // ADSR parameters (all in seconds)
        float delayTime = 0.0f;
        float attackTime = 0.001f;  // 1ms minimum
        float decayTime = 0.1f;
        float sustainLevel = 0.7f;
        float releaseTime = 0.3f;
        
        // SSM2050 characteristics
        float overshoot = 1.1f;     // Attack overshoot (>1.0, decays to sustain)
        bool exponentialCurves = true;
        
        void setSampleRate(float sr)
        {
            sampleRate = sr;
        }
        
        void setADSR(float attack, float decay, float sustain, float release, float delay = 0.0f)
        {
            attackTime = juce::jlimit(0.001f, 10.0f, attack);   // 1ms to 10s
            decayTime = juce::jlimit(0.005f, 30.0f, decay);     // 5ms to 30s
            sustainLevel = juce::jlimit(0.0f, 1.0f, sustain);
            releaseTime = juce::jlimit(0.005f, 30.0f, release); // 5ms to 30s
            delayTime = juce::jlimit(0.0f, 1.0f, delay);        // Up to 1s delay
        }
        
        void setOvershoot(float overshootAmount)
        {
            overshoot = juce::jlimit(1.0f, 1.5f, overshootAmount); // 0-50% overshoot
        }
        
        void noteOn()
        {
            if (delayTime > 0.001f)
            {
                // Start with delay stage
                currentStage = Delay;
                samplesRemaining = static_cast<int>(delayTime * sampleRate);
                currentValue = 0.0f;
                targetValue = 0.0f;
                stageIncrement = 0.0f;
            }
            else
            {
                // Skip delay, go straight to attack
                startAttack();
            }
        }
        
        void noteOff()
        {
            currentStage = Release;
            targetValue = 0.0f;
            samplesRemaining = static_cast<int>(releaseTime * sampleRate);
            
            if (exponentialCurves && samplesRemaining > 0)
            {
                stageIncrement = -currentValue / samplesRemaining;
            }
        }
        
        float process()
        {
            switch (currentStage)
            {
                case Idle:
                    return 0.0f;
                    
                case Delay:
                    if (--samplesRemaining <= 0)
                        startAttack();
                    return 0.0f;
                    
                case Attack:
                    if (--samplesRemaining <= 0)
                    {
                        startDecay();
                    }
                    else if (exponentialCurves)
                    {
                        // Exponential attack curve
                        float progress = 1.0f - (static_cast<float>(samplesRemaining) / (attackTime * sampleRate));
                        currentValue = overshoot * (1.0f - std::exp(-5.0f * progress));
                    }
                    else
                    {
                        currentValue += stageIncrement;
                    }
                    return currentValue;
                    
                case Decay:
                    if (--samplesRemaining <= 0)
                    {
                        currentStage = Sustain;
                        currentValue = sustainLevel;
                    }
                    else if (exponentialCurves)
                    {
                        // Exponential decay from overshoot to sustain
                        float progress = 1.0f - (static_cast<float>(samplesRemaining) / (decayTime * sampleRate));
                        currentValue = sustainLevel + (overshoot - sustainLevel) * std::exp(-5.0f * progress);
                    }
                    else
                    {
                        currentValue += stageIncrement;
                    }
                    return currentValue;
                    
                case Sustain:
                    return sustainLevel;
                    
                case Release:
                    if (--samplesRemaining <= 0)
                    {
                        currentStage = Idle;
                        currentValue = 0.0f;
                        return 0.0f;
                    }
                    else if (exponentialCurves)
                    {
                        // Exponential release
                        float progress = 1.0f - (static_cast<float>(samplesRemaining) / (releaseTime * sampleRate));
                        currentValue = sustainLevel * std::exp(-5.0f * progress);
                    }
                    else
                    {
                        currentValue += stageIncrement;
                    }
                    return currentValue;
            }
            
            return 0.0f;
        }
        
        bool isActive() const
        {
            return currentStage != Idle;
        }
        
        void reset()
        {
            currentStage = Idle;
            currentValue = 0.0f;
            targetValue = 0.0f;
            samplesRemaining = 0;
        }
        
    private:
        void startAttack()
        {
            currentStage = Attack;
            currentValue = 0.0f;
            targetValue = overshoot; // Overshoot target
            samplesRemaining = static_cast<int>(attackTime * sampleRate);
            
            if (!exponentialCurves && samplesRemaining > 0)
            {
                stageIncrement = targetValue / samplesRemaining;
            }
        }
        
        void startDecay()
        {
            currentStage = Decay;
            currentValue = overshoot; // Start from overshoot level
            targetValue = sustainLevel;
            samplesRemaining = static_cast<int>(decayTime * sampleRate);
            
            if (!exponentialCurves && samplesRemaining > 0)
            {
                stageIncrement = (targetValue - currentValue) / samplesRemaining;
            }
        }
    };
    
    SSM2050Envelope envelope;
    
    //==============================================================================
    // SSM2020-inspired VCA with Punch Path
    struct SSM2020VCA
    {
        bool punchPathEnabled = false;
        float punchMix = 0.3f;         // Amount of punch path in final mix
        float punchDrive = 2.0f;       // Aggressive saturation drive
        float hpfCutoff = 200.0f;      // High-pass filter cutoff for punch path
        
        // High-pass filter state for punch path
        struct HPFilter
        {
            float x1 = 0.0f, y1 = 0.0f;
            float sampleRate = 44100.0f;
            
            void setSampleRate(float sr) { sampleRate = sr; }
            
            void reset() { x1 = y1 = 0.0f; }
            
            float process(float input, float cutoffHz)
            {
                // Simple 1-pole HPF for punch path
                float RC = 1.0f / (cutoffHz * juce::MathConstants<float>::twoPi);
                float dt = 1.0f / sampleRate;
                float alpha = RC / (RC + dt);
                
                float output = alpha * (y1 + input - x1);
                x1 = input;
                y1 = output;
                return output;
            }
        } punchHPF;
        
        void setSampleRate(float sampleRate)
        {
            punchHPF.setSampleRate(sampleRate);
        }
        
        void reset()
        {
            punchHPF.reset();
        }
        
        void setPunchPath(bool enabled, float mix = 0.3f, float drive = 2.0f)
        {
            punchPathEnabled = enabled;
            punchMix = juce::jlimit(0.0f, 1.0f, mix);
            punchDrive = juce::jlimit(1.0f, 5.0f, drive);
        }
        
        float process(float input, float vcaLevel, bool useExponentialResponse = false)
        {
            // Main VCA path - clean linear or exponential amplification
            float mainOutput;
            if (useExponentialResponse)
            {
                // Exponential VCA response (more musical for envelopes)
                mainOutput = input * std::pow(vcaLevel, 2.0f);
            }
            else
            {
                // Linear VCA response (cleaner for precise control)
                mainOutput = input * vcaLevel;
            }
            
            // Soft-clip the main path
            mainOutput = std::tanh(mainOutput * 0.8f);
            
            if (!punchPathEnabled)
                return mainOutput;
            
            // Punch path processing
            float punchSignal = punchHPF.process(input, hpfCutoff);
            
            // Aggressive saturation for punch path
            punchSignal = std::tanh(punchSignal * punchDrive) * vcaLevel;
            
            // Mix punch path back with main signal
            return mainOutput * (1.0f - punchMix) + punchSignal * punchMix;
        }
    };
    
    SSM2020VCA vca;
    
    //==============================================================================
    // Modulation Matrix (Audity-style routing)
    struct ModulationMatrix
    {
        // Modulation sources (values between -1.0 and 1.0)
        float lfoValue = 0.0f;
        float envelopeValue = 0.0f;
        float velocityValue = 0.0f;
        float pressureValue = 0.0f;
        
        // Modulation depths (0.0 to 1.0)
        float lfoToCutoff = 0.0f;
        float lfoToResonance = 0.0f;
        float lfoToVCALevel = 0.0f;
        float lfoToPitch = 0.0f;          // Future: for oscillator pitch mod
        
        float envelopeToCutoff = 0.0f;
        float envelopeToResonance = 0.0f;
        
        float velocityToCutoff = 0.0f;
        float velocityToVCALevel = 0.3f;  // Default velocity sensitivity
        
        float pressureToCutoff = 0.2f;    // Default pressure to cutoff
        float pressureToResonance = 0.1f; // Default pressure to resonance
        
        // Movement knob (global modulation depth multiplier)
        float movementDepth = 0.0f;
        
        void reset()
        {
            lfoValue = 0.0f;
            envelopeValue = 0.0f;
            velocityValue = 0.0f;
            pressureValue = 0.0f;
        }
        
        void updateSources(float lfo, float envelope, float velocity, float pressure)
        {
            lfoValue = lfo;
            envelopeValue = envelope;
            velocityValue = velocity;
            pressureValue = pressure;
        }
        
        float getModulatedCutoff(float baseCutoff) const
        {
            float modulation = 0.0f;
            
            // LFO modulation (scaled by Movement depth)
            modulation += lfoValue * lfoToCutoff * movementDepth;
            
            // Envelope modulation
            modulation += envelopeValue * envelopeToCutoff;
            
            // Velocity modulation
            modulation += velocityValue * velocityToCutoff;
            
            // Pressure modulation (real-time paint control)
            modulation += pressureValue * pressureToCutoff;
            
            // Apply modulation as frequency multiplier (exponential)
            float multiplier = std::pow(2.0f, modulation * 4.0f); // ±4 octaves max
            return baseCutoff * juce::jlimit(0.1f, 10.0f, multiplier);
        }
        
        float getModulatedResonance(float baseResonance) const
        {
            float modulation = 0.0f;
            
            // LFO modulation (scaled by Movement depth)
            modulation += lfoValue * lfoToResonance * movementDepth;
            
            // Envelope modulation
            modulation += envelopeValue * envelopeToResonance;
            
            // Pressure modulation
            modulation += pressureValue * pressureToResonance;
            
            return juce::jlimit(0.0f, 0.99f, baseResonance + modulation * 0.5f);
        }
        
        float getModulatedVCALevel(float baseLevel) const
        {
            float modulation = 0.0f;
            
            // LFO modulation (scaled by Movement depth)
            modulation += lfoValue * lfoToVCALevel * movementDepth;
            
            // Velocity modulation
            modulation += velocityValue * velocityToVCALevel;
            
            // Apply as linear modulation
            return juce::jlimit(0.0f, 1.0f, baseLevel + modulation * 0.3f);
        }
        
        // Future: getPitchModulation for oscillator pitch
        float getPitchModulation() const
        {
            return lfoValue * lfoToPitch * movementDepth;
        }
    };
    
    ModulationMatrix modMatrix;
    
    // Movement LFO (for modulation matrix)
    LFO* movementLFO = nullptr;  // Shared LFO instance from processor
    
    //==============================================================================
    // RT-Safe Noise Generators (preallocated)
    struct NoiseGenerators
    {
        // White noise (mutable to allow modification in const methods)
        mutable std::mt19937 whiteRng{std::random_device{}()};
        mutable std::uniform_real_distribution<float> whiteDist{-1.0f, 1.0f};
        
        // Pink noise (1/f)
        float pinkState[7] = {0.0f};
        
        // Mauve noise (1/f²)
        float mauveState[12] = {0.0f};
        
        float generateWhite() const
        {
            return whiteDist(whiteRng);
        }
        
        float generatePink()
        {
            // Paul Kellet's pink noise algorithm
            float white = generateWhite();
            pinkState[0] = 0.99886f * pinkState[0] + white * 0.0555179f;
            pinkState[1] = 0.99332f * pinkState[1] + white * 0.0750759f;
            pinkState[2] = 0.96900f * pinkState[2] + white * 0.1538520f;
            pinkState[3] = 0.86650f * pinkState[3] + white * 0.3104856f;
            pinkState[4] = 0.55000f * pinkState[4] + white * 0.5329522f;
            pinkState[5] = -0.7616f * pinkState[5] - white * 0.0168980f;
            float output = pinkState[0] + pinkState[1] + pinkState[2] + pinkState[3] + pinkState[4] + pinkState[5] + pinkState[6] + white * 0.5362f;
            pinkState[6] = white * 0.115926f;
            return output * 0.11f; // Scale down
        }
        
        float generateMauve()
        {
            // Brownian/red noise (1/f²)
            float white = generateWhite();
            for (int i = 0; i < 12; ++i)
                mauveState[i] = mauveState[i] * 0.996f + white * 0.004f;
            return mauveState[11] * 3.5f; // Scale up
        }
        
        void reset()
        {
            std::fill(std::begin(pinkState), std::end(pinkState), 0.0f);
            std::fill(std::begin(mauveState), std::end(mauveState), 0.0f);
        }
    };
    
    NoiseGenerators noiseGen;
    
    //==============================================================================
    // Drift simulation (per-stroke)
    float driftPhase = 0.0f;
    float driftSpeed = 0.1f;
    
    //==============================================================================
    // Helper methods
    void updateDrift();
    void updateBpmModulation();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudityVoiceModule)
};