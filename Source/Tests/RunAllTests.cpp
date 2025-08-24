// Source/Tests/RunAllTests.cpp
#include <JuceHeader.h>
#include <iostream>

// External test function declarations
extern int TestProcessor_FilterIntegration();
extern int TestSpectralSmoke(); 
extern int TestAPVTSParams();
extern int RenderSineFFTTest();

// Minimal JUCE UnitTest console runner
int main (int argc, char* argv[])
{
    juce::ignoreUnused (argc, argv);

    // Initialize JUCE for console application
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    juce::UnitTestRunner runner;
    runner.setAssertOnFailure (true);

    // Run all registered JUCE tests (if any)
    runner.runAllTests(); 

    // Count total failures across all test results
    int totalFailures = 0;
    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        if (auto* result = runner.getResult(i))
            totalFailures += result->failures;
    }
    
    // Run C-style tests
    std::cout << "\n=== Running C-style tests ===" << std::endl;
    totalFailures += TestProcessor_FilterIntegration();
    totalFailures += TestSpectralSmoke();
    totalFailures += TestAPVTSParams();
    totalFailures += RenderSineFFTTest();
    
    std::cout << "\n=== All tests completed ===" << std::endl;
    std::cout << "Total failures: " << totalFailures << std::endl;
    
    return totalFailures == 0 ? 0 : 1;
}
