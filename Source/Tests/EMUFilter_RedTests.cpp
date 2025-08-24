/**
 * EMUFilter_RedTests.cpp - Comprehensive RED Test Suite for EMUFilter
 * 
 * This is a deterministic, seedless test suite designed to comprehensively
 * validate EMUFilter behavior. Some tests are designed to FAIL with the current
 * stub implementation to ensure proper RED phase behavior.
 * 
 * Test Categories:
 * - API Basic Tests
 * - DC Tracking Tests  
 * - High-Frequency Attenuation Tests
 * - Resonance Tests
 * - Sample Rate Switching Tests
 * - Denormal Handling Tests
 */

#include "../Core/EMUFilter.h"
#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace testutils {
    // Test helper functions
    std::vector<float> generateSine(float frequency, float sampleRate, int numSamples) {
        std::vector<float> buffer(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * i / sampleRate);
        }
        return buffer;
    }
    
    std::vector<float> generateDC(float level, int numSamples) {
        return std::vector<float>(numSamples, level);
    }
    
    std::vector<float> generateWhiteNoise(int numSamples) {
        std::vector<float> buffer(numSamples);
        juce::Random rng(12345); // Fixed seed for deterministic tests
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = rng.nextFloat() * 2.0f - 1.0f;
        }
        return buffer;
    }
    
    float calculateRMS(const std::vector<float>& buffer) {
        float sum = 0.0f;
        for (float sample : buffer) {
            sum += sample * sample;
        }
        return std::sqrt(sum / buffer.size());
    }
    
    float calculateDCLevel(const std::vector<float>& buffer) {
        return std::accumulate(buffer.begin(), buffer.end(), 0.0f) / buffer.size();
    }
    
    std::vector<float> processBuffer(EMUFilter& filter, const std::vector<float>& input) {
        juce::AudioBuffer<float> buffer(1, static_cast<int>(input.size()));
        for (size_t i = 0; i < input.size(); ++i) {
            buffer.setSample(0, static_cast<int>(i), input[i]);
        }
        filter.processBlock(buffer);
        
        std::vector<float> output(input.size());
        for (size_t i = 0; i < input.size(); ++i) {
            output[i] = buffer.getSample(0, static_cast<int>(i));
        }
        return output;
    }
    
    bool containsDenormals(const std::vector<float>& buffer) {
        for (float sample : buffer) {
            if (std::fpclassify(sample) == FP_SUBNORMAL) {
                return true;
            }
        }
        return false;
    }
}

class EMUFilterRedTests : public juce::UnitTest
{
public:
    EMUFilterRedTests() : juce::UnitTest("EMUFilter RED Tests") {}
    
    void runTest() override
    {
        beginTest("API Basic Tests");
        testBasicAPI();
        
        beginTest("DC Tracking Tests");
        testDCTracking();
        
        beginTest("High-Frequency Attenuation Tests");
        testHighFrequencyAttenuation();
        
        beginTest("Resonance Tests");
        testResonance();
        
        beginTest("Sample Rate Switching Tests");
        testSampleRateSwitching();
        
        beginTest("Denormal Handling Tests");
        testDenormalHandling();
    }
    
private:
    void testBasicAPI()
    {
        // Test 1: Constructor should not crash
        EMUFilter filter;
        expect(true, "Constructor should complete without crash");
        
        // Test 2: prepare should accept valid sample rates
        filter.prepare(44100.0, 512);
        filter.prepare(48000.0, 512);
        filter.prepare(96000.0, 512);
        expect(true, "prepare should accept standard sample rates");
        
        // Test 3: setParams should accept valid frequencies and Q values
        filter.setParams(1000.0f, 0.707f);
        filter.setParams(20.0f, 0.1f);
        filter.setParams(20000.0f, 10.0f);
        expect(true, "setParams should accept audio range frequencies and Q values");
        
        // Test 5: processBlock should not crash with zero input
        juce::AudioBuffer<float> testBuffer(1, 10);
        testBuffer.clear();
        filter.processBlock(testBuffer);
        float result = testBuffer.getSample(0, 0);
        expect(!std::isnan(result), "processBlock should not return NaN for zero input");
        expect(!std::isinf(result), "processBlock should not return infinity for zero input");
        
        // Test 6: processBlock should handle normal audio levels
        testBuffer.setSample(0, 0, 0.5f);
        filter.processBlock(testBuffer);
        result = testBuffer.getSample(0, 0);
        expect(!std::isnan(result), "processBlock should not return NaN for normal input");
        expect(!std::isinf(result), "processBlock should not return infinity for normal input");
        
        // Test 7: reset should not crash
        filter.reset();
        expect(true, "reset should complete without crash");
    }
    
    void testDCTracking()
    {
        EMUFilter filter;
        filter.prepare(44100.0, 512);
        filter.setParams(1000.0f, 0.707f);
        
        // Test 1: Pure DC input should be significantly attenuated
        auto dcInput = testutils::generateDC(1.0f, 1000);
        auto output = testutils::processBuffer(filter, dcInput);
        float outputDC = testutils::calculateDCLevel(output);
        
        // For a proper highpass filter, DC should be heavily attenuated
        expectWithinAbsoluteError(outputDC, 0.0f, 0.1f, 
            "DC component should be significantly attenuated (expected near 0, got " + 
            juce::String(outputDC, 6) + ")");
        
        // Test 2: Low frequency sine should be attenuated more than high frequency
        filter.reset();
        filter.setParams(1000.0f, 0.707f);
        
        auto lowFreqSine = testutils::generateSine(100.0f, 44100.0f, 2048);
        auto highFreqSine = testutils::generateSine(5000.0f, 44100.0f, 2048);
        
        auto lowOutput = testutils::processBuffer(filter, lowFreqSine);
        filter.reset();
        auto highOutput = testutils::processBuffer(filter, highFreqSine);
        
        float lowRMS = testutils::calculateRMS(lowOutput);
        float highRMS = testutils::calculateRMS(highOutput);
        
        // High frequency should have higher amplitude than low frequency
        expect(highRMS > lowRMS * 1.5f, 
            "High frequency should be less attenuated than low frequency (low RMS: " +
            juce::String(lowRMS, 6) + ", high RMS: " + juce::String(highRMS, 6) + ")");
    }
    
    void testHighFrequencyAttenuation()
    {
        EMUFilter filter;
        filter.prepare(44100.0, 512);
        filter.setParams(1000.0f, 0.707f);
        
        // Test passband vs stopband
        auto passbandSine = testutils::generateSine(5000.0f, 44100.0f, 2048);  // Well above cutoff
        auto stopbandSine = testutils::generateSine(100.0f, 44100.0f, 2048);   // Well below cutoff
        
        auto passbandOutput = testutils::processBuffer(filter, passbandSine);
        filter.reset();
        auto stopbandOutput = testutils::processBuffer(filter, stopbandSine);
        
        float passbandRMS = testutils::calculateRMS(passbandOutput);
        float stopbandRMS = testutils::calculateRMS(stopbandSine); // Input RMS for comparison
        float attenuatedRMS = testutils::calculateRMS(stopbandOutput);
        
        // Stopband should be significantly attenuated compared to input
        float attenuationRatio = attenuatedRMS / stopbandRMS;
        expect(attenuationRatio < 0.5f, 
            "Stopband frequencies should be significantly attenuated (attenuation ratio: " +
            juce::String(attenuationRatio, 6) + ", should be < 0.5)");
        
        // Passband should have minimal attenuation
        float passbandInputRMS = testutils::calculateRMS(passbandSine);
        float passbandRatio = passbandRMS / passbandInputRMS;
        expect(passbandRatio > 0.7f,
            "Passband frequencies should have minimal attenuation (passband ratio: " +
            juce::String(passbandRatio, 6) + ", should be > 0.7)");
    }
    
    void testResonance()
    {
        EMUFilter filter;
        filter.prepare(44100.0, 512);
        filter.setParams(1000.0f, 0.707f);
        
        // Test different resonance settings at cutoff frequency
        auto testSignal = testutils::generateSine(1000.0f, 44100.0f, 2048);
        
        // Low resonance
        filter.setParams(1000.0f, 0.5f);
        filter.reset();
        auto lowQOutput = testutils::processBuffer(filter, testSignal);
        float lowQRMS = testutils::calculateRMS(lowQOutput);
        
        // High resonance
        filter.setParams(1000.0f, 5.0f);
        filter.reset();
        auto highQOutput = testutils::processBuffer(filter, testSignal);
        float highQRMS = testutils::calculateRMS(highQOutput);
        
        // Higher Q should produce higher amplitude at cutoff frequency
        expect(highQRMS > lowQRMS * 1.2f,
            "Higher resonance should increase amplitude at cutoff frequency (low Q RMS: " +
            juce::String(lowQRMS, 6) + ", high Q RMS: " + juce::String(highQRMS, 6) + ")");
        
        // Test stability - high Q should not cause instability
        expect(!std::isnan(highQRMS), "High resonance should not cause NaN output");
        expect(!std::isinf(highQRMS), "High resonance should not cause infinite output");
        expect(highQRMS < 10.0f, "High resonance should not cause excessive gain");
    }
    
    void testSampleRateSwitching()
    {
        EMUFilter filter;
        
        // Test frequency response consistency across sample rates
        std::vector<float> sampleRates = {44100.0f, 48000.0f, 96000.0f};
        std::vector<float> responses;
        
        for (float sampleRate : sampleRates) {
            filter.prepare(sampleRate, 512);
            filter.setParams(1000.0f, 0.707f);
            filter.reset();
            
            // Generate test signal at 2kHz (above cutoff)
            auto testSignal = testutils::generateSine(2000.0f, sampleRate, 2048);
            auto output = testutils::processBuffer(filter, testSignal);
            float rms = testutils::calculateRMS(output);
            responses.push_back(rms);
        }
        
        // Responses should be reasonably consistent across sample rates
        float minResponse = *std::min_element(responses.begin(), responses.end());
        float maxResponse = *std::max_element(responses.begin(), responses.end());
        float variation = (maxResponse - minResponse) / minResponse;
        
        expect(variation < 0.3f,
            "Filter response should be consistent across sample rates (variation: " +
            juce::String(variation, 6) + ", should be < 0.3)");
    }
    
    void testDenormalHandling()
    {
        EMUFilter filter;
        filter.prepare(44100.0, 512);
        filter.setParams(1000.0f, 0.707f);
        
        // Test 1: Very small input values (potential denormals)
        std::vector<float> smallValues = {1e-20f, -1e-20f, 1e-30f, -1e-30f};
        juce::AudioBuffer<float> testBuffer(1, 1);
        for (float value : smallValues) {
            testBuffer.setSample(0, 0, value);
            filter.processBlock(testBuffer);
            float result = testBuffer.getSample(0, 0);
            expect(!std::isnan(result), "Small input should not produce NaN");
            expect(!std::isinf(result), "Small input should not produce infinity");
        }
        
        // Test 2: Process silence and check for denormal generation
        auto silence = testutils::generateDC(0.0f, 1000);
        auto output = testutils::processBuffer(filter, silence);
        
        bool hasDenormals = testutils::containsDenormals(output);
        expect(!hasDenormals, "Processing silence should not generate denormal numbers");
        
        // Test 3: Impulse response should decay properly
        filter.reset();
        std::vector<float> impulse(1000, 0.0f);
        impulse[0] = 1.0f;
        
        auto impulseResponse = testutils::processBuffer(filter, impulse);
        
        // Check that response eventually approaches zero
        float tailLevel = testutils::calculateRMS(std::vector<float>(
            impulseResponse.begin() + 800, impulseResponse.end()));
        
        expect(tailLevel < 1e-6f,
            "Impulse response should decay to near zero (tail level: " +
            juce::String(tailLevel, 10) + ")");
        
        // Check no denormals in impulse response
        bool impulseHasDenormals = testutils::containsDenormals(impulseResponse);
        expect(!impulseHasDenormals, "Impulse response should not contain denormal numbers");
    }
};

// Register the test
static EMUFilterRedTests emuFilterRedTests;

// Standalone main for CTest integration
int main(int argc, char* argv[])
{
    juce::UnitTestRunner runner;
    runner.setAssertOnFailure(false);
    
    // Create array of tests to run
    juce::Array<juce::UnitTest*> tests;
    tests.add(&emuFilterRedTests);
    runner.runTests(tests);
    
    int numResults = runner.getNumResults();
    int failures = 0;
    
    for (int i = 0; i < numResults; ++i) {
        auto* result = runner.getResult(i);
        if (result->failures > 0) {
            failures += result->failures;
            std::cout << "FAILED: " << result->subcategoryName << std::endl;
            for (auto& message : result->messages) {
                std::cout << "  " << message << std::endl;
            }
        } else {
            std::cout << "PASSED: " << result->subcategoryName << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== EMUFilter RED Test Results ===" << std::endl;
    std::cout << "Total tests: " << numResults << std::endl;
    std::cout << "Failures: " << failures << std::endl;
    std::cout << "Status: " << (failures == 0 ? "ALL PASSED" : "SOME FAILED") << std::endl;
    
    return failures > 0 ? 1 : 0;
}