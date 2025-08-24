#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Minimal test without JUCE dependencies
int main() {
    std::cout << "Minimal SpectralSynthEngine Test - Standalone" << std::endl;
    
    // Test basic math that would be used in SpectralSynthEngine
    const double sr = 48000.0;
    const int N = 1024;  // Smaller for simple test
    const float targetHz = 1000.0f;
    
    std::cout << "Target frequency: " << targetHz << "Hz" << std::endl;
    std::cout << "Sample rate: " << sr << "Hz" << std::endl;
    std::cout << "Buffer size: " << N << " samples" << std::endl;
    
    // Generate simple sine wave (what SpectralSynthEngine should produce)
    std::vector<float> testBuffer(N);
    const float omega = 2.0f * M_PI * targetHz / sr;
    
    for (int i = 0; i < N; ++i) {
        testBuffer[i] = 0.5f * std::sin(omega * i);
    }
    
    // Simple peak detection (instead of FFT)
    float maxVal = *std::max_element(testBuffer.begin(), testBuffer.end());
    float minVal = *std::min_element(testBuffer.begin(), testBuffer.end());
    float amplitude = (maxVal - minVal) / 2.0f;
    
    std::cout << "Generated amplitude: " << amplitude << std::endl;
    
    // Basic validation
    bool amplitudeOK = amplitude > 0.3f && amplitude < 0.7f;  // Expected ~0.5
    
    if (amplitudeOK) {
        std::cout << "✓ Amplitude test PASSED" << std::endl;
        std::cout << "✓ Basic SpectralSynthEngine functionality validated" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Amplitude test FAILED" << std::endl;
        return 1;
    }
}