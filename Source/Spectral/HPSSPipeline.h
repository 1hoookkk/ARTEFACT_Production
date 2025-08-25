#pragma once
#include <vector>
#include <cstddef>

/**
 * HPSSPipeline (stub): lightweight interface for percussive/harmonic soft-mask estimation.
 * This is a visual/data stub for Phase 2 scaffolding — replace with a real algorithm later.
 */
class HPSSPipeline
{
public:
    struct Weights {
        float percussive = 0.5f; // 0..1
        float harmonic   = 0.5f; // 0..1
    };

    struct Config {
        int medianTime = 9;   // median window along time axis
        int medianFreq = 9;   // median window along frequency axis
        float softness = 0.5f; // mask softness 0..1
    };

    void prepare(double sampleRate, int fftSize, int hopSize) {
        (void)sampleRate; (void)fftSize; (void)hopSize; prepared = true;
    }

    void setConfig(const Config& cfg) { config = cfg; }

    // Process a single magnitude spectrum frame and return coarse weights
    // magnitudes: size N frequency bins
    Weights processFrame(const float* magnitudes, std::size_t numBins) {
        if (!prepared || magnitudes == nullptr || numBins == 0)
            return {};

        // Heuristic stub: high spectral flux -> percussive, low flux + narrowband -> harmonic
        float energy = 0.0f;
        float roughness = 0.0f;
        for (std::size_t i = 1; i < numBins; ++i) {
            float a = magnitudes[i-1];
            float b = magnitudes[i];
            energy += b;
            float d = b - a;
            roughness += (d > 0.0f ? d : -d);
        }
        float flux = (energy > 1e-6f) ? (roughness / energy) : 0.0f;
        float perc = std::max(0.0f, std::min(1.0f, flux * 0.5f));
        float harm = 1.0f - perc;
        return { perc, harm };
    }

private:
    bool prepared = false;
    Config config{};
};

