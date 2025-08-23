#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>
#include <juce_dsp/juce_dsp.h>

#include "../Source/Core/SpectralSynthEngine.h"

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    try
    {
        // Construct SpectralSynthEngine with NO updateCanvasBounds() call
        SpectralSynthEngine engine;
        
        std::cout << "Constructed SpectralSynthEngine without updateCanvasBounds() call\n";
        
        // Call processPaintStrokeDirectToAudio
        // Expected: Should crash/fail due to hardcoded 600.0f divide in line 71
        std::cout << "Calling processPaintStrokeDirectToAudio(300, 300, 0.5, white)...\n";
        
        engine.processPaintStrokeDirectToAudio(300.0f, 300.0f, 0.5f, juce::Colours::white);
        
        std::cout << "UNEXPECTED: processPaintStrokeDirectToAudio succeeded without updateCanvasBounds\n";
        std::cout << "This test should have failed due to divide-by-zero or invalid bounds\n";
        
        return 0; // Return 0 to indicate test completed (but unexpectedly)
    }
    catch (const std::exception& e)
    {
        std::cout << "EXPECTED: Caught exception: " << e.what() << "\n";
        return 1; // Expected failure
    }
    catch (...)
    {
        std::cout << "EXPECTED: Caught unknown exception\n";
        return 1; // Expected failure
    }
}