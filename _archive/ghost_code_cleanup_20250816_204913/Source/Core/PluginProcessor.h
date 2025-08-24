// Source/PluginProcessor.h
#pragma once

#include <JuceHeader.h>

// Crash bisect stage mask (bitfield)
enum class StageMask : unsigned {
  None      = 0,
  Osc       = 1u << 0,  // SpectralSynthEngine/processSpectralOscillators
  Mask      = 1u << 1,  // SampleMaskingEngine
  Filter    = 1u << 2,  // CEM3389/MorphFilter
  Tube      = 1u << 3,  // TubeStage
  Secret    = 1u << 4,  // SecretSauceEngine
  Mix       = 1u << 5   // Mixing/accumulation path
};
inline StageMask operator|(StageMask a, StageMask b){ return StageMask(unsigned(a)|unsigned(b)); }
inline bool stageOn(StageMask m, StageMask f){ return (unsigned(m) & unsigned(f)) != 0; }
#include "Core/Commands.h"
#include "Core/CommandQueue.h"
#include "Core/CrashToggles.h"
#include "Core/ForgeProcessor.h"
#include "Core/PaintEngine.h"
#include "Core/SampleMaskingEngine.h"
#include "Core/ParameterBridge.h"
#include "Core/AudioRecorder.h"
#include "Core/SpectralSynthEngine.h"
#include "Telemetry/HudMetrics.h"

//==============================================================================
// Heartbeat Slice Data Structures
struct SpectralImage 
{ 
    juce::Image image; 
};

struct SynthBand 
{ 
    float frequencyHz = 440.0f; 
    float amplitude = 0.0f; 
    float pan = 0.0f; 
    juce::dsp::Complex<float> phasor{1.0f, 0.0f}; 
};

//==============================================================================
class ARTEFACTAudioProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    ARTEFACTAudioProcessor();
    ~ARTEFACTAudioProcessor() override;

    void prepareToPlay(double, int) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    bool pushCommandToQueue(const Command& newCommand);

    void parameterChanged(const juce::String&, float) override;
    
    // Accessors for GUI
    ForgeProcessor& getForgeProcessor() { return forgeProcessor; }
    PaintEngine& getPaintEngine() { return paintEngine; }
    SampleMaskingEngine& getSampleMaskingEngine() { return sampleMaskingEngine; }
    SpectralSynthEngine& getSpectralSynthEngine() { return spectralSynthEngine; }
    AudioRecorder& getAudioRecorder() { return audioRecorder; }
    
    // Paint Brush System
    void setActivePaintBrush(int slotIndex);
    int getActivePaintBrush() const { return activePaintBrushSlot; }
    void triggerPaintBrush(float canvasY, float pressure = 1.0f);
    void stopPaintBrush();
    
    // Audio Processing Control (prevents feedback when minimized)
    void pauseAudioProcessing();
    void resumeAudioProcessing();
    bool isAudioProcessingPaused() const { return audioProcessingPaused; }
    
    // BPM Sync
    void setTempo(double bpm) { lastKnownBPM = bpm; }
    double getTempo() const { return lastKnownBPM; }

    // Crash bisection debugging
    std::atomic<unsigned> debugStageMask { 0xFFFFFFFFu }; // default: all on
    
    // RT-SAFE PARAMETER ATOMICS: Replace all parameter listener engine calls
    std::atomic<float> masterGainAtomic { 1.0f };
    std::atomic<bool> paintActiveAtomic { true };
    std::atomic<int> processingModeAtomic { 0 }; // Canvas = 0, Forge = 1, Hybrid = 2
    
    // Parameter dirty flags for deferred updates
    std::atomic<bool> masterGainDirty { false };
    std::atomic<bool> paintActiveDirty { false };
    std::atomic<bool> processingModeDirty { false };
    
    // RT-safe telemetry data for Developer HUD
    // Thread-safe atomic counters following CommandQueue.h pattern
    struct RTTelemetry {
        std::atomic<uint64_t> evPushed{0};      // Total events pushed to command queue
        std::atomic<uint64_t> evPopped{0};      // Total events popped from command queue
        std::atomic<int> maxQDepth{0};          // Maximum queue depth observed
        std::atomic<float> lastBlockRMS{0.0f};  // RMS level of last processed audio block
        
        // Reset all counters to zero (thread-safe)
        void reset() {
            evPushed.store(0, std::memory_order_relaxed);
            evPopped.store(0, std::memory_order_relaxed);
            maxQDepth.store(0, std::memory_order_relaxed);
            lastBlockRMS.store(0.0f, std::memory_order_relaxed);
        }
    };
    
    // Lifecycle safety: Track parameter listener registration to prevent static initialization crashes
    std::atomic<bool> parametersListenersRegistered { false };
    
    // Engine preparation guard: Prevent parameter callbacks before prepareToPlay completes
    std::atomic<bool> enginePrepared { false };
    
    // Editor lifecycle guard: Track editor open/close for RT safety
    std::atomic<bool> editorOpen { false };
    
    // RT telemetry member - accessible from both GUI and audio threads
    RTTelemetry rtTelemetry;
    
    // HUD metrics queue for RT-safe telemetry transfer to GUI
    SpectralCanvas::HudQueue hudMetricsQueue;
    std::atomic<uint32_t> metricsSerial{0};
    
    // Public access to parameter tree for UI binding
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts; }
    
    // HUD metrics queue access for GUI
    SpectralCanvas::HudQueue& getHudQueue() noexcept { return hudMetricsQueue; }

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    ForgeProcessor  forgeProcessor;
    PaintEngine paintEngine;
    SampleMaskingEngine sampleMaskingEngine;
    SpectralSynthEngine spectralSynthEngine;
    ParameterBridge parameterBridge;
    AudioRecorder audioRecorder;

    enum class ProcessingMode { Forge = 0, Canvas, Hybrid };
    ProcessingMode currentMode = ProcessingMode::Canvas;

    // Thread-safe command queue
    CommandQueue<512> commandQueue;  // Increased size for better performance
    
    // Command processing methods
    void processCommands();
    void processCommand(const Command& cmd);
    void processForgeCommand(const Command& cmd);
    void processSampleMaskingCommand(const Command& cmd);
    void processPaintCommand(const Command& cmd);
    void processRecordingCommand(const Command& cmd);
    
    // Heartbeat Slice - Paint-to-Audio DSP kernel
    void updateAndRender(juce::AudioBuffer<float>& buffer);

    double lastKnownBPM = 120.0;
    double currentSampleRate = 44100.0;
    
    // Paint brush system
    int activePaintBrushSlot = 0;  // Which ForgeVoice slot is the active brush (0-7)
    
    // Audio processing control
    bool audioProcessingPaused = false;
    
    // Pre-allocated RT-safe buffers to prevent heap allocations in processBlock
    juce::AudioBuffer<float> maskingBuffer;
    juce::AudioBuffer<float> paintBuffer;
    
    // Heartbeat Slice - Paint-to-Audio Engine
    SpectralImage sourceImage;
    std::vector<SynthBand> synthBands;
    double currentColumn = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARTEFACTAudioProcessor)
};
