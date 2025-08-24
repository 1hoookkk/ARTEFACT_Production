#pragma once
#include "Voice.h"
#include <vector>

class VoicePool
{
public:
    explicit VoicePool(int numVoices = 64) { voices.resize(numVoices); }

    void prepare(double sr, int maxBlock, int maxPartials)
    {
        for (auto& v : voices) v.prepare(sr, maxBlock, maxPartials);
    }

    Voice* allocate()
    {
        for (auto& v : voices) if (!v.isActive()) return &v;
        // simple voice steal: take the first one
        return &voices.front();
    }

    void render(juce::AudioBuffer<float>& buf)
    {
        const int n = buf.getNumSamples();
        for (auto& v : voices)
            if (v.isActive())
                v.process(buf, 0, n);
    }

private:
    std::vector<Voice> voices;
};