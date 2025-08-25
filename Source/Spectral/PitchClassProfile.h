#pragma once
#include <array>
#include <vector>

/**
 * PitchClassProfile (stub): computes simple pitch-class energies and estimates scale.
 * Replace internals with a proper chromagram/PCP in a later sprint.
 */
class PitchClassProfile
{
public:
    using PCP = std::array<float, 12>; // C..B

    struct Estimate {
        int root = 0;              // 0=C .. 11=B
        const char* mode = "Major"; // mode name
        float confidence = 0.0f;   // 0..1
    };

    // Compute a crude PCP from an FFT magnitude frame
    PCP computePCP(const float* magnitudes, std::size_t numBins, double sampleRate) const;

    // Estimate root/mode with simple template matching for major/minor
    Estimate estimateScale(const PCP& pcp) const;
};

