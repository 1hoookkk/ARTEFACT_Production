#include "SpectralSynthEngine.h"

using namespace juce;

SpectralSynthEngine::SpectralSynthEngine() noexcept
{
    for (int i = 0; i < kSineTableSize; ++i)
        sineTable_[i] = std::sin((double)i / (double)kSineTableSize * MathConstants<double>::twoPi);
}

void SpectralSynthEngine::prepare(double sampleRate, int maxBlockSize) noexcept
{
    sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    blockSize_ = juce::jmax(16, maxBlockSize);
    for (int v = 0; v < kMaxVoices; ++v)
    {
        voices_[v].active = false;
        voices_[v].envLevel = 0.0f;
        voices_[v].ageSamples = 0;
        for (int p=0;p<kMaxPartials;++p) { voices_[v].amplitudes[p] = 0.0f; voices_[v].phases[p] = 0.0f; voices_[v].phaseIncs[p] = 0.0f; }
    }

    gestureHead_.store(0);
    gestureTail_.store(0);
}

inline float SpectralSynthEngine::sineFromTable(float phase) const noexcept
{
    float f = phase * (float)(kSineTableSize / (MathConstants<float>::twoPi));
    int idx = (int)f;
    float frac = f - (float)idx;
    idx &= (kSineTableSize - 1);
    int idx2 = (idx + 1) & (kSineTableSize - 1);
    return sineTable_[idx] + frac * (sineTable_[idx2] - sineTable_[idx]);
}

void SpectralSynthEngine::pushGestureRT(const PaintEvent& g) noexcept
{
    const size_t head = gestureHead_.load(std::memory_order_relaxed);
    const size_t next = (head + 1) & (kGestureCapacity - 1);
    if (next == gestureTail_.load(std::memory_order_acquire))
    {
        size_t t = gestureTail_.load(std::memory_order_relaxed);
        gestureTail_.store((t + 1) & (kGestureCapacity - 1), std::memory_order_release);
    }
    gestureRing_[head] = g;
    gestureHead_.store(next, std::memory_order_release);
}

bool SpectralSynthEngine::tryPopGestureInternal(PaintEvent& out) noexcept
{
    const size_t t = gestureTail_.load(std::memory_order_relaxed);
    if (t == gestureHead_.load(std::memory_order_acquire))
        return false;
    out = gestureRing_[t];
    gestureTail_.store((t + 1) & (kGestureCapacity - 1), std::memory_order_release);
    return true;
}

void SpectralSynthEngine::createVoiceFromGesture(const PaintEvent& g) noexcept
{
    int chosen = -1;
    int oldestAge = -1;
    for (int i=0;i<maxVoices_.load();++i)
    {
        if (!voices_[i].active) { chosen = i; break; }
        if ((int)voices_[i].ageSamples > oldestAge) { oldestAge = (int)voices_[i].ageSamples; chosen = i; }
    }
    if (chosen < 0) return;

    Voice& v = voices_[chosen];
    v.active = true;
    v.ageSamples = 0;

    float y = juce::jlimit(0.0f, 1.0f, g.ny);
    float low = 55.0f;
    float high = 1760.0f;
    float freq = low * std::pow(high / low, y);
    v.baseFreq = freq;

    v.pan = juce::jlimit(0.0f, 1.0f, g.nx);

    float pressure = juce::jlimit(0.0f, 1.0f, g.pressure <= 0.0f ? 1.0f : g.pressure);
    v.envLevel = pressure;
    float sustainSec = 0.05f + pressure * 1.2f;
    v.envDecay = 1.0f / (float)(std::max(1.0, sustainSec * sampleRate_));

    const int npart = juce::jlimit(1, (int)numPartials_.load(), kMaxPartials);
    float harmonicDepth = harmonicDepth_.load();
    float xf = juce::jlimit(0.0f, 1.0f, g.nx);
    float centerHarm = 1.0f + xf * 4.0f;

    float sigma = 1.0f + (1.0f - harmonicDepth) * 6.0f;

    for (int p=0; p<npart; ++p)
    {
        float harm = (float)(p + 1);
        float diff = (harm - centerHarm);
        float amp = std::expf(- (diff * diff) / (2.0f * sigma * sigma));
        amp *= harmonicDepth * (1.0f / (1.0f + 0.05f * p));
        v.amplitudes[p] = amp * 0.6f * pressure;
        v.phases[p] = 0.0f;
        v.phaseIncs[p] = (float)((2.0 * MathConstants<double>::pi * freq * (p+1)) / sampleRate_);
    }
    for (int p = numPartials_.load(); p < kMaxPartials; ++p)
    {
        v.amplitudes[p] = 0.0f;
        v.phaseIncs[p] = 0.0f;
        v.phases[p] = 0.0f;
    }
}

void SpectralSynthEngine::processAudioBlock(juce::AudioBuffer<float>& buffer, double /*sampleRate*/) noexcept
{
    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    PaintEvent g;
    while (tryPopGestureInternal(g))
    {
        createVoiceFromGesture(g);
    }

    buffer.clear();

    const int npart = juce::jlimit(1, (int)numPartials_.load(), kMaxPartials);
    const float mg = masterGain_.load();

    for (int v = 0; v < maxVoices_.load(); ++v)
    {
        Voice& voice = voices_[v];
        if (!voice.active) continue;

        for (int s = 0; s < numSamples; ++s)
        {
            float sample = 0.0f;
            for (int p = 0; p < npart; ++p)
            {
                float ph = voice.phases[p];
                float sVal = sineFromTable(ph);
                sample += sVal * voice.amplitudes[p];
                ph += voice.phaseIncs[p];
                if (ph >= MathConstants<float>::twoPi) ph -= MathConstants<float>::twoPi;
                voice.phases[p] = ph;
            }
            voice.envLevel -= voice.envDecay;
            if (voice.envLevel <= 0.0001f)
            {
                voice.active = false;
                break;
            }
            sample *= voice.envLevel;

            float drive = 1.0f;
            float driven = sample * drive;
            float clipped = driven - (driven * driven * driven) * (1.0f / 3.0f);

            float outL = clipped * (1.0f - voice.pan) * mg;
            float outR = clipped * (voice.pan) * mg;

            if (numCh > 0)
                buffer.getWritePointer(0)[s] += outL;
            if (numCh > 1)
                buffer.getWritePointer(1)[s] += outR;

            voice.ageSamples++;
        }
    }

    for (int ch = 0; ch < numCh; ++ch)
    {
        float* ptr = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            const float x = ptr[i];
            const float thresh = 0.98f;
            if (x > thresh) ptr[i] = thresh + (x - thresh) * 0.2f;
            else if (x < -thresh) ptr[i] = -thresh + (x + thresh) * 0.2f;
        }
    }
}