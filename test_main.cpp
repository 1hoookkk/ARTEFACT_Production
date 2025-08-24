// Simple main wrapper for RenderSineFFTTest
#include <JuceHeader.h>

extern int RenderSineFFTTest();

int main()
{
    // Initialize JUCE for console application
    juce::ScopedJuceInitialiser_GUI juceInitialiser;
    
    // Run the test
    return RenderSineFFTTest();
}