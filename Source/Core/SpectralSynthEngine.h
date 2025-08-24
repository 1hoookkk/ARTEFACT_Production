#pragma once
#include <JuceHeader.h>
#include "PaintQueue.h"
#include "HarmonicQuantizer.h"
#include "../dsp/SpscRing.h"
#include "../dsp/VoicePool.h"

class SpectralSynthEngine
{
public:
    static SpectralSynthEngine& instance() noexcept
    {
        static SpectralSynthEngine s;
        return s;
    }

    void prepare(double sampleRate, int maxBlockSize) noexcept;
    void pushGestureRT(const PaintEvent& g) noexcept;
    void processAudioBlock(juce::AudioBuffer<float>& buffer, double sampleRate) noexcept;
    void releaseResources() noexcept;
    
    bool isInitialized() const noexcept { return initialized_; }

    void setHarmonicDepth(float v) noexcept { harmonicDepth_.store(v); }
    void setMasterGain(float v) noexcept { masterGain_.store(v); }
    void setNumPartials(int n) noexcept { numPartials_.store( juce::jlimit(1, kMaxPartials, n)); }
    void setMaxVoices(int v) noexcept { maxVoices_.store( juce::jlimit(1, kMaxVoices, v)); }
    
    // Status queries
    size_t getQueueSize() const noexcept;

private:
    SpectralSynthEngine() noexcept;
    ~SpectralSynthEngine() = default;

    SpectralSynthEngine(const SpectralSynthEngine&) = delete;
    SpectralSynthEngine& operator=(const SpectralSynthEngine&) = delete;

    // Modern JUCE DSP-based voice management
    std::unique_ptr<VoicePool> voicePool_;
    
    // RT-safe SPSC queue using modern implementation
    struct InternalPaintEvent {
        float baseHz;
        float amplitude;
        float pan;
        uint16_t partials;
    };
    SpscRing<InternalPaintEvent, 1024> eventQueue_;

    // Engine state
    double sampleRate_ = 44100.0;
    int blockSize_ = 128;
    bool initialized_ = false;

    // RT-safe atomic parameters
    std::atomic<float> harmonicDepth_{0.8f};
    std::atomic<float> masterGain_{0.7f};
    std::atomic<int> numPartials_{16};
    std::atomic<int> maxVoices_{32};
    
    static constexpr int kMaxPartials = 64;
    static constexpr int kMaxVoices = 64;
    
    // Convert from old PaintEvent format to modern format
    void convertAndEnqueueGesture(const PaintEvent& g) noexcept;
};