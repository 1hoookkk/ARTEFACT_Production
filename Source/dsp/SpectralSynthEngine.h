#pragma once
#include <JuceHeader.h>
#include "SpscRing.h"
#include "PaintEvent.h"
#include "VoicePool.h"
#include "../Core/HarmonicQuantizer.h"

class SpectralSynthEngine
{
public:
    void prepare(double sampleRate, int maximumBlockSize, int maxPartials, int voices = 64)
    {
        sr = sampleRate;
        maxBlock = maximumBlockSize;
        pool = std::make_unique<VoicePool>(voices);
        pool->prepare(sr, maxBlock, maxPartials);
        
        // Pre-allocate mix buffer to avoid allocations in audio thread
        monoMixBuffer.setSize(1, maximumBlockSize, false, false, true);
        monoMixBuffer.clear();
    }

    // Called by UI thread - converts from existing PaintEvent format to new format
    bool postPaintEvent(float normalizedX, float normalizedY, float pressure, uint32_t flags = 0)
    {
        // Convert normalized Y position to frequency using HarmonicQuantizer
        const float minFreq = 80.0f;
        const float maxFreq = 2000.0f;
        const float baseHz = minFreq + normalizedY * (maxFreq - minFreq);
        
        // Apply harmonic quantization based on pressure
        const double sigmaCents = scp::pressureToSigmaCents(pressure, 200.0, 8.0);
        double weightCents;
        const float quantizedHz = static_cast<float>(
            scp::computeSnappedFrequencyCmaj(baseHz, sigmaCents, weightCents)
        );
        
        // Create paint event with quantized frequency
        PaintEvent ev {
            quantizedHz,                                    // baseHz - quantized to C major
            juce::jlimit(0.1f, 1.0f, pressure),           // amplitude based on pressure
            (normalizedX - 0.5f) * 2.0f,                   // pan from X position (-1 to +1)
            static_cast<uint16_t>(8 + pressure * 8),       // partials based on pressure (8-16)
            0                                               // sampleOffset
        };
        
        return queue.push(ev);
    }

    // Called by audio thread (once per processBlock)
    void process(juce::AudioBuffer<float>& buffer)
    {
        // Clear the buffer to start fresh (we're generating, not processing input)
        buffer.clear();
        
        // Consume queued paint events and spawn voices
        PaintEvent ev;
        while (queue.pop(ev))
        {
            if (auto* v = pool->allocate())
                v->noteOn(ev.baseHz, ev.amplitude, ev.partials, ev.pan);
        }

        // Render active voices into buffer (in-place)
        pool->render(buffer);
        
        // Apply master gain
        const float masterGain = masterGainAtomic.load();
        buffer.applyGain(masterGain);
    }
    
    // RT-safe parameter updates
    void setMasterGain(float gainLinear) noexcept { masterGainAtomic.store(gainLinear); }
    
    // Status queries
    bool isInitialized() const noexcept { return pool != nullptr; }
    size_t getQueueSize() const noexcept { return queue.size(); }

private:
    double sr = 44100.0;
    int    maxBlock = 512;

    // Tune capacity as needed for your GUI event rate
    SpscRing<PaintEvent, 1024> queue;

    std::unique_ptr<VoicePool> pool;
    
    // Pre-allocated buffer to avoid allocations in audio thread
    juce::AudioBuffer<float> monoMixBuffer;
    
    // RT-safe atomic parameters
    std::atomic<float> masterGainAtomic{0.7f};
};