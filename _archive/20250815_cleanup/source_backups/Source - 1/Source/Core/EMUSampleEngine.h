/**
 * EMU Sample Engine
 * Professional sample playback engine for EMU Rompler functionality
 * Supports multi-layer samples, streaming, and EMU-style characteristics
 */

#pragma once
#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <array>
#include <atomic>

/**
 * Individual sample voice with EMU-style characteristics
 */
class EMUSampleVoice
{
public:
    EMUSampleVoice();
    ~EMUSampleVoice() = default;
    
    // Voice lifecycle
    void startNote(int midiNote, float velocity, const juce::AudioSampleBuffer* sample);
    void stopNote(bool allowTailOff);
    bool isActive() const { return active.load(); }
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples);
    
    // EMU-style parameters
    void setPitch(float pitchBend);              // -2.0 to +2.0 semitones
    void setTranspose(int semitones);            // -48 to +48 semitones
    void setFineTune(float cents);               // -100 to +100 cents
    void setLoopMode(bool enabled);
    void setLoopPoints(int start, int end);
    void setVelocityLayer(int layer);            // 0-3 velocity layers
    
    // Modulation inputs (from paint canvas and EMU controls)
    void modulatePitch(float modulation);        // Real-time pitch modulation
    void modulateAmplitude(float modulation);    // Real-time amplitude modulation
    void modulateFilter(float modulation);       // Real-time filter modulation
    
private:
    // Voice state
    std::atomic<bool> active{false};
    int currentNote = -1;
    float currentVelocity = 0.0f;
    
    // Sample playback
    const juce::AudioSampleBuffer* currentSample = nullptr;
    double samplePosition = 0.0;
    double playbackRate = 1.0;
    
    // EMU parameters
    float transpose = 0.0f;
    float fineTune = 0.0f;
    float pitchBend = 0.0f;
    bool loopEnabled = false;
    int loopStart = 0, loopEnd = 0;
    int velocityLayer = 0;
    
    // Modulation values
    float pitchModulation = 0.0f;
    float amplitudeModulation = 1.0f;
    float filterModulation = 0.0f;
    
    // Envelope for amplitude (simple AD for now)
    float envelope = 0.0f;
    float envelopeRate = 0.0f;
    bool isReleasing = false;
    
    // Anti-aliasing filter for pitch shifting
    juce::IIRFilter antiAliasFilter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUSampleVoice)
};

/**
 * EMU Sample Slot
 * Manages sample loading, streaming, and multiple velocity layers
 */
class EMUSampleSlot
{
public:
    EMUSampleSlot();
    ~EMUSampleSlot() = default;
    
    // Sample management
    bool loadSample(const juce::File& sampleFile);
    bool loadSample(const juce::AudioSampleBuffer& sampleBuffer, double sampleRate);
    void clearSample();
    bool hasSample() const { return sampleLoaded.load(); }
    
    // Multi-layer support
    void addVelocityLayer(const juce::AudioSampleBuffer& layer, int minVel, int maxVel);
    void clearVelocityLayers();
    
    // Sample properties
    juce::String getSampleName() const { return sampleName; }
    double getSampleRate() const { return sampleRate; }
    int getSampleLength() const { return sampleBuffer ? sampleBuffer->getNumSamples() : 0; }
    
    // Visual feedback for UI
    struct SampleInfo
    {
        juce::String name;
        double lengthInSeconds;
        int numChannels;
        double sampleRate;
        bool isLooped;
    };
    SampleInfo getSampleInfo() const;
    
    // Access to sample data (for voices)
    const juce::AudioSampleBuffer* getSampleBuffer() const { return sampleBuffer.get(); }
    const juce::AudioSampleBuffer* getVelocityLayer(int velocity) const;
    
private:
    // Sample data
    std::unique_ptr<juce::AudioSampleBuffer> sampleBuffer;
    std::atomic<bool> sampleLoaded{false};
    juce::String sampleName;
    double sampleRate = 44100.0;
    
    // Velocity layers
    struct VelocityLayer
    {
        std::unique_ptr<juce::AudioSampleBuffer> buffer;
        int minVelocity, maxVelocity;
    };
    std::vector<VelocityLayer> velocityLayers;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUSampleSlot)
};

/**
 * Main EMU Sample Engine
 * Manages multiple sample slots and voices for full rompler functionality
 */
class EMUSampleEngine
{
public:
    EMUSampleEngine();
    ~EMUSampleEngine() = default;
    
    // Audio processing
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages);
    void releaseResources();
    
    // Sample slot management (8 slots like classic EMU romplers)
    static constexpr int NUM_SAMPLE_SLOTS = 8;
    void setSampleSlot(int slot, const juce::File& sampleFile);
    void setSampleSlot(int slot, const juce::AudioSampleBuffer& sample, double sampleRate);
    void clearSampleSlot(int slot);
    EMUSampleSlot& getSampleSlot(int slot);
    
    // Global EMU parameters
    void setMasterVolume(float volume);          // 0.0-1.0
    void setMasterTuning(float cents);           // -100 to +100 cents
    void setPitchBendRange(int semitones);       // 1-12 semitones
    void setPolyphony(int maxVoices);            // 1-64 voices
    
    // Paint canvas integration
    void handlePaintStroke(float x, float y, float pressure, juce::Colour color, bool isStart);
    void setXAxisMapping(int mapping);           // 0=SlotSelect, 1=Transpose, 2=StartPoint
    void setYAxisMapping(int mapping);           // 0=Pitch, 1=Volume, 2=Filter
    void setPressureMapping(int mapping);        // 0=Velocity, 1=Volume, 2=Modulation
    
    // Voice management
    int getActiveVoiceCount() const { return activeVoices.load(); }
    void allNotesOff();
    void panicStop();  // Emergency stop all audio
    
    // Performance monitoring
    struct EngineStats
    {
        int activeVoices;
        int totalVoices;
        float cpuUsage;
        int sampleMemoryUsage;  // MB
    };
    EngineStats getEngineStats() const;
    
private:
    // Voice pool
    static constexpr int MAX_VOICES = 64;
    std::array<EMUSampleVoice, MAX_VOICES> voices;
    std::atomic<int> activeVoices{0};
    
    // Sample slots
    std::array<EMUSampleSlot, NUM_SAMPLE_SLOTS> sampleSlots;
    
    // Global parameters
    float masterVolume = 0.8f;
    float masterTuning = 0.0f;
    int pitchBendRange = 2;
    int maxPolyphony = 32;
    
    // Paint-to-sample mapping
    int xAxisMapping = 0;  // Sample slot selection
    int yAxisMapping = 0;  // Pitch control  
    int pressureMapping = 0; // Velocity control
    
    // Audio processing state
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    bool isInitialized = false;
    
    // Performance monitoring
    mutable std::atomic<float> cpuUsage{0.0f};
    
    // Internal methods
    EMUSampleVoice* findFreeVoice();
    EMUSampleVoice* findVoiceForNote(int midiNote);
    void updateVoiceCount();
    int mapPaintToSampleSlot(float x) const;
    float mapPaintToPitch(float y) const;
    float mapPaintToVelocity(float pressure) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMUSampleEngine)
};