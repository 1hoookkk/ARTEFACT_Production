#include "LFO.h"

LFO::LFO()
{
    reset();
}

void LFO::prepareToPlay(double sr)
{
    sampleRate = sr;
    updatePhaseIncrement();
}

float LFO::getNextSample()
{
    if (!hostPlaying && bpmSyncEnabled)
    {
        // If host stopped and BPM sync enabled, hold current value
        return currentValue * depth.load();
    }
    
    // Generate waveform
    currentValue = generateWaveform();
    
    // Advance phase
    phase += phaseIncrement;
    if (phase >= 1.0f)
        phase -= 1.0f;
        
    // Update random value periodically
    if (waveShape.load() == Random)
    {
        randomUpdateCounter++;
        if (randomUpdateCounter >= static_cast<int>(sampleRate * 0.01f)) // Update every 10ms
        {
            randomValue = randomGen.nextFloat() * 2.0f - 1.0f; // -1 to +1
            randomUpdateCounter = 0;
        }
    }
    
    return currentValue * depth.load();
}

void LFO::reset()
{
    phase = 0.0f;
    currentValue = 0.0f;
    randomValue = 0.0f;
    randomUpdateCounter = 0;
    updatePhaseIncrement();
}

void LFO::setDepth(float newDepth)
{
    depth = juce::jlimit(0.0f, 1.0f, newDepth);
}

void LFO::setRate(SyncRate rate)
{
    syncRate = rate;
    updatePhaseIncrement();
}

void LFO::setWaveShape(WaveShape shape)
{
    waveShape = shape;
}

void LFO::setBPMSync(bool enabled)
{
    bpmSyncEnabled = enabled;
    updatePhaseIncrement();
}

void LFO::setFreeRate(float hz)
{
    freeRate = juce::jlimit(0.01f, 20.0f, hz);
    updatePhaseIncrement();
}

void LFO::setHostBPM(double bpm)
{
    hostBPM = juce::jlimit(60.0, 200.0, bpm);
    updatePhaseIncrement();
}

void LFO::setHostPosition(double positionInBeats)
{
    hostPosition = positionInBeats;
    
    if (bpmSyncEnabled)
    {
        // Sync phase to host position
        double syncRateBeats = getSyncRateInBeats(static_cast<SyncRate>(syncRate.load()));
        double phaseFromHost = std::fmod(positionInBeats / syncRateBeats, 1.0);
        phase = static_cast<float>(phaseFromHost);
    }
}

void LFO::setHostPlaying(bool isPlaying)
{
    hostPlaying = isPlaying;
}

void LFO::updatePhaseIncrement()
{
    if (bpmSyncEnabled.load())
    {
        // Calculate phase increment based on BPM and sync rate
        double bpm = hostBPM.load();
        double syncRateBeats = getSyncRateInBeats(static_cast<SyncRate>(syncRate.load()));
        double frequencyHz = (bpm / 60.0) / syncRateBeats;
        phaseIncrement = static_cast<float>(frequencyHz / sampleRate);
    }
    else
    {
        // Free-running mode
        phaseIncrement = freeRate.load() / static_cast<float>(sampleRate);
    }
}

float LFO::generateWaveform() const
{
    switch (waveShape.load())
    {
        case Sine:
            return std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
            
        case Triangle:
            if (phase < 0.5f)
                return (phase * 4.0f) - 1.0f; // Rising edge
            else
                return 3.0f - (phase * 4.0f); // Falling edge
                
        case Square:
            return (phase < 0.5f) ? -1.0f : 1.0f;
            
        case Sawtooth:
            return (phase * 2.0f) - 1.0f;
            
        case Random:
            return randomValue;
            
        default:
            return 0.0f;
    }
}

double LFO::getSyncRateInBeats(SyncRate rate) const
{
    switch (rate)
    {
        case Rate_16th: return 0.25;  // 1/16 note
        case Rate_8th:  return 0.5;   // 1/8 note
        case Rate_4th:  return 1.0;   // 1/4 note
        case Rate_2nd:  return 2.0;   // 1/2 note
        case Rate_1st:  return 4.0;   // whole note
        case Rate_2bars: return 8.0;  // 2 bars
        case Rate_4bars: return 16.0; // 4 bars
        default: return 1.0;
    }
}