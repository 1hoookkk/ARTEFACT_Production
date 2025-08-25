/**
 * LFO - Low Frequency Oscillator with BPM Sync
 * Provides tempo-synced modulation for Movement knob and other parameters
 */

#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <cmath>

class LFO
{
public:
    LFO();
    ~LFO() = default;
    
    enum WaveShape
    {
        Sine = 0,
        Triangle = 1,
        Square = 2,
        Sawtooth = 3,
        Random = 4
    };
    
    enum SyncRate
    {
        Rate_16th = 0,  // 1/16
        Rate_8th = 1,   // 1/8
        Rate_4th = 2,   // 1/4
        Rate_2nd = 3,   // 1/2
        Rate_1st = 4,   // 1/1 (whole note)
        Rate_2bars = 5, // 2/1
        Rate_4bars = 6  // 4/1
    };
    
    // Audio processing
    void prepareToPlay(double sampleRate);
    float getNextSample();
    void reset();
    
    // Parameter control (thread-safe for UI)
    void setDepth(float depth);           // 0.0 - 1.0
    void setRate(SyncRate rate);          // Sync rate enum
    void setWaveShape(WaveShape shape);   // Waveform
    void setBPMSync(bool enabled);        // Enable/disable BPM sync
    void setFreeRate(float hz);           // Free-running rate (Hz)
    
    // Host sync
    void setHostBPM(double bpm);
    void setHostPosition(double positionInBeats);
    void setHostPlaying(bool isPlaying);
    
    // Current state (for UI display)
    float getCurrentPhase() const { return phase; }
    float getCurrentValue() const { return currentValue; }
    bool isBPMSynced() const { return bpmSyncEnabled; }
    
private:
    // Audio thread state
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float currentValue = 0.0f;
    float phaseIncrement = 0.0f;
    
    // Parameters (atomic for thread safety)
    std::atomic<float> depth{0.0f};
    std::atomic<int> syncRate{Rate_4th};
    std::atomic<int> waveShape{Sine};
    std::atomic<bool> bpmSyncEnabled{true};
    std::atomic<float> freeRate{1.0f};
    
    // Host sync
    std::atomic<double> hostBPM{120.0};
    std::atomic<double> hostPosition{0.0};
    std::atomic<bool> hostPlaying{false};
    
    // Random generator for random waveform
    juce::Random randomGen;
    float randomValue = 0.0f;
    int randomUpdateCounter = 0;
    
    // Internal methods
    void updatePhaseIncrement();
    float generateWaveform() const;
    double getSyncRateInBeats(SyncRate rate) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFO)
};