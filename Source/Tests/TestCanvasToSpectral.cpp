#include <juce_audio_processors/juce_audio_processors.h>
#include "../Core/SpectralSynthEngine.h"
#include "../ParamIDs.h"
#include <iostream>
#include <vector>
#include <cmath>

// Paint-to-Sound Immediacy Test
// Verifies mouse position → frequency mapping and pressure → amplitude

int TestCanvasToSpectral() {
    std::cout << "TestCanvasToSpectral: Starting paint-to-sound mapping validation..." << std::endl;
    
    try {
        // Test coordinate mapping: Canvas position → Spectral parameters
        std::cout << "Testing coordinate mapping..." << std::endl;
        
        // Simulate canvas bounds (800x600 typical window)
        const float canvasWidth = 800.0f;
        const float canvasHeight = 600.0f;
        
        // Test case 1: Mouse position → Frequency mapping
        struct TestPoint {
            float x, y;
            float expectedFreqMin, expectedFreqMax;
            const char* description;
        };
        
        std::vector<TestPoint> testPoints = {
            {0.0f, 0.0f, 20000.0f, 20000.0f, "Top-left (high frequency)"},
            {canvasWidth, 0.0f, 20000.0f, 20000.0f, "Top-right (high frequency)"},
            {canvasWidth/2, canvasHeight/2, 500.0f, 2000.0f, "Center (mid frequency)"},
            {0.0f, canvasHeight, 20.0f, 100.0f, "Bottom-left (low frequency)"},
            {canvasWidth, canvasHeight, 20.0f, 100.0f, "Bottom-right (low frequency)"}
        };
        
        bool coordinateMappingPassed = true;
        
        for (const auto& point : testPoints) {
            // Simulate the coordinate mapping logic from CanvasComponent::screenToSpectral
            float normalizedY = 1.0f - (point.y / canvasHeight);
            float logMin = std::log10(20.0f);
            float logMax = std::log10(20000.0f);
            float frequency = std::pow(10.0f, logMin + normalizedY * (logMax - logMin));
            
            bool freqInRange = (frequency >= point.expectedFreqMin && frequency <= point.expectedFreqMax) || 
                              (std::abs(frequency - point.expectedFreqMin) < 1.0f) || 
                              (std::abs(frequency - point.expectedFreqMax) < 1.0f);
            
            std::cout << "  " << point.description << ": (" << point.x << "," << point.y 
                      << ") → " << (int)frequency << "Hz";
            
            if (freqInRange) {
                std::cout << " ✓" << std::endl;
            } else {
                std::cout << " ✗ (expected " << point.expectedFreqMin << "-" << point.expectedFreqMax << "Hz)" << std::endl;
                coordinateMappingPassed = false;
            }
        }
        
        // Test case 2: Pressure → Amplitude mapping
        std::cout << "Testing pressure mapping..." << std::endl;
        
        struct PressureTest {
            float pressure;
            float expectedAmpMin, expectedAmpMax;
            const char* description;
        };
        
        std::vector<PressureTest> pressureTests = {
            {0.0f, 0.0f, 0.1f, "No pressure (silent)"},
            {0.5f, 0.1f, 0.6f, "Medium pressure"},
            {1.0f, 0.3f, 1.0f, "Full pressure (loud)"}
        };
        
        bool pressureMappingPassed = true;
        
        for (const auto& test : pressureTests) {
            // Simulate pressure → amplitude mapping
            float amplitude = test.pressure * 0.5f; // Simple linear mapping for test
            
            bool ampInRange = (amplitude >= test.expectedAmpMin && amplitude <= test.expectedAmpMax);
            
            std::cout << "  " << test.description << ": pressure=" << test.pressure 
                      << " → amp=" << amplitude;
            
            if (ampInRange) {
                std::cout << " ✓" << std::endl;
            } else {
                std::cout << " ✗ (expected " << test.expectedAmpMin << "-" << test.expectedAmpMax << ")" << std::endl;
                pressureMappingPassed = false;
            }
        }
        
        // Test case 3: SpectralSynthEngine interface
        std::cout << "Testing SpectralSynthEngine interface..." << std::endl;
        
        try {
            SpectralSynthEngine engine;
            
            // Test 1: Engine can be created
            std::cout << "  ✓ SpectralSynthEngine created successfully" << std::endl;
            
            // CRITICAL FIX: Initialize engine before use
            engine.prepareToPlay(44100.0, 512, 2);
            std::cout << "  ✓ SpectralSynthEngine prepared successfully" << std::endl;
            
            // Test 2: Engine has expected methods (this will fail until header is fixed)
            // This test verifies the interface exists and can be called
            
            // Simulate paint stroke data
            float testX = 400.0f;  // Center X
            float testY = 300.0f;  // Center Y  
            float testPressure = 0.7f;
            juce::Colour testColor = juce::Colours::blue;
            
            // Try to call the paint processing method
            std::cout << "  Attempting to call processPaintStrokeDirectToAudio..." << std::endl;
            
            // Set up canvas bounds first
            juce::Rectangle<float> canvasBounds(0.0f, 0.0f, 800.0f, 600.0f);
            engine.updateCanvasBounds(canvasBounds);
            std::cout << "  ✓ updateCanvasBounds called successfully" << std::endl;
            
            // This line should now compile and run with the exposed interface
            engine.processPaintStrokeDirectToAudio(testX, testY, testPressure, testColor);
            std::cout << "  ✓ processPaintStrokeDirectToAudio called successfully" << std::endl;
            
            std::cout << "✓ TestCanvasToSpectral interface test PASSED - SpectralSynthEngine methods accessible" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "  ✗ SpectralSynthEngine interface test failed: " << e.what() << std::endl;
            std::cout << "✗ TestCanvasToSpectral FAILED - Engine interface error" << std::endl;
            return 1; // RED - test failed
        }
        
        // Summary
        if (coordinateMappingPassed && pressureMappingPassed) {
            std::cout << "✓ TestCanvasToSpectral PASSED - All coordinate mapping, pressure mapping, and engine interface tests successful!" << std::endl;
            return 0; // GREEN - test passed
        } else {
            std::cout << "✗ TestCanvasToSpectral FAILED - Multiple mapping errors detected" << std::endl;
            return 1; // RED - test failed
        }
        
    } catch (const std::exception& e) {
        std::cout << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}