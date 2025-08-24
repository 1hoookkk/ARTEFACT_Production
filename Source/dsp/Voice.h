#pragma once
#include <JuceHeader.h>
#include <array>

class Voice
{
public:
    void prepare(double sr, int maxBlock, int maxPartialsToUse)
    {
        sampleRate = sr;
        partialsCount = std::clamp(maxPartialsToUse, 1, kMaxPartials);
        env.setSampleRate(sr);
        env.setParameters({ 0.002f, 0.01f, 0.8f, 0.05f }); // snappy defaults
        for (int i = 0; i < kMaxPartials; ++i)
        {
            osc[i].initialise([](float x) { return std::sin(x); }, 2048);
            osc[i].setFrequency(440.0f);
        }
        gain.setRampDurationSeconds(0.0);
        active = false;
    }

    bool isActive() const noexcept { return active; }

    void noteOn(float baseHz, float amp, uint16_t partials, float panIn)
    {
        partialsCount = std::clamp<int>(partials, 1, kMaxPartials);
        baseAmp = juce::jlimit(0.0f, 1.0f, amp);
        pan = juce::jlimit(-1.0f, 1.0f, panIn);

        for (int i = 0; i < partialsCount; ++i)
            osc[i].setFrequency(baseHz * (i + 1), true);

        env.noteOn();
        active = true;
    }

    void noteOff() { env.noteOff(); }

    void process(juce::AudioBuffer<float>& buffer, int start, int num, bool autoDeactivate = true)
    {
        if (! active) return;

        juce::dsp::AudioBlock<float> block (buffer);
        auto left  = block.getChannelPointer(0) + start;
        auto right = block.getNumChannels() > 1 ? block.getChannelPointer(1) + start : nullptr;

        const float panL = 0.5f * (1.0f - pan);
        const float panR = 0.5f * (1.0f + pan);

        for (int n = 0; n < num; ++n)
        {
            float s = 0.0f;
            for (int i = 0; i < partialsCount; ++i)
                s += osc[i].processSample(0.0f) * (1.0f / (i + 1)); // simple 1/h falloff

            const float envVal = env.getNextSample();
            const float out = s * baseAmp * envVal;

            left[n]  += out * panL;
            if (right) right[n] += out * panR;

            // voice silently ends near zero
            if (autoDeactivate && env.isActive() == false && std::abs(out) < 1.0e-5f)
            {
                active = false;
                break;
            }
        }
    }

private:
    static constexpr int kMaxPartials = 64;

    std::array<juce::dsp::Oscillator<float>, kMaxPartials> osc;
    juce::ADSR env;
    juce::dsp::Gain<float> gain;
    double sampleRate = 44100.0;
    int    partialsCount = 1;
    float  baseAmp = 0.0f;
    float  pan = 0.0f;
    bool   active = false;
};