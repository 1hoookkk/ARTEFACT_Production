#pragma once
#include <juce_graphics/juce_graphics.h>

namespace ui {

struct Tokens {
    // Panel colors
    juce::Colour panel       = juce::Colour::fromString("ff3F87B3"); // Bright Audity blue
    juce::Colour panelDark   = juce::Colour::fromString("ff2b5f7f");
    juce::Colour panelHi     = juce::Colour::fromString("ff6aa6c8");
    
    // LCD/CRT colors  
    juce::Colour lcdBase     = juce::Colour::fromString("ff104a2e");
    juce::Colour lcdLight    = juce::Colour::fromString("ff1f6b45");
    juce::Colour lcdScan     = juce::Colours::black.withAlpha(0.08f);
    
    // Control colors
    juce::Colour creamKnob   = juce::Colour::fromString("ffe8e1d2");
    juce::Colour creamShadow = juce::Colour::fromString("ff9a9285");
    juce::Colour ledAmber    = juce::Colour::fromString("fff6a623");
    
    // Realm identity colors
    juce::Colour realmBlue   = juce::Colour::fromString("ff1e90ff");
    juce::Colour realmPurple = juce::Colour::fromString("ff7d4ed8");
    juce::Colour realmYellow = juce::Colour::fromString("fff5c542");
    
    // Text
    juce::Colour hudText     = juce::Colours::white.withAlpha(0.9f);
    juce::Colour labelText   = juce::Colours::white.withAlpha(0.7f);
};

// Realm-specific token overrides
inline Tokens blueRealm() {
    return Tokens{}; // Default is blue
}

inline Tokens yellowRealm() { 
    Tokens t;
    t.panel = juce::Colour::fromString("fff5c542");
    t.panelDark = juce::Colour::fromString("ffb78f16");
    t.panelHi = t.panel.brighter(0.2f);
    return t;
}

inline Tokens purpleRealm() {
    Tokens t;
    t.panel = juce::Colour::fromString("ff6c3fa0");
    t.panelDark = juce::Colour::fromString("ff4a2b70");
    t.panelHi = t.panel.brighter(0.25f);
    return t;
}

// Get tokens for a realm index
inline Tokens getTokensForRealm(int realmIndex) {
    switch (realmIndex) {
        case 0: return blueRealm();
        case 1: return purpleRealm();
        case 2: return yellowRealm();
        default: return blueRealm();
    }
}

} // namespace ui