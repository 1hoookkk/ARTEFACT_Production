#pragma once
#include <JuceHeader.h>

namespace UIColour
{
    // Core palette (Phase 2 Scoped)
    inline const juce::Colour Background    = juce::Colour::fromRGB (10, 9, 8);    // #0A0908
    inline const juce::Colour GridLine      = juce::Colour::fromRGB (40, 40, 40);
    inline const juce::Colour PianoKey      = juce::Colour::fromRGB (30, 30, 30);
    inline const juce::Colour PianoKeyActive= juce::Colour::fromRGB (255, 165, 0); // amber
    inline const juce::Colour PercColor     = juce::Colour::fromRGB (0, 212, 255);  // cyan
    inline const juce::Colour HarmColor     = juce::Colour::fromRGB (255, 107, 53);  // burnt orange
}