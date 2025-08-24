/**
 * Test Runner for SpectralCanvas Unit Tests
 */

#include <JuceHeader.h>

// Main function for console app
int main()
{
    // Initialize JUCE
    juce::ScopedJuceInitialiser_GUI scopedJuce;
    
    // Create and run test runner
    juce::UnitTestRunner runner;
    runner.setPassesAreLogged(true);
    runner.setAssertOnFailure(false);
    
    // Run all tests
    juce::String results;
    runner.runAllTests();
    
    // Get results
    int numTests = runner.getNumResults();
    int failures = 0;
    
    for (int i = 0; i < numTests; ++i)
    {
        auto* result = runner.getResult(i);
        if (result)
        {
            failures += result->failures;
            
            // Print results
            std::cout << "\n" << result->unitTestName.toStdString() << ":\n";
            for (const auto& message : result->messages)
            {
                std::cout << "  " << message.toStdString() << "\n";
            }
        }
    }
    
    // Summary
    std::cout << "\n========================================\n";
    std::cout << "Test Results:\n";
    std::cout << "  Total Tests: " << numTests << "\n";
    std::cout << "  Failures: " << failures << "\n";
    std::cout << "  Status: " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
    std::cout << "========================================\n";
    
    return failures == 0 ? 0 : 1;
}