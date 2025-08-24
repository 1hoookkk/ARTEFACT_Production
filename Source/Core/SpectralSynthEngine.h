#pragma once
#include <JuceHeader.h>
#include "PaintQueue.h"
#include "TransientDetector.h"

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
    
    bool isInitialized() const noexcept { return sampleRate_ > 0.0; }

    void setHarmonicDepth(float v) noexcept { harmonicDepth_.store(v); }
    void setMasterGain(float v) noexcept { masterGain_.store(v); }
    void setNumPartials(int n) noexcept { numPartials_.store( juce::jlimit(1, kMaxPartials, n)); }
    void setMaxVoices(int v) noexcept { maxVoices_.store( juce::jlimit(1, kMaxVoices, v)); }
    
    // ========== TEMPORARY DEBUG SHIMS ==========
    void releaseResources() noexcept {}
    void setTopNBands(int) noexcept {}
    void processBlock(juce::AudioBuffer<float>&) noexcept {}
    bool isMulticoreActive() const noexcept { return false; }
    int getSeqFallbackCount() const noexcept { return 0; }
    
    struct PaintData { float dummy = 0; };
    struct MaskSnapshot { 
        void setMaskBlend(float) {} 
        void setMaskStrength(float) {}
        void setFeatherTime(float) {}
        void setFeatherFreq(float) {}
        void setThreshold(float) {}
        void setProtectHarmonics(bool) {}
    };
    MaskSnapshot getMaskSnapshot() const noexcept { return MaskSnapshot(); }
    void processPaintStroke(const PaintData&) noexcept {}
    // ========== END TEMPORARY DEBUG SHIMS ==========

private:
    SpectralSynthEngine() noexcept;
    ~SpectralSynthEngine() = default;

    SpectralSynthEngine(const SpectralSynthEngine&) = delete;
    SpectralSynthEngine& operator=(const SpectralSynthEngine&) = delete;

    struct Voice
    {
        bool active = false;
        float baseFreq = 0.0f;
        float amplitudes[64];
        float phases[64];
        float phaseIncs[64];
        float envLevel = 0.0f;
        float envDecay = 0.0f;
        float pan = 0.5f;
        uint32_t ageSamples = 0;
    };

    static constexpr size_t kGestureCapacity = 1024;
    PaintEvent gestureRing_[kGestureCapacity];
    std::atomic<size_t> gestureHead_{0}, gestureTail_{0};

    static constexpr int kMaxVoices = 12;
    static constexpr int kMaxPartials = 64;
    Voice voices_[kMaxVoices];

    static constexpr int kSineTableSize = 2048;
    float sineTable_[kSineTableSize];

    double sampleRate_ = 44100.0;
    int blockSize_ = 128;

    std::atomic<float> harmonicDepth_{0.8f};
    std::atomic<float> masterGain_{0.9f};
    std::atomic<int> numPartials_{16};
    std::atomic<int> maxVoices_{6};
    
    // Amplitude boost for snapped partials
    static constexpr float kHarmonicAmpBoost = 0.15f;
    // Partial culling threshold (-60dB)
    static constexpr float kPartialCullThreshold = 0.001f;
    
    // Transient detection
    scp::TransientDetector transientDetector_;

    inline float sineFromTable(float phase) const noexcept;
    bool tryPopGestureInternal(PaintEvent& out) noexcept;
    void createVoiceFromGesture(const PaintEvent& g) noexcept;
};