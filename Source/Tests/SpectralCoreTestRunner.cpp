// SpectralCoreTestRunner.cpp
// Minimal test runner for TDD resurrection
#include <iostream>

// Declare external test functions
extern int TestSpectralSmoke();
extern int TestAPVTSParams();
extern int TestCanvasToSpectral();

int main(int argc, char* argv[])
{
    std::cout << "=== SpectralCanvas Pro Core Tests ===" << std::endl;
    std::cout << "TDD Resurrection Phase 1" << std::endl;
    std::cout << std::endl;
    
    int totalFailures = 0;
    
    // Run TestSpectralSmoke
    std::cout << "Running TestSpectralSmoke..." << std::endl;
    int smokeResult = TestSpectralSmoke();
    totalFailures += smokeResult;
    std::cout << "TestSpectralSmoke: " << (smokeResult == 0 ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    // Run TestAPVTSParams  
    std::cout << "Running TestAPVTSParams..." << std::endl;
    int apvtsResult = TestAPVTSParams();
    totalFailures += apvtsResult;
    std::cout << "TestAPVTSParams: " << (apvtsResult == 0 ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    // Run TestCanvasToSpectral
    std::cout << "Running TestCanvasToSpectral..." << std::endl;
    int canvasResult = TestCanvasToSpectral();
    totalFailures += canvasResult;
    std::cout << "TestCanvasToSpectral: " << (canvasResult == 0 ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    // Summary
    std::cout << "=== Test Results ===" << std::endl;
    std::cout << "Total failures: " << totalFailures << std::endl;
    
    if (totalFailures == 0) {
        std::cout << "🎯 ALL TESTS PASSED - SpectralCanvas Pro Core is GREEN!" << std::endl;
        return 0;
    } else {
        std::cout << "❌ " << totalFailures << " test(s) FAILED - needs fixing" << std::endl;
        return 1;
    }
}