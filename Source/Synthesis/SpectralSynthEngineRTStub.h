#pragma once
#include <array>
#include <vector>
#include <atomic>
#include <cmath>
#include <algorithm>
#include "../Core/GestureSnapshot.h"
#include "../Core/HarmonicQuantizer.h"
#include <juce_audio_basics/juce_audio_basics.h>

namespace scp {

struct PaintGestureRT {
    double timeSec = 0.0;
    double yPos = 0.5;     // 0..1
    double pressure = 0.6; // 0..1
};

class SpectralSynthEngineRTStub {
public:
    SpectralSynthEngineRTStub() noexcept { 
        voices.reserve(128); 
        sampleRate = 44100.0;
    }
    
    static SpectralSynthEngineRTStub& instance() noexcept {
        static SpectralSynthEngineRTStub s;
        return s;
    }

    void prepare(double sr, int /*maxBlock*/) noexcept { 
        sampleRate = sr > 0.0 ? sr : 44100.0;
    }

    bool pushGestureRT(const PaintGestureRT& g) noexcept {
        const int next = (writeIndex + 1) & (RING_SIZE - 1);
        if (next == readIndex.load(std::memory_order_acquire)) {
            // ring full -> advance readIndex (drop oldest)
            readIndex.store((readIndex.load(std::memory_order_relaxed) + 1) & (RING_SIZE - 1), std::memory_order_release);
        }
        buffer[writeIndex] = g;
        writeIndex = next;
        return true;
    }

    void drainGesturesToVoices() noexcept {
        while (readIndex.load(std::memory_order_acquire) != writeIndex) {
            PaintGestureRT g = buffer[readIndex.load(std::memory_order_relaxed)];
            readIndex.store((readIndex.load(std::memory_order_relaxed) + 1) & (RING_SIZE - 1), std::memory_order_release);
            createVoiceFromGesture(g);
        }
    }

    void renderInto(juce::AudioBuffer<float>& buffer) noexcept {
        drainGesturesToVoices();
        const int channels = buffer.getNumChannels();
        const int frames = buffer.getNumSamples();
        if (voices.empty()) return;

        for (int n=0; n<frames; ++n) {
            double s = 0.0;
            for (auto &v : voices) {
                s += v.amp * std::sin(v.phase);
                v.phase += v.phaseInc;
                if (v.phase >= twoPi) v.phase -= twoPi;
                v.amp *= 0.9996; // slow decay
            }
            float out = static_cast<float>(s * 0.3); // scale down
            for (int ch=0; ch<channels; ++ch)
                buffer.addSample(ch, n, out);
        }
        // compact quiet voices
        size_t w = 0;
        for (size_t i=0; i<voices.size(); ++i) {
            if (voices[i].amp > 1e-4) { 
                voices[w++] = voices[i]; 
            }
        }
        voices.resize(w);
    }

    void processAudioBlock(juce::AudioBuffer<float>& buffer, double sr) noexcept {
        prepare(sr, buffer.getNumSamples());
        renderInto(buffer);
    }

private:
    struct Voice {
        double freq=440.0, amp=0.0, phase=0.0, phaseInc=0.0;
    };

    void createVoiceFromGesture(const PaintGestureRT& g) noexcept {
        // Map Y position to MIDI range (lower Y = higher pitch)
        double midi = 36.0 + (1.0 - std::clamp(g.yPos, 0.0, 1.0)) * (96.0 - 36.0);
        
        // Quantize to C major via the helper's nearest-midi
        int tgtMidi = computeNearestTargetMidiForScale(midiToFreqDouble(midi), C_MAJOR_PCS);
        double freq = midiToFreqDouble(static_cast<double>(tgtMidi));
        
        Voice v;
        v.freq = freq;
        v.amp = std::clamp(g.pressure, 0.0, 1.0) * 0.8;
        v.phase = 0.0;
        v.phaseInc = (v.freq / sampleRate) * twoPi;
        
        if (voices.size() < voices.capacity()) {
            voices.push_back(v);
        } else {
            voices[nextReplace] = v;
            nextReplace = (nextReplace + 1) % voices.size();
        }
    }

    inline double midiToFreqDouble(double m) noexcept { 
        return 440.0 * std::pow(2.0, (m - 69.0)/12.0); 
    }

    static constexpr int RING_SIZE = 256;
    PaintGestureRT buffer[RING_SIZE];
    int writeIndex = 0;
    std::atomic<int> readIndex{0};

    std::vector<Voice> voices;
    size_t nextReplace = 0;
    double sampleRate = 44100.0;
    const double twoPi = 2.0 * 3.14159265358979323846;
};

} // namespace scp