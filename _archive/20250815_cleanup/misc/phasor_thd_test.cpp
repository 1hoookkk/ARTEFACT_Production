// THD Test for Phasor Oscillator
// Compile: cl /EHsc /std:c++17 phasor_thd_test.cpp
// Run: phasor_thd_test.exe

#include <iostream>
#include <cmath>
#include <vector>
#include <complex>
#include <iomanip>

struct Phasor {
    float cr = 1.0f;  // current real (cos)
    float ci = 0.0f;  // current imag (sin)
    float rr = 1.0f;  // rotation real
    float ri = 0.0f;  // rotation imag
    int normCounter = 0;
    
    void init(float freq, float sampleRate) {
        float omega = 2.0f * M_PI * freq / sampleRate;
        rr = std::cos(omega);
        ri = std::sin(omega);
        cr = 1.0f;
        ci = 0.0f;
        normCounter = 0;
    }
    
    float step() {
        // Complex multiply: phasor *= rotation
        float newReal = cr * rr - ci * ri;
        float newImag = cr * ri + ci * rr;
        cr = newReal;
        ci = newImag;
        
        // Renormalize every 256 samples
        if (++normCounter >= 256) {
            float m2 = cr * cr + ci * ci;
            if (std::fabs(m2 - 1.0f) > 1e-3f) {
                float invMag = 1.0f / std::sqrt(m2);
                cr *= invMag;
                ci *= invMag;
            }
            normCounter = 0;
        }
        
        return ci;  // Return imaginary part as sine output
    }
};

// Simple FFT for THD calculation
void computeFFT(const std::vector<float>& signal, std::vector<float>& magnitudes) {
    int N = signal.size();
    magnitudes.resize(N/2);
    
    for (int k = 0; k < N/2; ++k) {
        float real = 0, imag = 0;
        for (int n = 0; n < N; ++n) {
            float angle = -2.0f * M_PI * k * n / N;
            real += signal[n] * std::cos(angle);
            imag += signal[n] * std::sin(angle);
        }
        magnitudes[k] = std::sqrt(real * real + imag * imag) / N;
    }
}

int main() {
    const float sampleRate = 48000.0f;
    const float testFreq = 1000.0f;  // 1 kHz test tone
    const int fftSize = 8192;
    const int renderSamples = sampleRate * 2;  // 2 seconds
    
    // Initialize phasor oscillator
    Phasor phasor;
    phasor.init(testFreq, sampleRate);
    
    // Render test signal
    std::vector<float> signal(fftSize, 0.0f);
    for (int i = 0; i < fftSize; ++i) {
        signal[i] = phasor.step();
    }
    
    // Apply Blackman window
    for (int i = 0; i < fftSize; ++i) {
        float a0 = 0.42f, a1 = 0.5f, a2 = 0.08f;
        float window = a0 - a1 * std::cos(2.0f * M_PI * i / (fftSize - 1)) 
                          + a2 * std::cos(4.0f * M_PI * i / (fftSize - 1));
        signal[i] *= window;
    }
    
    // Compute FFT
    std::vector<float> magnitudes;
    computeFFT(signal, magnitudes);
    
    // Find fundamental and harmonics
    int fundBin = (int)(testFreq * fftSize / sampleRate);
    float fundamental = magnitudes[fundBin];
    
    // Calculate THD
    float harmonicSum = 0.0f;
    for (int harmonic = 2; harmonic <= 5; ++harmonic) {
        int bin = fundBin * harmonic;
        if (bin < magnitudes.size()) {
            float mag = magnitudes[bin];
            harmonicSum += mag * mag;
            std::cout << "Harmonic " << harmonic << " at " << (harmonic * testFreq) 
                      << " Hz: " << 20.0f * std::log10(mag / fundamental) << " dB\n";
        }
    }
    
    float thd = std::sqrt(harmonicSum) / fundamental;
    float thdDb = 20.0f * std::log10(thd);
    
    std::cout << "\n=== Phasor Oscillator THD Test Results ===\n";
    std::cout << "Test frequency: " << testFreq << " Hz\n";
    std::cout << "Sample rate: " << sampleRate << " Hz\n";
    std::cout << "FFT size: " << fftSize << " points\n";
    std::cout << "Fundamental magnitude: " << fundamental << "\n";
    std::cout << "THD: " << (thd * 100.0f) << "%\n";
    std::cout << "THD (dB): " << thdDb << " dB\n";
    std::cout << "Target: < -80 dB\n";
    std::cout << "Status: " << (thdDb < -80.0f ? "PASS ✓" : "FAIL ✗") << "\n";
    
    return 0;
}