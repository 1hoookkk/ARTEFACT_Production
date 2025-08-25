#pragma once
#include <JuceHeader.h>

namespace UIColour
{
    // Core palette (Phase 2 Scoped)
    inline const juce::Colour Background    = juce::Colour::fromRGB (10, 9, 8);    // #0A0908
    inline const juce::Colour GridLine      = juce::Colour::fromRGB (40, 40, 40);
    inline const juce::Colour PianoKey      = juce::Colour::fromRGB (30, 30, 30);
    inline const juce::Colour PianoKeyActive= juce::Colour::fromRGB (255, 165, 0); // amber
    
    // HPSS Color System (Drum Focus)
    inline const juce::Colour PercColor     = juce::Colour::fromRGB (0, 212, 255);  // Electric Cyan #00D4FF
    inline const juce::Colour HarmColor     = juce::Colour::fromRGB (255, 107, 53);  // Burnt Orange #FF6B35
    
    // Musical Context Colors
    inline const juce::Colour NoteLineC     = juce::Colour::fromRGB (255, 107, 53); // #FF6B35 @ 40%
    inline const juce::Colour NoteLineOther = juce::Colour::fromRGB (255, 107, 53); // #FF6B35 @ 20%
    inline const juce::Colour ScaleInKey    = juce::Colour::fromRGB (255, 165, 0);  // #FFA500 @ 15%
    inline const juce::Colour ScaleRoot     = juce::Colour::fromRGB (255, 165, 0);  // #FFA500 @ 25%
    inline const juce::Colour GhostSnap     = juce::Colour::fromRGB (245, 242, 237); // #F5F2ED @ 60%
    
    // Overtone Guide Colors
    inline const juce::Colour OvertoneGuide = juce::Colour::fromRGB (255, 107, 53); // #FF6B35 @ 30%
    
    // Chord Recognition
    inline const juce::Colour ChordLabel    = juce::Colour::fromRGB (255, 165, 0);  // #FFA500
    
    // Export Pod Colors
    inline const juce::Colour ExportReady   = juce::Colour::fromRGB (0, 255, 0);    // Green for ready state
    inline const juce::Colour ExportRendering = juce::Colour::fromRGB (255, 165, 0); // Orange for rendering
}