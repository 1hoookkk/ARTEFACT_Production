// Simple smoke test for HarmonicQuantizer helpers
// Returns 0 on success, non-zero on failure.
#include "../Core/HarmonicQuantizer.h"
#include <iostream>
#include <cmath>

int main() {
    using namespace scp;
    // basic mapping sanity checks
    double s0 = pressureToSigmaCents(0.0);
    double s1 = pressureToSigmaCents(1.0);
    if (!(s0 > s1)) {
        std::cerr << "pressureToSigmaCents mapping failed: s0=" << s0 << " s1=" << s1 << "\n";
        return 2;
    }

    // compute weight for an in-scale note (C4 = 261.6256 Hz)
    double c4 = 261.6256;
    double sigma = pressureToSigmaCents(0.8);
    double w_c4 = computeSnapWeightCmaj(c4, sigma);
    if (!(w_c4 > 0.9)) {
        std::cerr << "Expected high weight for exact scale note; got " << w_c4 << "\n";
        return 3;
    }

    // compute a distant frequency (approx D#4 ~ 311 Hz) should have lower weight
    double ds4 = 311.0;
    double w_ds4 = computeSnapWeightCmaj(ds4, sigma);
    if (w_ds4 >= w_c4) {
        std::cerr << "Unexpected weight ordering: w_ds4=" << w_ds4 << " >= w_c4=" << w_c4 << "\n";
        return 4;
    }

    // blend should move a non-scale frequency toward the scale note
    double outWeight = 0.0;
    double blended = computeSnappedFrequencyCmaj(ds4, sigma, outWeight);
    if (!(blended > 0.0)) {
        std::cerr << "Blended frequency invalid: " << blended << "\n";
        return 5;
    }

    // trivial pass
    std::cout << "HarmonicQuantizer smoke test passed. w_c4=" << w_c4 << " w_ds4=" << w_ds4 << " blended=" << blended << "\n";
    return 0;
}