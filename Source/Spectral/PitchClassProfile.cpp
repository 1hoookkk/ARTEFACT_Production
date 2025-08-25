#include "PitchClassProfile.h"
#include <cmath>
#include <algorithm>

static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

PitchClassProfile::PCP PitchClassProfile::computePCP(const float* mags, std::size_t n, double sampleRate) const
{
    PCP p{};
    if (!mags || n == 0 || sampleRate <= 0) return p;
    // Very rough bin->midi mapping assuming linear bin spacing (for stub only)
    for (std::size_t i = 1; i < n; ++i)
    {
        // Approximate frequency for bin i (assuming FFT size == 2*n for stub)
        double hz = (double)i * (sampleRate / double(2 * n));
        if (hz <= 20.0 || hz >= 20000.0) continue;
        double midi = 69.0 + 12.0 * std::log2(hz / 440.0);
        int pc = ((int)std::round(midi)) % 12;
        if (pc < 0) pc += 12;
        p[(std::size_t)pc] += (float)std::max(0.0, (double)mags[i]);
    }
    // Normalize
    float sum = 0.0f; for (auto v : p) sum += v; if (sum > 1e-9f) for (auto& v : p) v /= sum;
    return p;
}

PitchClassProfile::Estimate PitchClassProfile::estimateScale(const PCP& pcp) const
{
    // Major and minor templates (Krumhansl-like, simplified)
    static const float maj[12]  = {6.35f,2.23f,3.48f,2.33f,4.38f,4.09f,2.52f,5.19f,2.39f,3.66f,2.29f,2.88f};
    static const float min_[12] = {6.33f,2.68f,3.52f,5.38f,2.60f,3.53f,2.54f,4.75f,3.98f,2.69f,3.34f,3.17f};

    auto score = [&](const float* tpl, int rot) {
        float s = 0.0f;
        for (int i = 0; i < 12; ++i) s += pcp[(i + rot) % 12] * tpl[i];
        return s;
    };

    float bestScore = -1e9f; int bestRoot = 0; const char* bestMode = "Major";
    for (int r = 0; r < 12; ++r)
    {
        float sMaj = score(maj, r);
        float sMin = score(min_, r);
        if (sMaj > bestScore) { bestScore = sMaj; bestRoot = r; bestMode = "Major"; }
        if (sMin > bestScore) { bestScore = sMin; bestRoot = r; bestMode = "Minor"; }
    }

    // Confidence: normalized difference between best and average
    float avg = 0.0f; for (int r = 0; r < 12; ++r) { avg += score(maj, r); avg += score(min_, r); }
    avg /= 24.0f;
    float conf = bestScore - avg;
    // crude scaling to 0..1
    conf = clamp01(0.5f + 0.5f * (conf / (avg + 1e-6f)));
    return { bestRoot, bestMode, conf };
}

